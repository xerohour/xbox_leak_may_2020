//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     nvsubch.h
//
// Abstract:
//
//     This module contains the object subchannel definitions
//
// Environment:
//
//     Kernel mode
//
// Revision History:
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#define CLIP_SUBCHANNEL                6
#define IMAGE_PATTERN_SUBCHANNEL       5
#define ROP_SOLID_SUBCHANNEL           4

#define RECTANGLE_SUBCHANNEL           3
#define LUT_CURSOR_DAC_SUBCHANNEL      3                
#define IMAGE_MONO_FROM_CPU_SUBCHANNEL 2
#define RECT_AND_TEXT_SUBCHANNEL       2
#define BLIT_SUBCHANNEL                1
#define IMAGE_FROM_CPU_SUBCHANNEL      0
#define DD_SPARE                       7
#define DD_ROP_RECT_AND_TEXT           2
#define DD_STRETCH                     DD_SPARE

#define SOLID_LIN_SUBCHANNEL           7
//******************************************************************************
// OpenGL SubChannel assignments for objects
//******************************************************************************

#define TRIANGLE_SUBCHANNEL            0
#define D3DTRIANGLE_SUBCHANNEL         0
#define CLEARZ_SUBCHANNEL              2
#define SRCIMAGE_IN_MEMORY_SUBCHANNEL  5
#define D3DTRIANGLE_SYSMEM_SUBCHANNEL  4
#define RENDER_SOLID_LIN_SUBCHANNEL    0

//******************************************************************************
// My 32-bit names for the STATIC image objects
//******************************************************************************

#define MY_IMAGE_SOLID                 0x99990001
#define MY_BLACK_RECTANGLE             0x99990002
#define MY_IMAGE_PATTERN               0x99990003
#define MY_ROP_SOLID                   0x99990004

//******************************************************************************
// My 32-bit names for RENDERING type objects
//******************************************************************************

#define MY_RENDER_SOLID_RECTANGLE      0x34000001
#define MY_RENDER_SOLID_TRIANGLE       0x34000002
#define MY_RENDER_SCRN_TO_SCRN_BLIT    0x34000003
#define MY_IMAGE_FROM_CPU_BLIT         0x34000004
#define MY_IMAGE_MONO_FROM_CPU_BLIT    0x34000005
#define MY_RENDER_D3D_TRIANGLE         0x34000006
#define MY_CLEARZ_RECTANGLE            0x34000007
#define MY_SRCIMAGE_IN_MEMORY          0x34000008
#define MY_DSTIMAGE_IN_MEMORY          0x34000009
#define MY_MEMORY_TO_MEMORY            0x3400000A
#define MY_RENDER_D3D_SYSMEM_TRIANGLE  0x3400000B
#define MY_RENDER_D3D_TRIANGLE_SYSMEM  MY_RENDER_D3D_SYSMEM_TRIANGLE
#define MY_RENDER_SOLID_LIN            0x3400000C
#define MY_STENCIL_TRIANGLE            0x3400000D
#define MY_STENCIL_LIN                 0x3400000E
#define MY_STENCIL_SYSMEM_TRIANGLE     0x3400000F

