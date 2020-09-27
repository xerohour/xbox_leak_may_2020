 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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
/*==========================================================================;
 *
 *  Copyright (C) 2000 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       nvmocomp.h
 *  Content:    Windows98 DDraw 32 bit driver motion compensation definitions
 *
 ***************************************************************************/

#ifndef _NVMOCOMP_H_
#define _NVMOCOMP_H_

#ifdef __cplusplus
extern "C" {
#endif

// Execute functions
#define EXECUTE_MOTION_COMPENSATION                 0
#define EXECUTE_TEMPORAL_FILTER                     1
#define EXECUTE_SUBPICTURE_DECOMPRESSION            2
#define EXECUTE_SUBPICTURE_COMPOSITING              3
#define EXECUTE_DISPLAY_OVERLAY_SURFACE             4
#define EXECUTE_FORMAT_CONVERT_OVERLAY_SURFACE      5
#define EXECUTE_NON_PREBIASED_MOTION_COMPENSATION   6
#define EXECUTE_DXVA_ALPHA_BLENDING                 7

// Temporal filter options
#define TEMPORAL_FILTER_ENABLE_ON_ALL_SURFACES      0
#define TEMPORAL_FILTER_DISABLE_ON_ALL_SURFACES     1
#define TEMPORAL_FILTER_DISABLE_ON_SINGLE_SURFACE   128

// PictureStructure
#define PICTURE_STRUCTURE_TOP_FIELD     1
#define PICTURE_STRUCTURE_BOTTOM_FIELD  2
#define PICTURE_STRUCTURE_FRAME         3

// PictureCodingType
#define PICTURE_TYPE_I                  1
#define PICTURE_TYPE_P                  2
#define PICTURE_TYPE_B                  3

// CodedBlockPattern
#define CBP_Y0                          32
#define CBP_Y1                          16
#define CBP_Y2                          8
#define CBP_Y3                          4
#define CBP_CB                          2
#define CBP_CR                          1

// DCTType
#define DCT_FRAME                       0
#define DCT_FIELD                       1

// MacroblockType
#define MB_INTRA                        0
#define MB_MOTION_FORWARD               1
#define MB_MOTION_BACKWARD              2

// MotionType
#define FRAME_STRUCTURE_MOTION_FIELD    1       // Field prediction
#define FRAME_STRUCTURE_MOTION_FRAME    2       // Frame prediction
#define FRAME_STRUCTURE_MOTION_DP       3       // Dual prime prediction
#define FIELD_STRUCTURE_MOTION_FIELD    1       // Field prediction
#define FIELD_STRUCTURE_MOTION_16X8     2       // 16x8 prediction
#define FIELD_STRUCTURE_MOTION_DP       3       // Dual prime prediction

// CorrectionType
#define CORRECTION_FIRST_PASS           0
#define CORRECTION_OVERFLOW_PASS        1

// VertexMode
#define VERTEX_MODE_FORWARD             1
#define VERTEX_MODE_BACKWARD            2
#define VERTEX_MODE_BIDIRECTIONAL       3

// DataFormat
#define DATA_FORMAT_LUMA                        1
#define DATA_FORMAT_CHROMA                      2
#define DATA_FORMAT_CORRECTION                  3
#define DATA_FORMAT_CHROMA_CORRECTION           4
#define DATA_FORMAT_UNBIASED_CORRECTION         5
#define DATA_FORMAT_UNBIASED_CHROMA_CORRECTION  6
#define DATA_FORMAT_FOURCC_CONVERSION           7
#define DATA_FORMAT_TEMPORAL_FILTER             8
#define DATA_FORMAT_SUBPICTURE_LUMA_BLEND       9
#define DATA_FORMAT_SUBPICTURE_CHROMA_BLEND     10

#define CELSIUS_DESTINATION_VIEWPORT_XOFFSET    (2046 << 2)
// Calculated for a scale factor of 75% ((2046/0.75) << 2)
#define SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET    (10912)

// DXVA Convert Buffer
#define LOOKUP_TABLE_OFFSET         0
#define LOOKUP_TABLE_SIZE           1024L
#define MACROBLOCKS_OFFSET          1024L
#define MACROBLOCKS_SIZE            1048576L
#define CONVERT_BUFFER_SIZE         LOOKUP_TABLE_SIZE + MACROBLOCKS_SIZE

// Other DXVA Constants
#define DXVA_SUBPICTURE_WIDTH       720L
#define DXVA_SUBPICTURE_HEIGHT      576L


// PMV[r][s][t] definitions
//      index   0                                   1
//
//      r       first motion vector in macroblock   second motion vector in macroblock (also indexes 2 and 3 in dual prime)
//      s       forward motion vector               backward motion vector
//      t       horizontal component                vertical component
//


typedef struct tagNVBEGINFRAMEDATA {
  BYTE  bMCPictureStructure;
  BYTE  bMCPictureCodingType;
  BYTE  bMCTopFieldFirst;
  BYTE  bMCSecondField;
  BYTE  bMCFrameType;
} NVBEGINFRAMEDATA, *LPNVBEGINFRAMEDATA;


typedef unsigned long (*PFUNC)();

typedef struct tagNVMCFRAMEDATA {
  PFUNC dwMCNvExecute;
  DWORD dwMCMultiMonID;
  BYTE  bMCDestinationSurfaceIndex;
  BYTE  bMCForwardSurfaceIndex;
  BYTE  bMCBackwardSurfaceIndex;
  BYTE  bMCPictureStructure;
  BYTE  bMCPictureCodingType;
  BYTE  bMCTopFieldFirst;
  BYTE  bMCSecondField;
  BYTE  bMCFrameType;
  BYTE  bMCEndOfPicture;

  // The following are reserved for NVDS proprietary interface.
  BYTE  bReserved1;
  BYTE  bReserved2;
  BYTE  bReserved3;
  DWORD dwReserved4;

  // The following are used by our DXVA functions only.
  WORD  wDXVADecodedPictureIndex;
  WORD  wDXVAForwardRefPictureIndex;
  WORD  wDXVABackwardRefPictureIndex;
  WORD  wDXVAPicWidthInMB;
  WORD  wDXVAPicHeightInMB;
  DWORD dwDXVASubpicBuffer;
  DWORD dwDXVASubpicStride;
  RECT  rcGraphicSourceRect;
  RECT  rcGraphicDestinationRect;
  BYTE  bBlendOn;

} NVMCFRAMEDATA, *LPNVMCFRAMEDATA;


typedef struct tagNVDECODEMACROBLOCK {
  short hOffset;            // upper left macroblock coordinates in pels
  short vOffset;            // upper left macroblock coordinates in scanlines
  unsigned short firstIDCTIndex;
  BYTE DCTType;
  BYTE codedBlockPattern;
  BYTE overflowCodedBlockPattern;
  BYTE motionType;
  short PMV[2][2][2];       // contains motion vectors in half pel units
  BYTE fieldSelect[2][2];
  BYTE macroblockType;
  BYTE pad0;                // currently unused
} NVDECODEMACROBLOCK, *LPNVDECODEMACROBLOCK;


typedef struct tagNVMCDISPLAYOVERLAYDATA {
    DWORD dwMCOverlayFlags; // DDOVER_INTERLEAVED, DDOVER_BOB, etc.
    DWORD dwMCFlipFlags;    // DDFLIP_ODD, DDFLIP_EVEN, DDFLIP_WAIT, etc.
} NVMCDISPLAYOVERLAYDATA, *LPNVMCDISPLAYOVERLAYDATA;


typedef struct tagNVMCFILTERDATA {
    BYTE bMCCurrentFieldCombinationFactor;      // 0 - 100 (%)
    BYTE bMCFilterOptions;                      // 0 == default == enable on all surfaces, 1 == disable on all surfaces
} NVMCFILTERDATA, *LPNVMCFILTERDATA;            // 128 == disable on dwSurfaceIndex only


typedef struct tagNVSPDECOMPRESSDATA {
    short wSPHOffset;
    short wSPVOffset;
    short wSPWidth;
    short wSPHeight;
    BYTE bSPUpdateLookUpTable; // TRUE == look up table has changed, FALSE == no look up table change
    BYTE bSPNumberOfActiveTableEntries; // 0 == 256, else 1 - 255
    BYTE bSPFormat;             // 0 == YUV, 1 == RGB
    BYTE bSPpad1;               // currently unused
    DWORD dwSPLookUpTable[256]; // byte format in each YUV DWORD is byte0 = V, byte1 = U, byte2 = Y, byte3 = X
                                // byte format in each RGB DWORD is byte0 = B, byte1 = G, byte2 = R, byte3 = X
} NVSPDECOMPRESSDATA, *LPNVSPDECOMPRESSDATA;


typedef struct tagNVSPCOMPOSITEDATA {
    short wSPHOffset;
    short wSPVOffset;
    short wSPWidth;
    short wSPHeight;
    short wMCHOffset;
    short wMCVOffset;
    short wMCWidth;
    short wMCHeight;
    BYTE bSPCompositeWith; // Frame/Top Field/Bottom Field
    BYTE bSPRestoreImageBeforeCompositing;
} NVSPCOMPOSITEDATA, *LPNVSPCOMPOSITEDATA;


typedef struct tagNVMOCOMPPARAMS {
  unsigned long surfaceLumaPitch;
  unsigned long surfaceLumaHeight;
  unsigned long surfaceOffset;
  unsigned long surfaceChromaOffset;
  unsigned long surfaceBiasedIntraBlockOffset;
  long blockX;
  long blockY;
  long blockY2;
  long correctionY;
  short blockWidth;
  short blockHeight;
  short forwardHorizontalVector;
  short forwardVerticalVector;
  short backwardHorizontalVector;
  short backwardVerticalVector;
  BYTE forwardField;
  BYTE backwardField;
  BYTE correctionField;
  BYTE destinationField;
  BYTE currentForwardField;
  BYTE currentBackwardField;
  BYTE currentDestinationField;
  BYTE destinationFieldIs0;
  short forwardHorizontalVector2;
  short forwardVerticalVector2;
  short backwardHorizontalVector2;
  short backwardVerticalVector2;
  BYTE forwardField2;
  BYTE backwardField2;
  BYTE correctionField2;
  BYTE destinationField2;
  BYTE vertexMode;
  BYTE dataFormat;
  short celsiusDestinationViewportXOffset;
  short celsiusFieldXOffset;
} NVMOCOMPPARAMS, *LPNVMOCOMPPARAMS;


#if (NVARCH >= 0x10)

typedef void (__stdcall *PNVMCPREDFUNC)(BOOL);
typedef DWORD (__stdcall *PNVMCPREDFUNC2)(LPNVDECODEMACROBLOCK, WORD);
typedef void (__stdcall *PNVMCCORRFUNC)(void);
typedef void (__stdcall *PNVMCCORRFUNC2)(long, long);

#define nvPushMoCompQuadData(OFFSET) do { \
    nvPushData((OFFSET), texture1BlockY0 | texture1BlockX0); \
    nvPushData((OFFSET+1), texture0BlockY0 | texture0BlockX0); \
    nvPushData((OFFSET+2), blockY0 | blockX0); \
    nvPushData((OFFSET+3), texture1BlockY1 | texture1BlockX0); \
    nvPushData((OFFSET+4), texture0BlockY1 | texture0BlockX0); \
    nvPushData((OFFSET+5), blockY1 | blockX0); \
    nvPushData((OFFSET+6), texture1BlockY1 | texture1BlockX1); \
    nvPushData((OFFSET+7), texture0BlockY1 | texture0BlockX1); \
    nvPushData((OFFSET+8), blockY1 | blockX1); \
    nvPushData((OFFSET+9), texture1BlockY0 | texture1BlockX1); \
    nvPushData((OFFSET+10), texture0BlockY0 | texture0BlockX1); \
    nvPushData((OFFSET+11), blockY0 | blockX1); \
} while(0)

