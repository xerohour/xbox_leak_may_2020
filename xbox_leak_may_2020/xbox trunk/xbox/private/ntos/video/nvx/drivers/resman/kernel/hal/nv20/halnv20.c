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
* Module: HALNV20.C                                                         *
*   The chip dependent HAL data is kept here.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <nv20_ref.h>
#include <nv20_hal.h>
#include <nv20_gr.h>
#include "nvhw.h"

//-----------------------------------------------------------------------
// NV20 HAL engine descriptor table.
//-----------------------------------------------------------------------

//
// Engine default per-object instance memory sizes (16byte units).
//
#define GR_OBJINSTSZ                    0x1

ENGINEDESCRIPTOR NV20_EngineDescriptors[] =
{
    //
    // master controller
    //
    MKENGDECL(MC_ENGINE_TAG, 0, 0),
    MKENGMALLOC(MC_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MCHALINFO_NV20)/4),

    //
    // frame buffer
    //
    MKENGDECL(FB_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FB_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (FBHALINFO_NV20)/4),

    //
    // dac
    //
    MKENGDECL(DAC_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    //
    // timer
    //
    MKENGDECL(TMR_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    //
    // dma -- the size of a DMA object is setup via nvHalDmaAlloc
    //
    MKENGDECL(DMA_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    //
    // graphics
    //
    MKENGDECL(GR_ENGINE_TAG, NV_RAMHT_ENGINE_GRAPHICS, GR_OBJINSTSZ),
    MKENGMALLOC(GR_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (GRHALINFO_NV20)/4),
    // channel_ctx_table is 32 channels * 4bytes
    MKENGINSTMALLOC(GR_ENGINE_TAG, 1, (32 * 4)/16),

    //
    // fifo
    //
    MKENGDECL(FIFO_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FIFO_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (FIFOHALINFO_NV20)/4),

    //
    // mediaport
    //
    MKENGDECL(MP_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),
    MKENGMALLOC(MP_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MPHALINFO_NV20)/4),

    //
    // video
    //
    MKENGDECL(VIDEO_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0)
};

U032 NV20_NumEngineDescs = (sizeof (NV20_EngineDescriptors) / sizeof (ENGINEDESCRIPTOR));

//-----------------------------------------------------------------------
// NV20 HAL class descriptor table.
//-----------------------------------------------------------------------

//
// The base classes supported for all NV20 Architectures
//
#define NV20_ARCHITECTURE_CLASS_DESCRIPTORS                                                         \
    /* fifo engine classes */                                                                       \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_PIO),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_DMA),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_PIO),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_DMA),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV10_CHANNEL_DMA),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV20_CHANNEL_DMA),                                              \
    MKCLASSINSTMALLOC(1, (NV20_GR_CONTEXT_SIZE/16)),                                                \
                                                                                                    \
    /* graphics engine classes */                                                                   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA_SOLID, NV01_CONTEXT_BETA),                      \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV01_CONTEXT_COLOR_KEY),                  \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV01_CONTEXT_PATTERN),                      \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_CLIP, NV01_CONTEXT_CLIP_RECTANGLE),                  \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_RENDER_SOLID_LIN),                                     \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_RENDER_SOLID_TRIANGLE),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_IMAGE_BLIT),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_IMAGE_FROM_CPU),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV1_NULL),                                                  \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_STRETCHED_IMAGE_FROM_CPU),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_SCALED_IMAGE_FROM_MEMORY),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_MEMORY_TO_MEMORY_FORMAT),                              \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_ROP, NV03_CONTEXT_ROP),                              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_GDI_RECTANGLE_TEXT),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_DESTINATION),   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_SOURCE),        \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_DEPTH),         \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_COLOR),         \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_DVD, NV04_DVD_SUBPICTURE),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV04_CONTEXT_SURFACES_2D),              \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV04_CONTEXT_PATTERN),                      \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_GDI_RECTANGLE_TEXT),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_SWIZZLED, NV04_CONTEXT_SURFACE_SWIZZLED),   \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV04_CONTEXT_COLOR_KEY),                  \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_RENDER_SOLID_LIN),                                     \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_RENDER_SOLID_TRIANGLE),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_IMAGE_BLIT),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_IMAGE_FROM_CPU),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA, NV04_CONTEXT_BETA),                            \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_STRETCHED_IMAGE_FROM_CPU),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_SCALED_IMAGE_FROM_MEMORY),                             \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_SCALED_IMAGE_FROM_MEMORY),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_INDEXED_IMAGE_FROM_CPU),                               \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_IMAGE_FROM_CPU),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_STRETCHED_IMAGE_FROM_CPU),                             \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV10_CONTEXT_SURFACES_2D),              \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_TEXTURE_FROM_CPU),                                     \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_DVD, NV10_DVD_SUBPICTURE),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_SCALED_IMAGE_FROM_MEMORY),                             \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV10_IMAGE_FROM_CPU),                                       \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV11_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV11_SCALED_IMAGE_FROM_MEMORY),                             \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV15_CELSIUS_PRIMITIVE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_SWIZZLED, NV15_CONTEXT_SURFACE_SWIZZLED),   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV15_IMAGE_BLIT),                                           \
                                                                                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV20_IMAGE_BLIT),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV20_KELVIN_PRIMITIVE),                                     \
    MKCLASSINSTMALLOC(NV20_KELVIN_CONTEXT_ALIGN/16, NV20_KELVIN_CONTEXT_SIZE/16),                   \
                                                                                                    \
    /* dac engine classes */                                                                        \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV04_VIDEO_LUT_CURSOR_DAC),                                      \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV05_VIDEO_LUT_CURSOR_DAC),                                      \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV10_VIDEO_LUT_CURSOR_DAC),                                      \
    MKCLASSDECL(DAC_ENGINE_TAG, 0, NV15_VIDEO_LUT_CURSOR_DAC),                                      \
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

