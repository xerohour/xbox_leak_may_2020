 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/******************************** HAL Data *********************************\
*                                                                           *
* Module: HALNV10.C                                                         *
*   The chip dependent HAL data is kept here.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <nv10_ref.h>
#include <nv10_hal.h>
#include "nvhw.h"

//-----------------------------------------------------------------------
// NV10 HAL engine descriptor table.
//-----------------------------------------------------------------------

//
// Engine default per-object instance memory sizes (16byte units).
//
#define GR_OBJINSTSZ                    0x1

ENGINEDESCRIPTOR NV10_EngineDescriptors[] =
{
    MKENGDECL(MC_ENGINE_TAG, 0, 0),
    MKENGMALLOC(MC_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MCHALINFO_NV10)/4),

    MKENGDECL(FB_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FB_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (FBHALINFO_NV10)/4),

    MKENGDECL(DAC_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    MKENGDECL(TMR_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    // dma object size is determined by separate HAL call
    MKENGDECL(DMA_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    MKENGDECL(GR_ENGINE_TAG, NV_RAMHT_ENGINE_GRAPHICS, GR_OBJINSTSZ),
    MKENGMALLOC(GR_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (GRHALINFO_NV10)/4),

    MKENGDECL(FIFO_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FIFO_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (FIFOHALINFO_NV10)/4),

    MKENGDECL(MP_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),
    MKENGMALLOC(MP_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MPHALINFO_NV10)/4),

    MKENGDECL(VIDEO_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0)
};

U032 NV10_NumEngineDescs = (sizeof (NV10_EngineDescriptors) / sizeof (ENGINEDESCRIPTOR));

//-----------------------------------------------------------------------
// NV10 HAL class descriptor table.
//-----------------------------------------------------------------------

//
// The base classes supported for all NV10 Architectures
// 
#define NV10_ARCHITECTURE_CLASS_DESCRIPTORS                                                         \
    /* fifo engine classes */                                                                       \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_PIO),                                              \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_DMA),                                              \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_PIO),                                              \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_DMA),                                              \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV10_CHANNEL_DMA),                                              \
                                                                                                    \
    /* graphics engine classes */                                                                   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA_SOLID, NV01_BETA_SOLID),                        \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV01_IMAGE_SOLID),                        \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV01_IMAGE_PATTERN),                        \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_CLIP, NV1_IMAGE_BLACK_RECTANGLE),                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_RENDER_SOLID_LIN),                                      \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_RENDER_SOLID_TRIANGLE),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_RENDER_SOLID_RECTANGLE),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_IMAGE_BLIT),                                            \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_IMAGE_FROM_CPU),                                        \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_NULL),                                                  \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV3_STRETCHED_IMAGE_FROM_CPU),                              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV3_SCALED_IMAGE_FROM_MEMORY),                              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV3_MEMORY_TO_MEMORY_FORMAT),                               \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_ROP, NV3_CONTEXT_ROP),                               \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV3_DX3_TEXTURED_TRIANGLE),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV3_GDI_RECTANGLE_TEXT),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_DESTINATION),   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_SOURCE),        \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_DEPTH),         \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_COLOR),         \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_DVD, NV4_DVD_SUBPICTURE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV4_CONTEXT_SURFACES_2D),               \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV4_CONTEXT_PATTERN),                       \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_GDI_RECTANGLE_TEXT),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_SWIZZLED, NV4_CONTEXT_SURFACE_SWIZZLED),    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV4_CONTEXT_SURFACES_ARGB_ZS),          \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV4_CONTEXT_COLOR_KEY),                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_RENDER_SOLID_LIN),                                      \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_RENDER_SOLID_TRIANGLE),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_RENDER_SOLID_RECTANGLE),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_IMAGE_BLIT),                                            \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_IMAGE_FROM_CPU),                                        \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_SCALED_IMAGE_FROM_MEMORY),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_INDEXED_IMAGE_FROM_CPU),                               \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_IMAGE_FROM_CPU),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_STRETCHED_IMAGE_FROM_CPU),                             \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA, NV4_CONTEXT_BETA),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_STRETCHED_IMAGE_FROM_CPU),                              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV4_SCALED_IMAGE_FROM_MEMORY),                              \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV10_CONTEXT_SURFACES_2D),              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_TEXTURE_FROM_CPU),                                     \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_DVD, NV10_DVD_SUBPICTURE),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_SCALED_IMAGE_FROM_MEMORY),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_IMAGE_FROM_CPU),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV10_CONTEXT_SURFACES_3D),              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_DX5_TEXTURED_TRIANGLE),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_DX6_MULTI_TEXTURE_TRIANGLE),                           \
                                                                                                    \
    /* dac engine classes */                                                                        \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV04_VIDEO_LUT_CURSOR_DAC),                                      \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV05_VIDEO_LUT_CURSOR_DAC),                                      \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV10_VIDEO_LUT_CURSOR_DAC),                                      \
                                                                                                    \
    /* mediaport engine classes */                                                                  \
    MKCLASSDECL(MP_ENGINE_TAG, MP_LOCK_DECODER, NV03_EXTERNAL_VIDEO_DECODER),                       \
    MKCLASSDECL(MP_ENGINE_TAG, MP_LOCK_DECOMPRESSOR, NV03_EXTERNAL_VIDEO_DECOMPRESSOR),             \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV01_EXTERNAL_PARALLEL_BUS),                                \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV03_EXTERNAL_MONITOR_BUS),                                 \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV04_EXTERNAL_SERIAL_BUS),                                  \
                                                                                                    \
    /* video engine classes */                                                                      \
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV04_VIDEO_OVERLAY),                                     \
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV10_VIDEO_OVERLAY),