//****************************************************************************
// object id's (only used by Release driver)
//****************************************************************************
#define DD_IMAGE_FROM_CPU               0x000A0000
#define DD_IMAGE_BLIT                   0x000A0001
#define DD_IMAGE_MONO                   0x000A0002
#define DD_RENDER_SOLID_RECTANGLE       0x000A0003
//#define DD_ROP_SOLID                  0x000A0004 undefined for RIVA (jsw)
#define DD_ROP5_SOLID                   0x000A0004
#define DD_IMAGE_PATTERN                0x000A0005
#define DD_IMAGE_BLACK_RECTANGLE        0x000A0006
#define DD_IMAGE_SOLID                  0x000A0007
#define DD_RENDER_RECT_AND_TEXT         0x000A0008
#define DD_IMAGE_ROP_AND0               0x000A0010
#define DD_IMAGE_ROP_AND1               0x000A0011
#define DD_IMAGE_ROP_AND2               0x000A0012
#define DD_IMAGE_ROP_AND3               0x000A0013
#define DD_IMAGE_ROP_AND4               0x000A0014
#define DD_IMAGE_ROP_AND5               0x000A0015
#define DD_IMAGE_ROP_AND6               0x000A0016
#define DD_IMAGE_STENCIL0               0x000A0020
#define DD_IMAGE_STENCIL1               0x000A0021
#define DD_IMAGE_STENCIL2               0x000A0022
#define DD_IMAGE_STENCIL3               0x000A0023
#define DD_IMAGE_STENCIL4               0x000A0024
#define DD_IMAGE_STENCIL5               0x000A0025
#define DD_IMAGE_STENCIL6               0x000A0026
#define DD_IMAGE_COLOR_KEY0             0x000A0030
#define DD_IMAGE_COLOR_KEY1             0x000A0031
#define DD_IMAGE_COLOR_KEY2             0x000A0032
#define DD_IMAGE_COLOR_KEY3             0x000A0033
#define DD_IMAGE_COLOR_KEY4             0x000A0034
#define DD_IMAGE_COLOR_KEY5             0x000A0035
#define DD_IMAGE_COLOR_KEY6             0x000A0036
#define DD_IMAGE_TO_VIDEO               0x000A00E0
#define DD_VIDEO_SINK                   0x000A00F0
#define DD_PRIMARY_VIDEO_FROM_MEMORY    0x000A00F1
#define DD_PRIMARY_IMAGE_IN_MEMORY      0x000A00F2
#define DD_COLORMAP                     0x000A00F3
#define DD_SRC_IMAGE_IN_MEMORY          0x000A00F4
#define DD_ZETA_BUFFER                  0x000A00F5
#define DD_RENDER_D3D_TRIANGLE          0x000A00F6
#define DD_IMAGE_FROM_MEMORY            0x000A00F7
#define DD_ZBUFFER_IMAGE_IN_MEMORY      0x000A00F8
#define DD_IMAGE_ROP_AND_LIN            0x000A00F9
#define DD_P_V_SHARED_VIDEO_COLORMAP    0x000A00FA
#define DD_INDEXED_IMAGE_FROM_CPU       0x000A00FB
#define DD_INDEXED_IMAGE1_FROM_CPU      0x000A00FC
#define DD_INDEXED_IMAGE2_FROM_CPU      0x000A00FD
#define DD_INDEXED_IMAGE3_FROM_CPU      0x000A00FE
#define DD_INDEXED_IMAGE4_FROM_CPU      0x000A00FF
#define DD_INDEXED_IMAGE5_FROM_CPU      0x000A0100
#define DD_INDEXED_IMAGE6_FROM_CPU      0x000A0101
#define DD_INDEXED_IMAGE7_FROM_CPU      0x000A0102
#define DD_INDEXED_IMAGE8_FROM_CPU      0x000A0103
#define DD_INDEXED_IMAGE9_FROM_CPU      0x000A0104
#define DD_INDEXED_IMAGE10_FROM_CPU     0x000A0105
#define DD_INDEXED_IMAGE11_FROM_CPU     0x000A0106
#define DD_INDEXED_IMAGE12_FROM_CPU     0x000A0107
#define DD_INDEXED_IMAGE13_FROM_CPU     0x000A0108
#define DD_INDEXED_IMAGE14_FROM_CPU     0x000A0109
#define DD_INDEXED_IMAGE15_FROM_CPU     0x000A010A
#define DD_INDEXED_IMAGE16_FROM_CPU     0x000A010B
#define DD_SCALED_IMAGE_FROM_MEMORY     0x000A0110
#define DD_IMAGE_BLIT_COLORKEY          0x000A0111