CLASSDESCRIPTOR NV20_ClassDescriptors[] =
{
    NV20_ARCHITECTURE_CLASS_DESCRIPTORS
};

U032 NV20_NumClassDescs = (sizeof (NV20_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//-----------------------------------------------------------------------
// NV20 HAL function table layout.
//-----------------------------------------------------------------------

//
// temporary stub routine
// 
RM_STATUS nvHalStub_NV20(VOID *arg) { return RM_ERROR; }

// mc interfaces
extern RM_STATUS nvHalMcControl_NV20(VOID *);
extern RM_STATUS nvHalMcPower_NV20(VOID *);

// fifo interfaces
extern RM_STATUS nvHalFifoControl_NV20(VOID *);
extern RM_STATUS nvHalFifoAllocPio_NV20(VOID *);
extern RM_STATUS nvHalFifoAllocDma_NV20(VOID *);
extern RM_STATUS nvHalFifoFree_NV20(VOID *);
extern RM_STATUS nvHalFifoGetExceptionData_NV20(VOID *);
extern RM_STATUS nvHalFifoService_NV20(VOID *);
extern RM_STATUS nvHalFifoAccess_NV20(VOID *);
extern RM_STATUS nvHalFifoHashAdd_NV20(VOID *);
extern RM_STATUS nvHalFifoHashDelete_NV20(VOID *);
extern RM_STATUS nvHalFifoHashFunc_NV20(VOID *);
extern RM_STATUS nvHalFifoHashSearch_NV20(VOID *);

// framebuffer interfaces
extern RM_STATUS nvHalFbControl_NV20(VOID *);
extern RM_STATUS nvHalFbAlloc_NV20(VOID *);
extern RM_STATUS nvHalFbFree_NV20(VOID *);
extern RM_STATUS nvHalFbSetAllocParameters_NV20(VOID *);
extern RM_STATUS nvHalFbGetSurfacePitch_NV20(VOID *);
extern RM_STATUS nvHalFbGetSurfaceDimensions_NV20(VOID *);
extern RM_STATUS nvHalFbLoadOverride_NV20(VOID *);

// graphics interfaces
extern RM_STATUS nvHalGrControl_NV20(VOID *);
extern RM_STATUS nvHalGrAlloc_NV20(VOID *);
extern RM_STATUS nvHalGrFree_NV20(VOID *);
extern RM_STATUS nvHalGrGetExceptionData_NV20(VOID *);
extern RM_STATUS nvHalGrService_NV20(VOID *);
extern RM_STATUS nvHalGrGetNotifyData_NV20(VOID *);
extern RM_STATUS nvHalGrSetObjectContext_NV20(VOID *);
#define nvHalGrLoadOverride_NV20 nvHalStub_NV20

// dma interfaces
extern RM_STATUS nvHalDmaControl_NV20(VOID *);
extern RM_STATUS nvHalDmaAlloc_NV20(VOID *);
extern RM_STATUS nvHalDmaFree_NV20(VOID *);
extern RM_STATUS nvHalDmaGetInstSize_NV20(VOID *);

// dac interfaces
extern RM_STATUS nvHalDacControl_NV20(VOID *);
extern RM_STATUS nvHalDacAlloc_NV20(VOID *);
extern RM_STATUS nvHalDacFree_NV20(VOID *);
extern RM_STATUS nvHalDacSetStartAddr_NV20(VOID *);
extern RM_STATUS nvHalDacProgramMClk_NV20(VOID *);
extern RM_STATUS nvHalDacProgramNVClk_NV20(VOID *);
extern RM_STATUS nvHalDacProgramPClk_NV20(VOID *);
extern RM_STATUS nvHalDacProgramCursorImage_NV20(VOID *);
extern RM_STATUS nvHalDacGetRasterPosition_NV20(VOID *);
extern RM_STATUS nvHalDacValidateArbSettings_NV20(VOID *arg);
extern RM_STATUS nvHalDacUpdateArbSettings_NV20(VOID *arg);

// mediaport interfaces
extern RM_STATUS nvHalMpControl_NV20(VOID *);
extern RM_STATUS nvHalMpAlloc_NV20(VOID *);
extern RM_STATUS nvHalMpFree_NV20(VOID *);
extern RM_STATUS nvHalMpMethod_NV20(VOID *);
extern RM_STATUS nvHalMpGetEventStatus_NV20(VOID *);
extern RM_STATUS nvHalMpServiceEvent_NV20(VOID *);

// mediaport interfaces
extern RM_STATUS nvHalVideoControl_NV20(VOID *);
extern RM_STATUS nvHalVideoAlloc_NV20(VOID *);
extern RM_STATUS nvHalVideoFree_NV20(VOID *);
extern RM_STATUS nvHalVideoMethod_NV20(VOID *);
extern RM_STATUS nvHalVideoGetEventStatus_NV20(VOID *);
extern RM_STATUS nvHalVideoServiceEvent_NV20(VOID *);

HALFUNCS_000 NV20_HalFuncs_000 =
{
    // mc interfaces
    nvHalMcControl_NV20,
    nvHalMcPower_NV20,

    // fifo interfaces
    nvHalFifoControl_NV20,
    nvHalFifoAllocPio_NV20,
    nvHalFifoAllocDma_NV20,
    nvHalFifoFree_NV20,
    nvHalFifoGetExceptionData_NV20,
    nvHalFifoService_NV20,
    nvHalFifoAccess_NV20,
    nvHalFifoHashAdd_NV20,
    nvHalFifoHashDelete_NV20,
    nvHalFifoHashFunc_NV20,
    nvHalFifoHashSearch_NV20,

    // framebuffer interfaces
    nvHalFbControl_NV20,
    nvHalFbAlloc_NV20,
    nvHalFbFree_NV20,
    nvHalFbSetAllocParameters_NV20,
    nvHalFbGetSurfacePitch_NV20,
    nvHalFbGetSurfaceDimensions_NV20,
    nvHalFbLoadOverride_NV20,

    // graphics interfaces
    nvHalGrControl_NV20,
    nvHalGrAlloc_NV20,
    nvHalGrFree_NV20,
    nvHalGrGetExceptionData_NV20,
    nvHalGrService_NV20,
    nvHalGrGetNotifyData_NV20,
    nvHalGrSetObjectContext_NV20,
    nvHalGrLoadOverride_NV20,

    // dma interface
    nvHalDmaControl_NV20,
    nvHalDmaAlloc_NV20,
    nvHalDmaFree_NV20,
    nvHalDmaGetInstSize_NV20,

    // dac interfaces
    nvHalDacControl_NV20,
    nvHalDacAlloc_NV20,
    nvHalDacFree_NV20,
    nvHalDacSetStartAddr_NV20,
    nvHalDacProgramMClk_NV20,
    nvHalDacProgramNVClk_NV20,
    nvHalDacProgramPClk_NV20,
    nvHalDacProgramCursorImage_NV20,
    nvHalDacGetRasterPosition_NV20,
    nvHalDacValidateArbSettings_NV20,
    nvHalDacUpdateArbSettings_NV20,

    // mp interfaces
    nvHalMpControl_NV20,
    nvHalMpAlloc_NV20,
    nvHalMpFree_NV20,
    nvHalMpMethod_NV20,
    nvHalMpGetEventStatus_NV20,
    nvHalMpServiceEvent_NV20,

    // mp interfaces
    nvHalVideoControl_NV20,
    nvHalVideoAlloc_NV20,
    nvHalVideoFree_NV20,
    nvHalVideoMethod_NV20,
    nvHalVideoGetEventStatus_NV20,
    nvHalVideoServiceEvent_NV20
};
