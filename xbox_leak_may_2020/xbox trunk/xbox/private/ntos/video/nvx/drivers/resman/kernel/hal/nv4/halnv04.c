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
* Module: HALNV04.C                                                         *
*   The chip dependent HAL data is kept here.                               *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nv4_ref.h>
#include <nvrm.h>
#include <nv4_hal.h>
#include "nvhw.h"

//-----------------------------------------------------------------------
// NV4 HAL engine descriptor table.
//-----------------------------------------------------------------------

//
// Engine default per-object instance memory sizes (16byte units).
//
#define GR_OBJINSTSZ                    0x1

ENGINEDESCRIPTOR NV04_EngineDescriptors[] =
{
    MKENGDECL(MC_ENGINE_TAG, 0, 0),
    MKENGMALLOC(MC_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MCHALINFO_NV04)/4),

    MKENGDECL(FB_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FB_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, (0x20000 + BIOS_IMAGE_PAD + sizeof (FBHALINFO_NV04))/4),

    MKENGDECL(DAC_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    MKENGDECL(TMR_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    // dma object size is determined by separate HAL call
    MKENGDECL(DMA_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),

    MKENGDECL(GR_ENGINE_TAG, NV_RAMHT_ENGINE_GRAPHICS, GR_OBJINSTSZ),
    MKENGMALLOC(GR_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (GRHALINFO_NV04)/4),

    MKENGDECL(FIFO_ENGINE_TAG, 0, 0),
    MKENGMALLOC(FIFO_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (FIFOHALINFO_NV04)/4),

    MKENGDECL(MP_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),
    MKENGMALLOC(MP_ENGINE_TAG, ENGMALLOC_TYPE_PRIVATE, sizeof (MPHALINFO_NV04)/4),

    MKENGDECL(VIDEO_ENGINE_TAG, NV_RAMHT_ENGINE_SW, 0),
};

U032 NV04_NumEngineDescs = (sizeof (NV04_EngineDescriptors) / sizeof (ENGINEDESCRIPTOR));

//-----------------------------------------------------------------------
// NV04 HAL class descriptor table.
//-----------------------------------------------------------------------

//
// The base classes supported for all NV4 Architectures
// 
#define NV4_ARCHITECTURE_CLASS_DESCRIPTORS                                                              \
    /* fifo engine classes */                                                                           \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_PIO),                                                  \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV03_CHANNEL_DMA),                                                  \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_PIO),                                                  \
    MKCLASSDECL(FIFO_ENGINE_TAG, 0, NV04_CHANNEL_DMA),                                                  \
                                                                                                        \
    /* graphics engine classes */                                                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA_SOLID, NV01_CONTEXT_BETA),                          \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV01_CONTEXT_COLOR_KEY),                      \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV01_CONTEXT_PATTERN),                          \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_CLIP, NV01_CONTEXT_CLIP_RECTANGLE),                      \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_RENDER_SOLID_LIN),                                         \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_RENDER_SOLID_TRIANGLE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_RENDER_SOLID_RECTANGLE),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_IMAGE_BLIT),                                               \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_IMAGE_FROM_CPU),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV01_NULL),                                                     \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_STRETCHED_IMAGE_FROM_CPU),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_SCALED_IMAGE_FROM_MEMORY),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_MEMORY_TO_MEMORY_FORMAT),                                  \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_ROP, NV03_CONTEXT_ROP),                                  \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_DX3_TEXTURED_TRIANGLE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV03_GDI_RECTANGLE_TEXT),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_DESTINATION),       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV03_CONTEXT_SURFACE_2D_SOURCE),            \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_DEPTH),             \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV03_CONTEXT_SURFACE_3D_COLOR),            \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_DVD, NV04_DVD_SUBPICTURE),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_2D, NV04_CONTEXT_SURFACES_2D),                  \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_PATTERN, NV04_CONTEXT_PATTERN),                          \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_GDI_RECTANGLE_TEXT),                                       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_SWIZZLED, NV04_CONTEXT_SURFACE_SWIZZLED),       \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_SURFACES_3D, NV04_CONTEXT_SURFACES_3D),                  \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_DX5_TEXTURED_TRIANGLE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_DX6_MULTI_TEXTURE_TRIANGLE),                               \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_COLOR_KEY, NV04_CONTEXT_COLOR_KEY),                      \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_RENDER_SOLID_LIN),                                         \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_RENDER_SOLID_TRIANGLE),                                    \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_RENDER_SOLID_RECTANGLE),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_IMAGE_BLIT),                                               \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_INDEXED_IMAGE_FROM_CPU),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_IMAGE_FROM_CPU),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, GR_LOCK_CONTEXT_BETA, NV04_CONTEXT_BETA),                                \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_STRETCHED_IMAGE_FROM_CPU),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV04_SCALED_IMAGE_FROM_MEMORY),                                 \
                                                                                                        \
    /* dac engine classes */                                                                            \
    MKCLASSDECL(DAC_ENGINE_TAG, NO_LOCK, NV04_VIDEO_LUT_CURSOR_DAC),                                    \
                                                                                                        \
    /* mediaport engine classes */                                                                      \
    MKCLASSDECL(MP_ENGINE_TAG, MP_LOCK_DECODER, NV03_EXTERNAL_VIDEO_DECODER),                           \
    MKCLASSDECL(MP_ENGINE_TAG, MP_LOCK_DECOMPRESSOR, NV03_EXTERNAL_VIDEO_DECOMPRESSOR),                 \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV01_EXTERNAL_PARALLEL_BUS),                                    \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV03_EXTERNAL_MONITOR_BUS),                                     \
    MKCLASSDECL(MP_ENGINE_TAG, NO_LOCK, NV04_EXTERNAL_SERIAL_BUS),                                      \
                                                                                                        \
    /* video engine classes */                                                                          \
    MKCLASSDECL(VIDEO_ENGINE_TAG, NO_LOCK, NV04_VIDEO_OVERLAY),