//
// The classes supported on NV10
//
CLASSDESCRIPTOR NV10_ClassDescriptors[] =
{
    NV10_ARCHITECTURE_CLASS_DESCRIPTORS
};

U032 NV10_NumClassDescs = (sizeof (NV10_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//
// The classes supported on NV15
// 
#define NV15_IMPLEMENTATION_CLASS_DESCRIPTORS                                                       \
    /* graphics engine classes */                                                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV15_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV15_IMAGE_BLIT),                                           \
                                                                                                    \
    /* dac engine classes */                                                                        \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV15_VIDEO_LUT_CURSOR_DAC),                                      \

CLASSDESCRIPTOR NV15_ClassDescriptors[] =
{
    NV10_ARCHITECTURE_CLASS_DESCRIPTORS
    NV15_IMPLEMENTATION_CLASS_DESCRIPTORS
};

U032 NV15_NumClassDescs = (sizeof (NV15_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//
// The classes supported on NV11
// 
#define NV11_IMPLEMENTATION_CLASS_DESCRIPTORS                                                       \
    /* graphics engine classes */                                                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV11_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV11_SCALED_IMAGE_FROM_MEMORY),

CLASSDESCRIPTOR NV11_ClassDescriptors[] =
{
    NV10_ARCHITECTURE_CLASS_DESCRIPTORS
    NV15_IMPLEMENTATION_CLASS_DESCRIPTORS
    NV11_IMPLEMENTATION_CLASS_DESCRIPTORS
};

U032 NV11_NumClassDescs = (sizeof (NV11_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//-----------------------------------------------------------------------
// NV10 HAL function table layout.
//-----------------------------------------------------------------------

// mc interfaces
extern RM_STATUS nvHalMcControl_NV10(VOID *);
extern RM_STATUS nvHalMcPower_NV10(VOID *);

// fifo interfaces
extern RM_STATUS nvHalFifoControl_NV10(VOID *);
extern RM_STATUS nvHalFifoAllocPio_NV10(VOID *);
extern RM_STATUS nvHalFifoAllocDma_NV10(VOID *);
extern RM_STATUS nvHalFifoFree_NV10(VOID *);
extern RM_STATUS nvHalFifoGetExceptionData_NV10(VOID *);
extern RM_STATUS nvHalFifoService_NV10(VOID *);
extern RM_STATUS nvHalFifoAccess_NV10(VOID *);
extern RM_STATUS nvHalFifoHashAdd_NV10(VOID *);
extern RM_STATUS nvHalFifoHashDelete_NV10(VOID *);
extern RM_STATUS nvHalFifoHashFunc_NV10(VOID *);
extern RM_STATUS nvHalFifoHashSearch_NV10(VOID *);

// framebuffer interfaces
extern RM_STATUS nvHalFbControl_NV10(VOID *);
extern RM_STATUS nvHalFbAlloc_NV10(VOID *);
extern RM_STATUS nvHalFbFree_NV10(VOID *);
extern RM_STATUS nvHalFbSetAllocParameters_NV10(VOID *);
extern RM_STATUS nvHalFbGetSurfacePitch_NV10(VOID *);
extern RM_STATUS nvHalFbGetSurfaceDimensions_NV10(VOID *);
extern RM_STATUS nvHalFbLoadOverride_NV10(VOID *);

// graphics interfaces
extern RM_STATUS nvHalGrControl_NV10(VOID *);
extern RM_STATUS nvHalGrAlloc_NV10(VOID *);
extern RM_STATUS nvHalGrFree_NV10(VOID *);
extern RM_STATUS nvHalGrGetExceptionData_NV10(VOID *);
extern RM_STATUS nvHalGrService_NV10(VOID *);
extern RM_STATUS nvHalGrGetNotifyData_NV10(VOID *);
extern RM_STATUS nvHalGrSetObjectContext_NV10(VOID *);
extern RM_STATUS nvHalGrLoadOverride_NV10(VOID *);

// dma interfaces
extern RM_STATUS nvHalDmaControl_NV10(VOID *);
extern RM_STATUS nvHalDmaAlloc_NV10(VOID *);
extern RM_STATUS nvHalDmaFree_NV10(VOID *);
extern RM_STATUS nvHalDmaGetInstSize_NV10(VOID *);

// dac interfaces
extern RM_STATUS nvHalDacControl_NV10(VOID *);
extern RM_STATUS nvHalDacAlloc_NV10(VOID *);
extern RM_STATUS nvHalDacFree_NV10(VOID *);
extern RM_STATUS nvHalDacSetStartAddr_NV10(VOID *);
extern RM_STATUS nvHalDacProgramMClk_NV10(VOID *);
extern RM_STATUS nvHalDacProgramNVClk_NV10(VOID *);
extern RM_STATUS nvHalDacProgramPClk_NV10(VOID *);
extern RM_STATUS nvHalDacProgramCursorImage_NV10(VOID *);
extern RM_STATUS nvHalDacGetRasterPosition_NV10(VOID *);
extern RM_STATUS nvHalDacValidateArbSettings_NV10(VOID *arg);
extern RM_STATUS nvHalDacUpdateArbSettings_NV10(VOID *arg);


// mediaport interfaces
extern RM_STATUS nvHalMpControl_NV10(VOID *);
extern RM_STATUS nvHalMpAlloc_NV10(VOID *);
extern RM_STATUS nvHalMpFree_NV10(VOID *);
extern RM_STATUS nvHalMpMethod_NV10(VOID *);
extern RM_STATUS nvHalMpGetEventStatus_NV10(VOID *);
extern RM_STATUS nvHalMpServiceEvent_NV10(VOID *);

// mediaport interfaces
extern RM_STATUS nvHalVideoControl_NV10(VOID *);
extern RM_STATUS nvHalVideoAlloc_NV10(VOID *);
extern RM_STATUS nvHalVideoFree_NV10(VOID *);
extern RM_STATUS nvHalVideoMethod_NV10(VOID *);
extern RM_STATUS nvHalVideoGetEventStatus_NV10(VOID *);
extern RM_STATUS nvHalVideoServiceEvent_NV10(VOID *);

HALFUNCS_000 NV10_HalFuncs_000 =
{
    // mc interfaces
    nvHalMcControl_NV10,
    nvHalMcPower_NV10,

    // fifo interfaces
    nvHalFifoControl_NV10,
    nvHalFifoAllocPio_NV10,
    nvHalFifoAllocDma_NV10,
    nvHalFifoFree_NV10,
    nvHalFifoGetExceptionData_NV10,
    nvHalFifoService_NV10,
    nvHalFifoAccess_NV10,
    nvHalFifoHashAdd_NV10,
    nvHalFifoHashDelete_NV10,
    nvHalFifoHashFunc_NV10,
    nvHalFifoHashSearch_NV10,

    // framebuffer interfaces
    nvHalFbControl_NV10,
    nvHalFbAlloc_NV10,
    nvHalFbFree_NV10,
    nvHalFbSetAllocParameters_NV10,
    nvHalFbGetSurfacePitch_NV10,
    nvHalFbGetSurfaceDimensions_NV10,
    nvHalFbLoadOverride_NV10,

    // graphics interfaces
    nvHalGrControl_NV10,
    nvHalGrAlloc_NV10,
    nvHalGrFree_NV10,
    nvHalGrGetExceptionData_NV10,
    nvHalGrService_NV10,
    nvHalGrGetNotifyData_NV10,
    nvHalGrSetObjectContext_NV10,
    nvHalGrLoadOverride_NV10,

    // dma interface
    nvHalDmaControl_NV10,
    nvHalDmaAlloc_NV10,
    nvHalDmaFree_NV10,
    nvHalDmaGetInstSize_NV10,

    // dac interfaces
    nvHalDacControl_NV10,
    nvHalDacAlloc_NV10,
    nvHalDacFree_NV10,
    nvHalDacSetStartAddr_NV10,
    nvHalDacProgramMClk_NV10,
    nvHalDacProgramNVClk_NV10,
    nvHalDacProgramPClk_NV10,
    nvHalDacProgramCursorImage_NV10,
    nvHalDacGetRasterPosition_NV10,
    nvHalDacValidateArbSettings_NV10,
    nvHalDacUpdateArbSettings_NV10,

    // mp interfaces
    nvHalMpControl_NV10,
    nvHalMpAlloc_NV10,
    nvHalMpFree_NV10,
    nvHalMpMethod_NV10,
    nvHalMpGetEventStatus_NV10,
    nvHalMpServiceEvent_NV10,

    // mp interfaces
    nvHalVideoControl_NV10,
    nvHalVideoAlloc_NV10,
    nvHalVideoFree_NV10,
    nvHalVideoMethod_NV10,
    nvHalVideoGetEventStatus_NV10,
    nvHalVideoServiceEvent_NV10
};