#define nvPushKelvinMoCompQuadData(OFFSET) do { \
    nvPushData((OFFSET), blockY0 | blockX0); \
    nvPushData((OFFSET+1), texture0BlockY0 | texture0BlockX0); \
    nvPushData((OFFSET+2), texture1BlockY0 | texture1BlockX0); \
    nvPushData((OFFSET+3), blockY1 | blockX0); \
    nvPushData((OFFSET+4), texture0BlockY1 | texture0BlockX0); \
    nvPushData((OFFSET+5), texture1BlockY1 | texture1BlockX0); \
    nvPushData((OFFSET+6), blockY1 | blockX1); \
    nvPushData((OFFSET+7), texture0BlockY1 | texture0BlockX1); \
    nvPushData((OFFSET+8), texture1BlockY1 | texture1BlockX1); \
    nvPushData((OFFSET+9), blockY0 | blockX1); \
    nvPushData((OFFSET+10), texture0BlockY0 | texture0BlockX1); \
    nvPushData((OFFSET+11), texture1BlockY0 | texture1BlockX1); \
} while(0)



#define DEFAULT_ALPHA_ICW   (NV056_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |  \
                            (NV056_SET_COMBINER_ALPHA_ICW_A_ALPHA_TRUE << 28) |             \
                            (NV056_SET_COMBINER_ALPHA_ICW_A_SOURCE_REG_4 << 24) |           \
                                                                                            \
                            (NV056_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_INVERT << 21) |    \
                            (NV056_SET_COMBINER_ALPHA_ICW_B_ALPHA_TRUE << 20) |             \
                            (NV056_SET_COMBINER_ALPHA_ICW_B_SOURCE_REG_0 << 16) |           \
                                                                                            \
                            (NV056_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |  \
                            (NV056_SET_COMBINER_ALPHA_ICW_C_ALPHA_TRUE << 12) |             \
                            (NV056_SET_COMBINER_ALPHA_ICW_C_SOURCE_REG_0 << 8) |            \
                                                                                            \
                            (NV056_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |   \
                            (NV056_SET_COMBINER_ALPHA_ICW_D_ALPHA_TRUE << 4) |              \
                             NV056_SET_COMBINER_ALPHA_ICW_D_SOURCE_REG_0

#define DEFAULT_COLOR_ICW   (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |  \
                            (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |            \
                            (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_4 << 24) |           \
                                                                                            \
                            (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_INVERT << 21) |    \
                            (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |            \
                            (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_0 << 16) |           \
                                                                                            \
                            (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |  \
                            (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |            \
                            (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |            \
                                                                                            \
                            (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |   \
                            (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |             \
                             NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0

#define DEFAULT_ALPHA_OCW   (NV056_SET_COMBINER_ALPHA_OCW_OPERATION_NOSHIFT << 15) |        \
                            (NV056_SET_COMBINER_ALPHA_OCW_MUX_ENABLE_FALSE << 14) |         \
                            (NV056_SET_COMBINER_ALPHA_OCW_SUM_DST_REG_C << 8) |             \
                            (NV056_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0 << 4) |              \
                             NV056_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0

#define DEFAULT_C1_COLOR_OCW (NV056_SET_COMBINER1_COLOR_OCW_ITERATION_COUNT_ONE << 28) |    \
                             (NV056_SET_COMBINER1_COLOR_OCW_MUX_SELECT_MSB << 27) |         \
                             (NV056_SET_COMBINER1_COLOR_OCW_OPERATION_NOSHIFT << 15) |      \
                             (NV056_SET_COMBINER1_COLOR_OCW_MUX_ENABLE_FALSE << 14) |       \
                             (NV056_SET_COMBINER1_COLOR_OCW_AB_DOT_ENABLE_FALSE << 13) |    \
                             (NV056_SET_COMBINER1_COLOR_OCW_CD_DOT_ENABLE_FALSE << 12) |    \
                             (NV056_SET_COMBINER1_COLOR_OCW_SUM_DST_REG_C << 8) |           \
                             (NV056_SET_COMBINER1_COLOR_OCW_AB_DST_REG_0 << 4) |            \
                              NV056_SET_COMBINER1_COLOR_OCW_CD_DST_REG_0

#define SET_LUMA_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_LUMA) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_LUMA; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_KELVIN_LUMA_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_LUMA) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_LUMA; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)

#define SET_CHROMA_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CHROMA) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CHROMA; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_KELVIN_CHROMA_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CHROMA) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CHROMA; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)

#define SET_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_SY8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_CHROMA_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CHROMA_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CHROMA_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_SG8SB8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_KELVIN_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)

#define SET_KELVIN_CHROMA_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_CHROMA_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_CHROMA_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)

#define SET_UNBIASED_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_UNBIASED_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_UNBIASED_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_UNBIASED_CHROMA_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_UNBIASED_CHROMA_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_UNBIASED_CHROMA_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV056_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      NV056_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_TEXTURE_FORMAT(0) | 0x80000); \
        nvPushData(3, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, (NV056_SET_TEXTURE_FORMAT_WRAPV_FALSE << 31) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSV_CLAMP << 28) | \
                      (NV056_SET_TEXTURE_FORMAT_WRAPU_FALSE << 27) | \
                      (NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP << 24) | \
                      (1 << 12) | \
                      (NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8 << 7) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_FOH_CORNER << 5) | \
                      (NV056_SET_TEXTURE_FORMAT_ORIGIN_ZOH_CORNER << 3) | \
                      (NV056_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(5); \
    } \
} while(0)

#define SET_KELVIN_UNBIASED_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_UNBIASED_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_UNBIASED_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)

#define SET_KELVIN_UNBIASED_CHROMA_CORRECTION_DATA_FORMAT() do { \
    if (nvMoCompParams.dataFormat != DATA_FORMAT_UNBIASED_CHROMA_CORRECTION) { \
        nvMoCompParams.dataFormat = DATA_FORMAT_UNBIASED_CHROMA_CORRECTION; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_SURFACE_FORMAT | 0x40000); \
        nvPushData(1, (NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1 << 12) | \
                      (NV097_SET_SURFACE_FORMAT_TYPE_PITCH << 8) | \
                      (NV097_SET_SURFACE_FORMAT_ZETA_Z16 << 4) | \
                       NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(0) | 0x40000); \
        nvPushData(3, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B); \
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_TEXTURE_FORMAT(1) | 0x40000); \
        nvPushData(5, (1 << 16) | \
                      (NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 << 8) | \
                      (NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_TWO << 4) | \
                      (NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR << 3) | \
                      (NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE_FALSE << 2) | \
                       NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A); \
        nvPusherAdjust(6); \
    } \
} while(0)


// NOTE: vertexSizeContext is an NV10 hardware context switching bug workaround which saves the
// current vertex size context in an unused but context switched location in the 2nd inverse
// model view matrix which the RM uses to restore this lost context data when restoring the context of this channel

#define SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_BIDIRECTIONAL) { \
        unsigned long vertexSizeContext; \
        nvMoCompParams.vertexMode = VERTEX_MODE_BIDIRECTIONAL; \
        vertexSizeContext = NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 | \
                            (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                            (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8) | \
                            (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 12) | \
                            (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 16) | \
                            (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20) | \
                            (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24) | \
                            (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28); \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15) | 0x40000); \
        nvPushData(1, vertexSizeContext); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_VERTEX_ARRAY_OFFSET | 0x400000); \
        nvPushData(3, 0); \
        nvPushData(4, (NV056_SET_VERTEX_ARRAY_FORMAT_W_NONE << 24) | \
                      (12 << 8) | \
                      (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 << 4) | \
                       NV056_SET_VERTEX_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(5, 0); \
        nvPushData(6, (12 << 8) | \
                      (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_DIFFUSE_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(7, 0); \
        nvPushData(8, (12 << 8) | \
                      (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_SPECULAR_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(9, 4); \
        nvPushData(10, (12 << 8) | \
                      (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 4) | \
                       NV056_SET_TEX_COORD0_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(11, 8); \
        nvPushData(12, (12 << 8) | \
                       (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV056_SET_TEX_COORD1_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(13, 0); \
        nvPushData(14, (12 << 8) | \
                       (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_NORMAL_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(15, 0); \
        nvPushData(16, (12 << 8) | \
                       (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_WEIGHT_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(17, 0); \
        nvPushData(18, (12 << 8) | \
                       (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_FOG_ARRAY_FORMAT_TYPE_SHORT); \
        nvPusherAdjust(19); \
    } \
} while(0)

// Kelvin vertex data array slots (in order)
//
// Position
// Weight
// Normal
// Diffuse
// Specular (RGBA or BGRF in 4th component, if present)
// Fog
// Point Size
// Back Diffuse
// Back Specular
// Texture 0
// Texture 1
// Texture 2
// Texture 3
// Reserved
// Reserved
// Reserved

#define SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_BIDIRECTIONAL) { \
        nvMoCompParams.vertexMode = VERTEX_MODE_BIDIRECTIONAL; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) | 0x340000); \
        nvPushData(1, 0); \
        nvPushData(2, 0); \
        nvPushData(3, 0); \
        nvPushData(4, 0); \
        nvPushData(5, 0); \
        nvPushData(6, 0); \
        nvPushData(7, 0); \
        nvPushData(8, 0); \
        nvPushData(9, 0); \
        nvPushData(10, 4); \
        nvPushData(11, 8); \
        nvPushData(12, 0); \
        nvPushData(13, 0); \
        nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                       NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0) | 0x340000); \
        nvPushData(15, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(16, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(17, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(18, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(19, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(20, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(21, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(22, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(23, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(24, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(25, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(26, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(27, (12 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPusherAdjust(28); \
    } \
} while(0)

// NOTE: vertexSizeContext is an NV10 hardware context switching bug workaround which saves the
// current vertex size context in an unused but context switched location in the 2nd inverse
// model view matrix which the RM uses to restore this lost context data when restoring the context of this channel

#define SET_FORWARD_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_FORWARD) { \
        unsigned long vertexSizeContext; \
        nvMoCompParams.vertexMode = VERTEX_MODE_FORWARD; \
        vertexSizeContext = NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 | \
                            (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                            (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8) | \
                            (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 12) | \
                            (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_0 << 16) | \
                            (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20) | \
                            (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24) | \
                            (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28); \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15) | 0x40000); \
        nvPushData(1, vertexSizeContext); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_VERTEX_ARRAY_OFFSET | 0x400000); \
        nvPushData(3, 0); \
        nvPushData(4, (NV056_SET_VERTEX_ARRAY_FORMAT_W_NONE << 24) | \
                      (8 << 8) | \
                      (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 << 4) | \
                       NV056_SET_VERTEX_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(5, 0); \
        nvPushData(6, (8 << 8) | \
                      (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_DIFFUSE_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(7, 0); \
        nvPushData(8,(8 << 8) | \
                      (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_SPECULAR_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(9, 4); \
        nvPushData(10, (8 << 8) | \
                      (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_2 << 4) | \
                       NV056_SET_TEX_COORD0_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(11, 0); \
        nvPushData(12, (8 << 8) | \
                       (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_TEX_COORD1_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(13, 0); \
        nvPushData(14, (8 << 8) | \
                       (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_NORMAL_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(15, 0); \
        nvPushData(16, (8 << 8) | \
                       (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_WEIGHT_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(17, 0); \
        nvPushData(18, (8 << 8) | \
                       (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_FOG_ARRAY_FORMAT_TYPE_SHORT); \
        nvPusherAdjust(19); \
    } \
} while(0)

#define SET_KELVIN_FORWARD_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_FORWARD) { \
        nvMoCompParams.vertexMode = VERTEX_MODE_FORWARD; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) | 0x340000); \
        nvPushData(1, 0); \
        nvPushData(2, 0); \
        nvPushData(3, 0); \
        nvPushData(4, 0); \
        nvPushData(5, 0); \
        nvPushData(6, 0); \
        nvPushData(7, 0); \
        nvPushData(8, 0); \
        nvPushData(9, 0); \
        nvPushData(10, 4); \
        nvPushData(11, 0); \
        nvPushData(12, 0); \
        nvPushData(13, 0); \
        nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                       NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0) | 0x340000); \
        nvPushData(15, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(16, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(17, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(18, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(19, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(20, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(21, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(22, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(23, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(24, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(25, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(26, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(27, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPusherAdjust(28); \
    } \
} while(0)

// NOTE: vertexSizeContext is an NV10 hardware context switching bug workaround which saves the
// current vertex size context in an unused but context switched location in the 2nd inverse
// model view matrix which the RM uses to restore this lost context data when restoring the context of this channel

#define SET_BACKWARD_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_BACKWARD) { \
        unsigned long vertexSizeContext; \
        nvMoCompParams.vertexMode = VERTEX_MODE_BACKWARD; \
        vertexSizeContext = NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 | \
                            (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                            (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 8) | \
                            (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_0 << 12) | \
                            (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 16) | \
                            (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 20) | \
                            (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 24) | \
                            (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 28); \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_INVERSE_MODEL_VIEW_MATRIX1(15) | 0x40000); \
        nvPushData(1, vertexSizeContext); \
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) + \
                      NV056_SET_VERTEX_ARRAY_OFFSET | 0x400000); \
        nvPushData(3, 0); \
        nvPushData(4, (NV056_SET_VERTEX_ARRAY_FORMAT_W_NONE << 24) | \
                      (8 << 8) | \
                      (NV056_SET_VERTEX_ARRAY_FORMAT_SIZE_2 << 4) | \
                       NV056_SET_VERTEX_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(5, 0); \
        nvPushData(6, (8 << 8) | \
                      (NV056_SET_DIFFUSE_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_DIFFUSE_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(7, 0); \
        nvPushData(8, (8 << 8) | \
                      (NV056_SET_SPECULAR_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_SPECULAR_ARRAY_FORMAT_TYPE_UNSIGNED_BYTE_BGRA); \
        nvPushData(9, 0); \
        nvPushData(10, (8 << 8) | \
                      (NV056_SET_TEX_COORD0_ARRAY_FORMAT_SIZE_0 << 4) | \
                       NV056_SET_TEX_COORD0_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(11, 4); \
        nvPushData(12, (8 << 8) | \
                       (NV056_SET_TEX_COORD1_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV056_SET_TEX_COORD1_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(13, 0); \
        nvPushData(14, (8 << 8) | \
                       (NV056_SET_NORMAL_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_NORMAL_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(15, 0); \
        nvPushData(16, (8 << 8) | \
                       (NV056_SET_WEIGHT_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_WEIGHT_ARRAY_FORMAT_TYPE_SHORT); \
        nvPushData(17, 0); \
        nvPushData(18, (8 << 8) | \
                       (NV056_SET_FOG_ARRAY_FORMAT_SIZE_0 << 4) | \
                        NV056_SET_FOG_ARRAY_FORMAT_TYPE_SHORT); \
        nvPusherAdjust(19); \
    } \
} while(0)

#define SET_KELVIN_BACKWARD_VERTEX_ARRAY_MODE() do { \
    if (nvMoCompParams.vertexMode != VERTEX_MODE_BACKWARD) { \
        nvMoCompParams.vertexMode = VERTEX_MODE_BACKWARD; \
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                      NV097_SET_VERTEX_DATA_ARRAY_OFFSET(0) | 0x340000); \
        nvPushData(1, 0); \
        nvPushData(2, 0); \
        nvPushData(3, 0); \
        nvPushData(4, 0); \
        nvPushData(5, 0); \
        nvPushData(6, 0); \
        nvPushData(7, 0); \
        nvPushData(8, 0); \
        nvPushData(9, 0); \
        nvPushData(10, 0); \
        nvPushData(11, 4); \
        nvPushData(12, 0); \
        nvPushData(13, 0); \
        nvPushData(14, dDrawSubchannelOffset(NV_DD_KELVIN) + \
                       NV097_SET_VERTEX_DATA_ARRAY_FORMAT(0) | 0x340000); \
        nvPushData(15, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(16, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(17, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(18, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(19, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(20, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(21, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(22, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(23, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(24, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(25, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_2 << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(26, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPushData(27, (8 << 8) | \
                       (NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE_DISABLED << 4) | \
                        NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K); \
        nvPusherAdjust(28); \
    } \
} while(0)

//---------------------------------------------------------------------------

// global varaiables

#ifdef  DEBUG
extern unsigned long overlaySurfaceBase;
extern unsigned long moCompDebugLevel;
#endif  // DEBUG

//---------------------------------------------------------------------------

// Because of differences between the Win9X and Win2K headers...
#ifdef  WINNT
typedef DDMOCOMPBUFFERINFO  DDMCBUFFERINFO, *LPDDMCBUFFERINFO;
#endif

// prototypes

void  __stdcall nvGetMoCompDriverInfo (LPDDHAL_GETDRIVERINFODATA lpData);
DWORD __stdcall nvMoCompExecute (DWORD dwMCMultiMonID,
                                 DWORD dwFunction,
                                 LPVOID lpInputData,
                                 DWORD dwInputDataSize,
                                 DWORD dwSurfaceIndex);
DWORD __stdcall nvMoCompConvertSurfaceFormat (DWORD fpVidMem, BOOL filterChroma, DWORD dwHowMuchToConvert);
DWORD __stdcall nvMoCompTemporalFilter (DWORD fpVidMem, BYTE combinationFactor);
DWORD __stdcall nvRenderCheckDecodeConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData);
DWORD __stdcall nvRenderCheckAlphaLoadConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData);
DWORD __stdcall nvRenderCheckAlphaCombineConfig(LPDDHAL_RENDERMOCOMPDATA lpRMCData);
DWORD __stdcall nvRenderGetPictureParams(LPDDHAL_RENDERMOCOMPDATA lpRMCData,
                                         LPDDMCBUFFERINFO lpPictureParams);
DWORD __stdcall nvRenderDoMotionComp(LPDDHAL_RENDERMOCOMPDATA lpRMCData,
                                     DWORD dwMacroblockIndex,
                                     DWORD dwDifferenceIndex);
LPBYTE __stdcall nvGetLinearAddress(LPDDRAWI_DDRAWSURFACE_LCL lpSurf);
DWORD  __stdcall nvRenderGetYUVPalette(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex);
DWORD  __stdcall nvRenderGetIA44Surface(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex);
DWORD  __stdcall nvRenderAlphaBlendCombination(LPDDHAL_RENDERMOCOMPDATA lpRMCData, DWORD dwRenderIndex);
DWORD  __stdcall nvDXVACompositeSubpicture(LPVOID lpInputData, DWORD dwIndex);
void   __stdcall nvUpdateDXVAIndexTable(GLOBALDATA *pDriverData);
DWORD  __stdcall nvMoCompFrameStatus(FLATPTR fpVidMem);
DWORD  __stdcall nvDXVABackEndAlphaBlend(FLATPTR fpVidMem);

#endif  // NVARCH >= 0x10

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NVMOCOMP_H_