// patchcord id's
#define DD_PATCHCORD_IMAGE_FROM_CPU     0x000B0000
#define DD_PATCHCORD_IMAGE_BLIT         0x000B0001
#define DD_PATCHCORD_IMAGE_MONO         0x000B0002
#define DD_PATCHCORD_IMAGE_SOLID_RECT   0x000B0003
#define DD_PATCHCORD_ROP5_SOLID         0x000B0004
#define DD_PATCHCORD_IMAGE_PATTERN      0x000B0005
#define DD_PATCHCORD_IMAGE_BLACK_RECT   0x000B0006
#define DD_PATCHCORD_IMAGE_SOLID        0x000B0007
#define DD_PATCHCORD_IMAGE_RECT_AND_TEXT 0x000B0008
#define DD_PATCHCORD_IMAGE_ROP_AND0     0x000B0010
#define DD_PATCHCORD_IMAGE_ROP_AND1     0x000B0011
#define DD_PATCHCORD_IMAGE_ROP_AND2     0x000B0012
#define DD_PATCHCORD_IMAGE_ROP_AND3     0x000B0013
#define DD_PATCHCORD_IMAGE_ROP_AND4     0x000B0014
#define DD_PATCHCORD_IMAGE_ROP_AND5     0x000B0015
#define DD_PATCHCORD_IMAGE_ROP_AND6     0x000B0016
#define DD_PATCHCORD_IMAGE_STENCIL0     0x000B0020
#define DD_PATCHCORD_IMAGE_STENCIL1     0x000B0021
#define DD_PATCHCORD_IMAGE_STENCIL2     0x000B0022
#define DD_PATCHCORD_IMAGE_STENCIL3     0x000B0023
#define DD_PATCHCORD_IMAGE_STENCIL4     0x000B0024
#define DD_PATCHCORD_IMAGE_STENCIL5     0x000B0025
#define DD_PATCHCORD_IMAGE_STENCIL6     0x000B0026
#define DD_PATCHCORD_IMAGE0             0x000B0030
#define DD_PATCHCORD_IMAGE1             0x000B0031
#define DD_PATCHCORD_IMAGE2             0x000B0032
#define DD_PATCHCORD_IMAGE3             0x000B0033
#define DD_PATCHCORD_IMAGE4             0x000B0034
#define DD_PATCHCORD_IMAGE5             0x000B0035
#define DD_PATCHCORD_IMAGE6             0x000B0036
#define DD_PATCHCORD_IMAGE_FEEDBACK     0x000B00E0
#define DD_PATCHCORD_VIDEO              0x000B00E1
#define DD_PATCHCORD_COLORMAP           0x000B00E2
#define DD_PATCHCORD_SRC_FEEDBACK       0x000B00E3
#define DD_PATCHCORD_ZETA_IN            0x000B00E4
#define DD_PATCHCORD_ZETA_OUT           0x000B00E5
#define DD_PATCHCORD_IMAGE_TRIANGLE     0x000B00E6
#define DD_PATCHCORD_CLEAR_ZBUFFER      0x000B00E7
#define DD_PATCHCORD_IMAGE_SYSMEM_TRIANGLE 0x000B00E8
#define DD_PATCHCORD_ZETA_OUT_SYSMEM    0x000B00E9
#define DD_PATCHCORD_IMAGE_LIN          0x000B00EA
#define DD_PATCHCORD_STENCIL_TRIANGLE   0x000B00EB
#define DD_PATCHCORD_STENCIL_LIN        0x000B00EC
#define DD_PATCHCORD_STENCIL_SYSMEM_TRIANGLE   0x000B00ED

