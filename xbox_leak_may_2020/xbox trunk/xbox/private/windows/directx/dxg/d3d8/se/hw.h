/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       hw.h
 *  Content:    Miscellaneous hardware defines
 *
 ***************************************************************************/

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

typedef enum
{
    // From nv32.h: NUMBER_OF_SUBCHANNELS is 8.

    SUBCH_3D                 = 0,   // D3D_KELVIN_PRIMITIVE (0x097)
    SUBCH_MEMCOPY            = 1,   // D3D_MEMORY_TO_MEMORY_COPY (0x039)
    SUBCH_RECTCOPY           = 2,   // D3D_RECTANGLE_COPY (0x5F)
    SUBCH_RECTCOPYSURFACES   = 3,   // D3D_RECTANGLE_COPY_SURFACES (0x062)
    SUBCH_RECTCOPYOPTIONS    = 4,   // D3D_RECTANGLE_COPY_COLOR_KEY, BETA1, ...

    SUBCH_UNUSED0            = 5,
    SUBCH_UNUSED1            = 6,
    SUBCH_UNUSED2            = 7,

} SubChannel;

// Object handles

#define NVX_SOFTWARE_CLASS_HANDLE                               1 // Old:0xDD008000 
#define NV_DD_DMA_PUSHER_SYNC_NOTIFIER_CONTEXT_DMA_TO_MEMORY    2 // Old:0xDD00101A
#define NV_DD_DMA_CONTEXT_DMA_IN_VIDEO_MEMORY                   3 // Old:0xDD001010
#define NV_DD_DMA_CONTEXT_DMA_TO_VIDEO_MEMORY                   4 // Old:0xDD00101C
#define NV_DD_DMA_CONTEXT_DMA_FROM_VIDEO_MEMORY                 5 // Old:0xDD001002
#define NV_DD_DMA_PUSHER_CONTEXT_DMA_FROM_MEMORY                6 // Old:0xDD005000
#define D3D_MEMCOPY_NOTIFIER_CONTEXT_DMA_TO_MEMORY              7 // Old:0xDD00101F
#define D3D_SEMAPHORE_CONTEXT_DMA_IN_MEMORY                     8
#define D3D_COLOR_CONTEXT_DMA_IN_VIDEO_MEMORY                   9
#define D3D_ZETA_CONTEXT_DMA_IN_VIDEO_MEMORY                   10
#define D3D_COPY_CONTEXT_DMA_IN_VIDEO_MEMORY                   11
#define D3D_CONTEXT_IN_CACHED_MEMORY                           12

#define D3D_KELVIN_PRIMITIVE                                   13 // Old:0xD3D09701
#define D3D_MEMORY_TO_MEMORY_COPY                              14 // Old:0xD3D03900
#define D3D_SCALED_IMAGE_FROM_MEMORY                           15 // Old:0xD3D07701  
#define D3D_RECTANGLE_COPY                                     16
#define D3D_RECTANGLE_COPY_SURFACES                            17

#define D3D_RECTANGLE_COPY_PATTERN                             18
#define D3D_RECTANGLE_COPY_COLOR_KEY                           19
#define D3D_RECTANGLE_COPY_ROP                                 20
#define D3D_RECTANGLE_COPY_BETA1                               21
#define D3D_RECTANGLE_COPY_BETA4                               22
#define D3D_RECTANGLE_COPY_CLIP_RECTANGLE                      24

#define D3D_NULL_OBJECT                                        25

// Handy register access macros:

#define READ_REGISTER(Register)         (*(volatile DWORD *)(Register))
#define WRITE_REGISTER(Register, Value) (*(volatile DWORD *)(Register) = (Value))


// Maximum number of DWORDs in a single push-buffer instruction:

#define MAX_PUSH_METHOD_SIZE 2047

// Number of skinning blend matrices:

#define NUM_BLEND_MATRICES 4
#define NUM_COMBINERS 8

// The user can apportion amongst 136 vertex shader program slots:

#define VSHADER_PROGRAM_SLOTS 136

// Because the h/w is OpenGL'ized, we must D3D'ize it by adding 0.5 so
// that D3D pixels line up with OpenGL pixels.  Halfsubpixelbias is
// to adjust for floating point numbers which are not accurately
// represented.  E.g., -.7 is actually represented as -.699999
// If coordinates were only positive, this would be ok since it would
// result in being just to the left of the pixel center (integer coord).
// But, since the viewport is -2K...2K and the negative portion is used
// first, we actually end up being to the right of pixel center.
// Since the h/w doesn't do rounding to nearest 1/16 of a pixel, we
// adjust this here by subtracting 1/32 to round it.

const float KELVIN_D3D2OGL = 0.5f;              // add 0.5 so that D3D pixels line up with OGL pixels
const float KELVIN_HALFSUBPIXELBIAS = 0.03125f; // round to nearest 1/16 pixel grid
const float KELVIN_BORDER = 0.53125f;           // (KELVIN_D3D2OGL + KELVIN_HALFSUBPIXELBIAS)


// maximum fog scale = 2^13. (the HW has 14 bits. we leave one for roundoff)

const float MAX_FOG_SCALE = 8192.0f;

//------------------------------------------------------------------------------
// SwapRgb
//
// OpenGL'ize the D3D color

FORCEINLINE DWORD SwapRgb(
    D3DCOLOR color) 
{ 
    return (color & 0xff00ff00) | 
           ((color & 0x00ff0000) >> 16) | 
           ((color & 0x000000ff) << 16); 
}

} // end namespace