//
// The classes supported on NV4
//
CLASSDESCRIPTOR NV04_ClassDescriptors[] =
{
    NV4_ARCHITECTURE_CLASS_DESCRIPTORS
};

U032 NV04_NumClassDescs = (sizeof (NV04_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//
// The classes supported on NV5
// 
#define NV5_IMPLEMENTATION_CLASS_DESCRIPTORS                                                            \
    /* graphics engine classes */                                                                       \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_SCALED_IMAGE_FROM_MEMORY),                                 \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_INDEXED_IMAGE_FROM_CPU),                                   \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_IMAGE_FROM_CPU),                                           \
    MKCLASSDECL(GR_ENGINE_TAG, NO_LOCK, NV05_STRETCHED_IMAGE_FROM_CPU),                                 \
                                                                                                        \
    /* dac engine classes */                                                                            \
    MKCLASSDECL(DAC_ENGINE_TAG, NO_LOCK, NV05_VIDEO_LUT_CURSOR_DAC),

CLASSDESCRIPTOR NV05_ClassDescriptors[] =
{
    NV4_ARCHITECTURE_CLASS_DESCRIPTORS
    NV5_IMPLEMENTATION_CLASS_DESCRIPTORS
};

U032 NV05_NumClassDescs = (sizeof (NV05_ClassDescriptors) / sizeof (CLASSDESCRIPTOR));

//-----------------------------------------------------------------------
// NV4 HAL function table layout.
//-----------------------------------------------------------------------

// mc interfaces
RM_STATUS nvHalMcControl_NV04(VOID *);
RM_STATUS nvHalMcPower_NV04(VOID *);

// fifo interfaces
extern RM_STATUS nvHalFifoControl_NV04(VOID *);
extern RM_STATUS nvHalFifoAllocPio_NV04(VOID *);
extern RM_STATUS nvHalFifoAllocDma_NV04(VOID *);
extern RM_STATUS nvHalFifoFree_NV04(VOID *);
extern RM_STATUS nvHalFifoGetExceptionData_NV04(VOID *);
extern RM_STATUS nvHalFifoService_NV04(VOID *);
extern RM_STATUS nvHalFifoAccess_NV04(VOID *);
extern RM_STATUS nvHalFifoHashAdd_NV04(VOID *);
extern RM_STATUS nvHalFifoHashDelete_NV04(VOID *);
extern RM_STATUS nvHalFifoHashFunc_NV04(VOID *);
extern RM_STATUS nvHalFifoHashSearch_NV04(VOID *);

// framebuffer interfaces
extern RM_STATUS nvHalFbControl_NV04(VOID *);
extern RM_STATUS nvHalFbAlloc_NV04(VOID *);
extern RM_STATUS nvHalFbFree_NV04(VOID *);
extern RM_STATUS nvHalFbSetAllocParameters_NV04(VOID *);
extern RM_STATUS nvHalFbGetSurfacePitch_NV04(VOID *);
extern RM_STATUS nvHalFbGetSurfaceDimensions_NV04(VOID *);
extern RM_STATUS nvHalFbLoadOverride_NV04(VOID *);

// graphics interfaces
extern RM_STATUS nvHalGrControl_NV04(VOID *);
extern RM_STATUS nvHalGrAlloc_NV04(VOID *);
extern RM_STATUS nvHalGrFree_NV04(VOID *);
extern RM_STATUS nvHalGrGetExceptionData_NV04(VOID *);
extern RM_STATUS nvHalGrService_NV04(VOID *);
extern RM_STATUS nvHalGrGetNotifyData_NV04(VOID *);
extern RM_STATUS nvHalGrSetObjectContext_NV04(VOID *);
extern RM_STATUS nvHalGrLoadOverride_NV04(VOID *);