#define DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY    0x00001004
#define NV_WIN_NOTIFIER_CONTEXT_DMA_TO_MEMORY       0x00001005
#define NV_WIN_COLORMAP_CONTEXT                     0x00001010
#define NV_WIN_COLORMAP_CONTEXT_1                   0x00001011
#define NV_WIN_COLORMAP_CONTEXT_2                   0x00001012
#define NV_WIN_COLORMAP_CONTEXT_3                   0x00001013
#define NV_WIN_CONTEXT_DMA_FROM_MEMORY              0x00001015
#define DD_CONTEXT_DMA_NOTIFIER_TRIANGLE            0x00001017
#define DD_NOTIFIER_CONTEXT_DMA_MEMORY_TO_MEMORY_FORMAT 0x00001021
#define DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM          0x00001022
#define DD_CONTEXT_DMA_SYSMEM_TEXTURE_BUFFER        0x00001023
#define DD_NOTIFIER_CONTEXT_DMA_SCALED_IMAGE        0x00001024
#define DD_FIFO_SYNC_NOTIFIER                       0x00001025
#define DD_MEMORY_TO_MEMORY_CONTEXT_DMA_NOTIFIER    0x00001026
#define DD_TEMP_BUFFER_CONTEXT_DMA                  0x00001027
#define NV_WIN_INDEXED_IMAGE_CONTEXT                0x00001028
#define NV_DD_PRIMARY_2D_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x00001029
#define NV_VIDEO_LUT_CURSOR_DAC                     0x00001030
#define NV_VIDEO_LUT_CURSOR_DAC_2                   0x00001031
#define NV_VIDEO_LUT_CURSOR_DAC_3                   0x00001032
#define NV_VIDEO_LUT_CURSOR_DAC_4                   0x00001033
#define NV_CONTEXT_DMA_CURSOR                       0x00001034
#define NV_CONTEXT_DMA_CURSOR_2                     0x00001035
#define NV_CONTEXT_DMA_VIDEO_LUT_CURSOR_DAC_NOTIFIER     0x00001036

//****************************************************************************
// Dma push specific names
//****************************************************************************

#define NV_CONTEXT_DMA_FROM_MEMORY_PUSHBUF    0x000C0000
#define NV_CONTEXT_DMA_PUSHBUF_ERROR_NOTIFIER 0x000C0001

//***************************************************************************************
// NV object names used by the ddraw driver.
//***************************************************************************************
/* Windows 95 Direct Draw Object IDs */
#define NV_DD_DST_IMAGE_IN_MEMORY                               0x80023F00
#define NV_DD_P_I_DST_IMAGE_IN_MEMORY                           0x80023F01
#define NV_DD_SRC_IMAGE_IN_MEMORY                               0x80023F10
#define NV_DD_P_I_SRC_IMAGE_IN_MEMORY                           0x80023F11
#define NV_DD_ZETA_IMAGE_IN_MEMORY                              0x80023F20
#define NV_D3D_ZETA_BUFFER_IN_MEMORY                            0x80023F30
#define NV_D3D_P_Z_ZETA_BUFFER_IN_MEMORY                        0x80023F31

// ELAU from WIN9x
/* Video channel shared Display Driver and DDraw Object IDs */
#define NV_VIDEO_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0x00008008
#define NV_VIDEO_OVERLAY                                        0x00008030

#define NV_DD_CONTEXT_DMA_IN_MEMORY_VIDEO_MEM                   0x00002069
#define NV_DD_CONTEXT_DMA_FROM_SYSTEM_MEMORY                    0x00002070
#define NV_DD_WIN_COLORMAP_CONTEXT                              0x00002071
#define NV_DD_TO_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY             0x00002072

#define NV_DD_FLOATING_FORMAT_CONTEXT_DMA_FROM_SYSTEM_MEMORY    0x80021001
#define NV_DD_CONTEXT_DMA_FROM_VIDEO_MEMORY                     0x80021002
#define NV_DD_FLOATING0_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021003
#define NV_DD_FLOATING1_UYVY_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021004
#define NV_DD_FLOATING0_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021005
#define NV_DD_FLOATING1_YUYV_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021006
#define NV_DD_FLOATING0_Y420_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021007
#define NV_DD_FLOATING1_Y420_CONTEXT_DMA_FROM_SYSTEM_MEMORY     0x80021008
#define NV_DD_TEXTURE_CONTEXT_DMA_FROM_SYSTEM_MEMORY            0x80021009
#define NV_DD_FLOATING_TEXTURE_CONTEXT_DMA_FROM_SYSTEM_MEMORY   0x8002100A
#define NV_DD_TRIANGLE_CONTEXT_DMA_FROM_SYSTEM_MEMORY           0x8002100B
#define DD_CONTEXT_DMA_FLIP_NOTIFIER                            0x8002100B
#define NV_DD_CONTEXT_DMA_IN_MEMORY                             0x80021010
#define NV_DD_FLIP_PRIMARY_NOTIFIER_CONTEXT_DMA_TO_MEMORY       0x80021016
#define NV_DD_FLIP_OVERLAY_NOTIFIER_CONTEXT_DMA_TO_MEMORY       0x80021017
#define NV_DD_FLIP_VPORT_NOTIFIER_CONTEXT_DMA_TO_MEMORY         0x80021018
#define NV_DD_FROM_MEM_NOTIFIER_CONTEXT_DMA_TO_MEMORY           0x80021019
#define NV_DD_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY        0x8002101A
#define NV_DD_DUMMY_NOTIFIER_CONTEXT_DMA_TO_MEMORY              0x8002101B
#define NV_DD_CONTEXT_DMA_TO_VIDEO_MEMORY                       0x8002101C
#define NV_DD_VIDEO_MEM_TO_SYSTEM_MEM_FORMAT                    0x80021020
#define NV_DD_ROP5_SOLID                                        0x80021030
#define NV_DD_P_I_ROP5_SOLID                                    0x80021031
#define NV_DD_BETA_SOLID                                        0x80021040
#define NV_DD_P_B_BETA_SOLID                                    0x80021041
#define NV_DD_IMAGE_PATTERN                                     0x80021050
#define NV_DD_P_I_IMAGE_PATTERN                                 0x80021051
#define NV_DD_IMAGE_BLACK_RECTANGLE                             0x80021060
#define NV_DD_P_I_IMAGE_BLACK_RECTANGLE                         0x80021061
#define NV_DD_COLOR_KEY_IMAGE_SOLID                             0x80021070
#define NV_DD_P_I_COLOR_KEY_IMAGE_SOLID                         0x80021071
#define NV_DD_FROM_MEM_INDEXED_IMAGE_NOTIFIER_CONTEXT_DMA_TO_MEMORY   0x80021072
#define NV_DD_FROM_MEM_INDEXED_IMAGE1_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021073
#define NV_DD_FROM_MEM_INDEXED_IMAGE2_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021074
#define NV_DD_FROM_MEM_INDEXED_IMAGE3_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021075
#define NV_DD_FROM_MEM_INDEXED_IMAGE4_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021076
#define NV_DD_FROM_MEM_INDEXED_IMAGE5_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021077
#define NV_DD_FROM_MEM_INDEXED_IMAGE6_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021078
#define NV_DD_FROM_MEM_INDEXED_IMAGE7_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x80021079
#define NV_DD_FROM_MEM_INDEXED_IMAGE8_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x8002107A
#define NV_DD_FROM_MEM_INDEXED_IMAGE9_NOTIFIER_CONTEXT_DMA_TO_MEMORY  0x8002107B
#define NV_DD_FROM_MEM_INDEXED_IMAGE10_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x8002107C
#define NV_DD_FROM_MEM_INDEXED_IMAGE11_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x8002107D
#define NV_DD_FROM_MEM_INDEXED_IMAGE12_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x8002107E
#define NV_DD_FROM_MEM_INDEXED_IMAGE13_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x8002107F
#define NV_DD_FROM_MEM_INDEXED_IMAGE14_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x80021080
#define NV_DD_FROM_MEM_INDEXED_IMAGE15_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x80021081
#define NV_DD_FROM_MEM_INDEXED_IMAGE16_NOTIFIER_CONTEXT_DMA_TO_MEMORY 0x80021082