// dma interfaces
extern RM_STATUS nvHalDmaControl_NV04(VOID *);
extern RM_STATUS nvHalDmaAlloc_NV04(VOID *);
extern RM_STATUS nvHalDmaFree_NV04(VOID *);
extern RM_STATUS nvHalDmaGetInstSize_NV04(VOID *);

// dac interfaces
extern RM_STATUS nvHalDacControl_NV04(VOID *);
extern RM_STATUS nvHalDacAlloc_NV04(VOID *);
extern RM_STATUS nvHalDacFree_NV04(VOID *);
extern RM_STATUS nvHalDacSetStartAddr_NV04(VOID *);
extern RM_STATUS nvHalDacProgramMClk_NV04(VOID *);
extern RM_STATUS nvHalDacProgramNVClk_NV04(VOID *);
extern RM_STATUS nvHalDacProgramPClk_NV04(VOID *);
extern RM_STATUS nvHalDacProgramCursorImage_NV04(VOID *);
extern RM_STATUS nvHalDacGetRasterPosition_NV04(VOID *);
extern RM_STATUS nvHalDacValidateArbSettings_NV04(VOID *arg);
extern RM_STATUS nvHalDacUpdateArbSettings_NV04(VOID *arg);


// mediaport interfaces
extern RM_STATUS nvHalMpControl_NV04(VOID *);
extern RM_STATUS nvHalMpAlloc_NV04(VOID *);
extern RM_STATUS nvHalMpFree_NV04(VOID *);
extern RM_STATUS nvHalMpMethod_NV04(VOID *);
extern RM_STATUS nvHalMpGetEventStatus_NV04(VOID *);
extern RM_STATUS nvHalMpServiceEvent_NV04(VOID *);

// video interfaces
extern RM_STATUS nvHalVideoControl_NV04(VOID *);
extern RM_STATUS nvHalVideoAlloc_NV04(VOID *);
extern RM_STATUS nvHalVideoFree_NV04(VOID *);
extern RM_STATUS nvHalVideoMethod_NV04(VOID *);
extern RM_STATUS nvHalVideoGetEventStatus_NV04(VOID *);
extern RM_STATUS nvHalVideoServiceEvent_NV04(VOID *);

HALFUNCS_000 NV04_HalFuncs_000 =
{
    // mc interfaces
    nvHalMcControl_NV04,
    nvHalMcPower_NV04,

    // fifo interfaces
    nvHalFifoControl_NV04,
    nvHalFifoAllocPio_NV04,
    nvHalFifoAllocDma_NV04,
    nvHalFifoFree_NV04,
    nvHalFifoGetExceptionData_NV04,
    nvHalFifoService_NV04,
    nvHalFifoAccess_NV04,
    nvHalFifoHashAdd_NV04,
    nvHalFifoHashDelete_NV04,
    nvHalFifoHashFunc_NV04,
    nvHalFifoHashSearch_NV04,

    // framebuffer interfaces
    nvHalFbControl_NV04,
    nvHalFbAlloc_NV04,
    nvHalFbFree_NV04,
    nvHalFbSetAllocParameters_NV04,
    nvHalFbGetSurfacePitch_NV04,
    nvHalFbGetSurfaceDimensions_NV04,
    nvHalFbLoadOverride_NV04,

    // graphics interfaces
    nvHalGrControl_NV04,
    nvHalGrAlloc_NV04,
    nvHalGrFree_NV04,
    nvHalGrGetExceptionData_NV04,
    nvHalGrService_NV04,
    nvHalGrGetNotifyData_NV04,
    nvHalGrSetObjectContext_NV04,
    nvHalGrLoadOverride_NV04,

    // dma interface
    nvHalDmaControl_NV04,
    nvHalDmaAlloc_NV04,
    nvHalDmaFree_NV04,
    nvHalDmaGetInstSize_NV04,

    // dac interfaces
    nvHalDacControl_NV04,
    nvHalDacAlloc_NV04,
    nvHalDacFree_NV04,
    nvHalDacSetStartAddr_NV04,
    nvHalDacProgramMClk_NV04,
    nvHalDacProgramNVClk_NV04,
    nvHalDacProgramPClk_NV04,
    nvHalDacProgramCursorImage_NV04,
    nvHalDacGetRasterPosition_NV04,
    nvHalDacValidateArbSettings_NV04,
    nvHalDacUpdateArbSettings_NV04,

    // mp interfaces
    nvHalMpControl_NV04,
    nvHalMpAlloc_NV04,
    nvHalMpFree_NV04,
    nvHalMpMethod_NV04,
    nvHalMpGetEventStatus_NV04,
    nvHalMpServiceEvent_NV04,

    // video interfaces
    nvHalVideoControl_NV04,
    nvHalVideoAlloc_NV04,
    nvHalVideoFree_NV04,
    nvHalVideoMethod_NV04,
    nvHalVideoGetEventStatus_NV04,
    nvHalVideoServiceEvent_NV04
};