#define NV_DD_ROP_RECT_AND_TEXT_FROM_CPU                        0x80021100
#define NV_DD_P_I_ROP_RECT_AND_TEXT_FROM_CPU                    0x80021101
#define NV_DD_SYNC_RECT_AND_TEXT_FROM_CPU                       0x80021108
#define NV_DD_P_I_SYNC_RECT_AND_TEXT_FROM_CPU                   0x80021109
#define NV_DD_PUSHER_SYNC_RECT_AND_TEXT_FROM_CPU                0x8002110A
#define NV_DD_P_I_PUSHER_SYNC_RECT_AND_TEXT_FROM_CPU            0x8002110B
#define NV_DD_IMAGE_FROM_CPU                                    0x80021200
#define NV_DD_P_I_IMAGE_FROM_CPU                                0x80021201
#define NV_DD_IMAGE_BLEND_FROM_CPU                              0x80021300
#define NV_DD_P_I_IMAGE_BLEND_FROM_CPU                          0x80021301
#define NV_DD_STRETCHED_IMAGE_FROM_CPU                          0x80021400
#define NV_DD_P_I_STRETCHED_IMAGE_FROM_CPU                      0x80021401
#define NV_DD_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY               0x80021500
#define NV_DD_P_I_SCALED_RGB_IMAGE_FROM_SYSTEM_MEMORY           0x80021501
#define NV_DD_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY                0x80021510
#define NV_DD_P_I_SCALED_RGB_IMAGE_FROM_VIDEO_MEMORY            0x80021511
#define NV_DD_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY              0x80021520
#define NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY          0x80021521
#define NV_DD_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY               0x80021530
#define NV_DD_P_I_SCALED_UYVY_IMAGE_FROM_VIDEO_MEMORY           0x80021531
#define NV_DD_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY              0x80021540
#define NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY          0x80021541
#define NV_DD_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY               0x80021550
#define NV_DD_P_I_SCALED_YUYV_IMAGE_FROM_VIDEO_MEMORY           0x80021551
#define NV_DD_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY    0x80021560
#define NV_DD_P_I_FLOATING0_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY 0x80021561
#define NV_DD_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY    0x80021570
#define NV_DD_P_I_FLOATING1_SCALED_UYVY_IMAGE_FROM_SYSTEM_MEMORY 0x80021571
#define NV_DD_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY    0x80021580
#define NV_DD_P_I_FLOATING0_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY 0x80021581
#define NV_DD_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY    0x80021590
#define NV_DD_P_I_FLOATING1_SCALED_YUYV_IMAGE_FROM_SYSTEM_MEMORY 0x80021591
#define NV_DD_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY    0x800215A0
#define NV_DD_P_I_FLOATING0_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY 0x800215A1
#define NV_DD_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY    0x800215B0
#define NV_DD_P_I_FLOATING1_SCALED_Y420_IMAGE_FROM_SYSTEM_MEMORY 0x800215B1
#define NV_DD_STRETCHED_UV_IMAGE_FROM_CPU                       0x80021600
#define NV_DD_P_I_STRETCHED_UV_IMAGE_FROM_CPU                   0x80021601

#define NV_DD_IMAGE_BLIT                                        0x80021800
#define NV_DD_P_I_IMAGE_BLIT                                    0x80021801
#define NV_DD_ALT_IMAGE_FROM_CPU                                0x80021900
#define NV_DD_P_I_ALT_IMAGE_FROM_CPU                            0x80021901
#define NV_DD_RENDER_SOLID_ZETA_RECTANGLE                       0x80021A00
#define NV_DD_P_I_RENDER_SOLID_ZETA_RECTANGLE                   0x80021A01

#define NV_D3D_RENDER_TRIANGLE_ZETA                             0x80021B00
#define NV_D3D_P_I_RENDER_TRIANGLE_ZETA                         0x80021B01
#define NV_D3D_P_Z_RENDER_TRIANGLE_ZETA                         0x80021B02
#define NV_D3D_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY           0x80021B03
#define NV_D3D_P_I_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY       0x80021B04
#define NV_D3D_P_Z_RENDER_TRIANGLE_ZETA_FROM_VIDEO_MEMORY       0x80021B05
#define NV_D3D_FLOATING_RENDER_TRIANGLE_ZETA                    0x80021B06
#define NV_D3D_FLOATING_P_I_RENDER_TRIANGLE_ZETA                0x80021B07
#define NV_D3D_FLOATING_P_Z_RENDER_TRIANGLE_ZETA                0x80021B08

#define NV_DD_VIDEO_COLOR_KEY                                   0x80023FB0
#define NV_DD_YUV422_VIDEO_FROM_MEMORY                          0x80023F90
#define NV_DD_PRIMARY_VIDEO_FROM_MEMORY                         0x80023F80
#define NV_DD_P_V_PRIMARY_VIDEO_FROM_MEMORY                     0x80023F81
#define NV_DD_YUV422_VIDEO_FROM_MEMORY                          0x80023F90
#define NV_DD_P_V_YUV422_VIDEO_FROM_MEMORY                      0x80023F91
#define NV_DD_VIDEO_OVERLAY                                     0x80023F92
#define NV_DD_VIDEO_LUT_CURSOR_DAC                              0x80023F93
#define NV_DD_VIDEO_LUT_CURSOR_DAC_2                            0x80023F94
#define NV_DD_VIDEO_LUT_CURSOR_DAC_3                            0x80023F95
#define NV_DD_VIDEO_LUT_CURSOR_DAC_4                            0x80023F96
#define NV_DD_VIDEO_SCALER                                      0x80023FA0
#define NV_DD_P_V_VIDEO_SCALER                                  0x80023FA1
#define NV_DD_VIDEO_COLOR_KEY                                   0x80023FB0
#define NV_DD_P_V_VIDEO_COLOR_KEY                               0x80023FB1
#define NV_DD_SHARED_VIDEO_COLORMAP                             0x80023FC0
#define NV_DD_P_V_SHARED_VIDEO_COLORMAP                         0x80023FC1

#define NV_DD_SYSTEM_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT            0x80023FD0
#define NV_DD_SYSTEM_MEMORY_U_TO_VIDEO_MEMORY_FORMAT            0x80023FD1
#define NV_DD_SYSTEM_MEMORY_V_TO_VIDEO_MEMORY_FORMAT            0x80023FD2
#define NV_DD_SYSTEM_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT           0x80023FD3
#define NV_DD_VIDEO_MEMORY_Y_TO_VIDEO_MEMORY_FORMAT             0x80023FE0
#define NV_DD_VIDEO_MEMORY_U_TO_VIDEO_MEMORY_FORMAT             0x80023FE1
#define NV_DD_VIDEO_MEMORY_V_TO_VIDEO_MEMORY_FORMAT             0x80023FE2
#define NV_DD_VIDEO_MEMORY_UV_TO_VIDEO_MEMORY_FORMAT            0x80023FE3
#define NV_DD_VIDEO_SINK                                        0x80023FF0
#define NV_DD_CONTEXT_BETA                                      0x80023FF1
#define NV_DD_CONTEXT_BETA4                                     0x80023FF2
#define NV_DD_PRIMARY_SURFACE                                   0x80024000
#define NV_DD_SRC_SURFACE                                       0x80024001
#define NV_DD_IMAGE_SOLID                                       0x80024003
#define NV_DD_RENDER_RECT_AND_TEXT                              0x80024004
#define NV_DD_RENDER_SOLID_RECTANGLE                            0x80024005

// object handle definitions for the new architure
// values start after DMA pushbuf id's
#define DD_CLIENT_OBJECT_HANDLE                 0x000C0002
#ifndef NVD3D
#define DD_DEVICE_OBJECT_HANDLE                 0x000C0003
#else
//
// Use the same device handle as the DX6 driver.
//
#define DD_DEVICE_OBJECT_HANDLE                 0x00002002
#endif // #ifdef NVD3D
#define DD_PIO_CHANNEL_OBJECT_HANDLE            0x000C0004
#define DD_DMA_CHANNEL_OBJECT_HANDLE            0x000C0005
#define DD_FRAME_BUFFER_OBJECT_HANDLE           0x000C0006
#define DD_CURSOR_OBJECT_HANDLE                 0x000C0007
#define NV_DD_DDRAW_PIO_CHANNEL_OBJECT_HANDLE   0x000C0008
#define NV_AGP_PUSHBUF_OBJECT_HANDLE            0x000C0009
#define NV_SYS_PUSHBUF_OBJECT_HANDLE            0x000C000A
#define NV_NOTIFIERS_OBJECT_HANDLE              0x000C000B

//***************************************************************************
// Macros used to create NV objects via the IOCTLs.
//***************************************************************************
#define NV_CREATE_OBJECT_SETUP() \
    NvU32 createObjFail, createObjClass
#define NV_CREATE_OBJECT_INIT() createObjFail = 0
#define NV_CREATE_OBJECT_FAIL() createObjFail
#define NV_CREATE_OBJECT_CLASS() createObjClass

#define NV_CREATE_OBJECT_PIO1(CLASS1, OBJ) {                                 \
    NvU32 classes[] = {CLASS1};                                              \
    NvCreateObject(ppdev,FALSE, NULL, OBJ, &createObjClass, &createObjFail, 1, classes); \
}    

#define NV_CREATE_OBJECT_PIO2(CLASS1, CLASS2, OBJ) {                         \
    NvU32 classes[] = {CLASS1,CLASS2};                                       \
    NvCreateObject(ppdev,FALSE, NULL, OBJ, &createObjClass, &createObjFail, 2, classes); \
}    

#define NV_CREATE_OBJECT_PARM_PIO3(PARM, OBJ, CLASS1, CLASS2, CLASS3) {                             \
    NvU32 classes[] = {CLASS1,CLASS2,CLASS3};                                            \
    NvCreateObject(ppdev,FALSE, PARM, OBJ, &createObjClass, &createObjFail, 3, classes); \
}    

#define NV_CREATE_DMA1(PARMS,OBJ,CLASS1) {                                          \
    NvU32 classes[] = {CLASS1};                                                     \
    NvCreateObject(ppdev,TRUE, PARMS, OBJ, &createObjClass, &createObjFail, 1, classes);    \
}
#define NV_CREATE_DMA2(PARMS,OBJ,CLASS1,CLASS2) {                                   \
    NvU32 classes[] = {CLASS1,CLASS2};                                              \
    NvCreateObject(ppdev,TRUE, PARMS, OBJ, &createObjClass, &createObjFail, 2, classes);    \
}
#define NV_CREATE_DMA3(PARMS,OBJ,CLASS1,CLASS2,CLASS3) {                            \
    NvU32 classes[] = {CLASS1,CLASS2,CLASS3};                                       \
    NvCreateObject(ppdev,TRUE, PARMS, OBJ, &createObjClass, &createObjFail, 3, classes);    \
}
#define NV_CREATE_DMA4(PARMS,OBJ,CLASS1,CLASS2,CLASS3,CLASS4) {                     \
    NvU32 classes[] = {CLASS1,CLASS2,CLASS3,CLASS4};                                \
    NvCreateObject(ppdev,TRUE, PARMS, OBJ, &createObjClass, &createObjFail, 4, classes);    \
}
#define NV_CREATE_DMA5(PARMS,OBJ,CLASS1,CLASS2,CLASS3,CLASS4,CLASS5) {              \
    NvU32 classes[] = {CLASS1,CLASS2,CLASS3,CLASS4,CLASS5};                         \
    NvCreateObject(ppdev,TRUE, PARMS, OBJ, &createObjClass, &createObjFail, 5, classes);    \
}

#define NV_CREATE_OBJECT_DMA1(OBJ,CLASS1) \
        NV_CREATE_DMA1(NULL,OBJ,CLASS1);
#define NV_CREATE_OBJECT_DMA2(OBJ,CLASS1,CLASS2) \
        NV_CREATE_DMA2(NULL,OBJ,CLASS1,CLASS2);
#define NV_CREATE_OBJECT_DMA3(OBJ,CLASS1,CLASS2,CLASS3) \
        NV_CREATE_DMA3(NULL,OBJ,CLASS1,CLASS2,CLASS3);
#define NV_CREATE_OBJECT_DMA4(OBJ,CLASS1,CLASS2,CLASS3,CLASS4) \
        NV_CREATE_DMA4(NULL,OBJ,CLASS1,CLASS2,CLASS3,CLASS4);
#define NV_CREATE_OBJECT_DMA5(OBJ,CLASS1,CLASS2,CLASS3,CLASS4,CLASS5) \
        NV_CREATE_DMA5(NULL,OBJ,CLASS1,CLASS2,CLASS3,CLASS4,CLASS5);
