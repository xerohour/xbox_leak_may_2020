/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvTranslate.cpp                                                   *
*   definitions of tables used to translate from generic (D3D or NV)        *
*   constants to HW-specific constants                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*   Craig Duttweiler    bertrem     03May99     created                     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

//--------------------------------------------------------------------------
// surface formats
//--------------------------------------------------------------------------

const DWORD nvSurfaceFormatToBPP[NV_SURFACE_FORMAT_COUNT] =
{
    1,                                                          // NV_SURFACE_FORMAT_Y8
    1,                                                          // NV_SURFACE_FORMAT_AY8
    2,                                                          // NV_SURFACE_FORMAT_A1R5G5B5
    2,                                                          // NV_SURFACE_FORMAT_X1R5G5B5
    2,                                                          // NV_SURFACE_FORMAT_A4R4G4B4
    2,                                                          // NV_SURFACE_FORMAT_R5G6B5
    4,                                                          // NV_SURFACE_FORMAT_A8R8G8B8
    4,                                                          // NV_SURFACE_FORMAT_X1A7R8G8B8
    4,                                                          // NV_SURFACE_FORMAT_X8R8G8B8
    1,                                                          // NV_SURFACE_FORMAT_I8_A1R5G5B5
    1,                                                          // NV_SURFACE_FORMAT_I8_R5G6B5
    1,                                                          // NV_SURFACE_FORMAT_I8_A4R4G4B4
    1,                                                          // NV_SURFACE_FORMAT_I8_A8R8G8B8
    8,                                                          // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    16,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    16,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    4,                                                          // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    1,                                                          // NV_SURFACE_FORMAT_IMAGE_Y8
    1,                                                          // NV_SURFACE_FORMAT_IMAGE_SY8
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_X7SY9
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_R8B8
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_G8B8
    2,                                                          // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    2,                                                          // NV_SURFACE_FORMAT_Z16
    4,                                                          // NV_SURFACE_FORMAT_Z24S8
    4,                                                          // NV_SURFACE_FORMAT_Z24X8
    2,                                                          // NV_SURFACE_FORMAT_DV8DU8
    2,                                                          // NV_SURFACE_FORMAT_L6DV5DU5
    4,                                                          // NV_SURFACE_FORMAT_X8L8DV8DU8
    4,                                                          // NV_SURFACE_FORMAT_HILO_1_V16U16
    4,                                                          // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    4,                                                          // NV_SURFACE_FORMAT_A8B8G8R8
    0                                                           // NV_SURFACE_FORMAT_UNKNOWN
};

const DWORD nv052SurfaceFormat[NV_SURFACE_FORMAT_COUNT] = // for swizzling
{
    NV052_SET_FORMAT_COLOR_LE_Y8,                               // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    ~0,                                                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV052_SET_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5,                // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV052_SET_FORMAT_COLOR_LE_R5G6B5,                           // NV_SURFACE_FORMAT_R5G6B5
    NV052_SET_FORMAT_COLOR_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_A8R8G8B8
    NV052_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8,            // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV052_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8,                // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN

};

const DWORD nv053SurfaceFormat[NV_SURFACE_FORMAT_COUNT] =
{
    ~0,                                                         // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    ~0,                                                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV053_SET_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5,                // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV053_SET_FORMAT_COLOR_LE_R5G6B5,                           // NV_SURFACE_FORMAT_R5G6B5
    NV053_SET_FORMAT_COLOR_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_A8R8G8B8
    NV053_SET_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8,            // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV053_SET_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8,                // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};

#if (NVARCH >= 0x010)
const DWORD nv056SurfaceFormat[NV_SURFACE_FORMAT_COUNT] =
{
    ~0,                                                         // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    ~0,                                                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV056_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5,        // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV056_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5,                   // NV_SURFACE_FORMAT_R5G6B5
    NV056_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8,                 // NV_SURFACE_FORMAT_A8R8G8B8
    NV056_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8,    // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV056_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8,        // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_I8_A8R8G8B8,              // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};

const DWORD nv062SurfaceFormat[NV_SURFACE_FORMAT_COUNT] =
{
    NV062_SET_COLOR_FORMAT_LE_Y8,                               // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    ~0,                                                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV062_SET_COLOR_FORMAT_LE_X1R5G5B5_Z1R5G5B5,                // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV062_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_R5G6B5
    NV062_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_A8R8G8B8
    NV062_SET_COLOR_FORMAT_LE_X1A7R8G8B8_Z1A7R8G8B8,            // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV062_SET_COLOR_FORMAT_LE_X8R8G8B8_Z8R8G8B8,                // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};
#endif

const DWORD nv077SurfaceFormat[NV_SURFACE_FORMAT_COUNT] = // for swizzling
{
    ~0,                                                         // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    NV077_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_A1R5G5B5
    NV077_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_X1R5G5B5
    NV077_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_A4R4G4B4
    NV077_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_R5G6B5
    NV077_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_A8R8G8B8
    NV077_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV077_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    NV077_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_Z16
    NV077_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_Z24S8
    NV077_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};

#if (NVARCH >= 0x010)
const DWORD nv089SurfaceFormat[NV_SURFACE_FORMAT_COUNT] =
{
    NV089_SET_COLOR_FORMAT_LE_Y8,                               // NV_SURFACE_FORMAT_Y8
    NV089_SET_COLOR_FORMAT_LE_AY8,                              // NV_SURFACE_FORMAT_AY8
    NV089_SET_COLOR_FORMAT_LE_A1R5G5B5,                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV089_SET_COLOR_FORMAT_LE_X1R5G5B5,                         // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV089_SET_COLOR_FORMAT_LE_R5G6B5,                           // NV_SURFACE_FORMAT_R5G6B5
    NV089_SET_COLOR_FORMAT_LE_A8R8G8B8,                         // NV_SURFACE_FORMAT_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV089_SET_COLOR_FORMAT_LE_X8R8G8B8,                         // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};
#endif

#if (NVARCH >= 0x020)
const DWORD nv097SurfaceFormat[NV_SURFACE_FORMAT_COUNT] =
{
    ~0,                                                         // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    ~0,                                                         // NV_SURFACE_FORMAT_A1R5G5B5
    NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5,        // NV_SURFACE_FORMAT_X1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_A4R4G4B4
    NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5,                   // NV_SURFACE_FORMAT_R5G6B5
    NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8,                 // NV_SURFACE_FORMAT_A8R8G8B8
    NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8,    // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8,        // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    NV097_SET_SURFACE_FORMAT_ZETA_Z16,                          // NV_SURFACE_FORMAT_Z16
    NV097_SET_SURFACE_FORMAT_ZETA_Z24S8,                        // NV_SURFACE_FORMAT_Z24S8
    NV097_SET_SURFACE_FORMAT_ZETA_Z24S8,                        // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};
#endif

//--------------------------------------------------------------------------
// texture formats
//--------------------------------------------------------------------------

const DWORD nv054TextureFormat[NV_SURFACE_FORMAT_COUNT] =
{
    NV054_FORMAT_COLOR_LE_Y8,                                   // NV_SURFACE_FORMAT_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_AY8
    NV054_FORMAT_COLOR_LE_A1R5G5B5,                             // NV_SURFACE_FORMAT_A1R5G5B5
    NV054_FORMAT_COLOR_LE_X1R5G5B5,                             // NV_SURFACE_FORMAT_X1R5G5B5
    NV054_FORMAT_COLOR_LE_A4R4G4B4,                             // NV_SURFACE_FORMAT_A4R4G4B4
    NV054_FORMAT_COLOR_LE_R5G6B5,                               // NV_SURFACE_FORMAT_R5G6B5
    NV054_FORMAT_COLOR_LE_A8R8G8B8,                             // NV_SURFACE_FORMAT_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV054_FORMAT_COLOR_LE_X8R8G8B8,                             // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};

const DWORD nv055TextureFormat[NV_SURFACE_FORMAT_COUNT] =
{
    ~0,                                                         // NV_SURFACE_FORMAT_Y8
    NV055_FORMAT_COLOR_LE_AY8,                                  // NV_SURFACE_FORMAT_AY8
    NV055_FORMAT_COLOR_LE_A1R5G5B5,                             // NV_SURFACE_FORMAT_A1R5G5B5
    NV055_FORMAT_COLOR_LE_X1R5G5B5,                             // NV_SURFACE_FORMAT_X1R5G5B5
    NV055_FORMAT_COLOR_LE_A4R4G4B4,                             // NV_SURFACE_FORMAT_A4R4G4B4
    NV055_FORMAT_COLOR_LE_R5G6B5,                               // NV_SURFACE_FORMAT_R5G6B5
    NV055_FORMAT_COLOR_LE_A8R8G8B8,                             // NV_SURFACE_FORMAT_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV055_FORMAT_COLOR_LE_X8R8G8B8,                             // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_Y8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SY8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_X7SY9
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_R8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};

#if (NVARCH >= 0x010)
const DWORD nv056TextureFormat[NV_SURFACE_FORMAT_COUNT] =
{
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_Y8,                       // NV_SURFACE_FORMAT_Y8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_AY8,                      // NV_SURFACE_FORMAT_AY8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_A1R5G5B5,                 // NV_SURFACE_FORMAT_A1R5G5B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_X1R5G5B5,                 // NV_SURFACE_FORMAT_X1R5G5B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_A4R4G4B4,                 // NV_SURFACE_FORMAT_A4R4G4B4
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_R5G6B5,                   // NV_SURFACE_FORMAT_R5G6B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_A8R8G8B8,                 // NV_SURFACE_FORMAT_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_X8R8G8B8,                 // NV_SURFACE_FORMAT_X8R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_I8_A1R5G5B5,              // NV_SURFACE_FORMAT_I8_A1R5G5B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_I8_R5G6B5,                // NV_SURFACE_FORMAT_I8_R5G6B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_I8_A4R4G4B4,              // NV_SURFACE_FORMAT_I8_A4R4G4B4
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_I8_A8R8G8B8,              // NV_SURFACE_FORMAT_I8_A8R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_DXT1_A1R5G5B5,            // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_DXT23_A8R8G8B8,           // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_DXT45_A8R8G8B8,           // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A1R5G5B5,           // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_R5G6B5,             // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_A8R8G8B8,           // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_Y8,                 // NV_SURFACE_FORMAT_IMAGE_Y8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_SY8,                // NV_SURFACE_FORMAT_IMAGE_SY8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_X7SY9,              // NV_SURFACE_FORMAT_IMAGE_X7SY9
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_R8B8,               // NV_SURFACE_FORMAT_IMAGE_R8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_G8B8,               // NV_SURFACE_FORMAT_IMAGE_G8B8
    NV056_SET_TEXTURE_FORMAT_COLOR_LE_IMAGE_SG8SB8,             // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    ~0,                                                         // NV_SURFACE_FORMAT_DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_L6DV5DU5
    ~0,                                                         // NV_SURFACE_FORMAT_X8L8DV8DU8
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_1_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    ~0,                                                         // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN
};
#endif

#if (NVARCH >= 0x020)
const DWORD nv097TextureFormat[NV_SURFACE_FORMAT_COUNT] =
{
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_Y8,                       // NV_SURFACE_FORMAT_Y8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_AY8,                      // NV_SURFACE_FORMAT_AY8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A1R5G5B5,                 // NV_SURFACE_FORMAT_A1R5G5B5
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X1R5G5B5,                 // NV_SURFACE_FORMAT_X1R5G5B5
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A4R4G4B4,                 // NV_SURFACE_FORMAT_A4R4G4B4
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G6B5,                   // NV_SURFACE_FORMAT_R5G6B5
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8,                 // NV_SURFACE_FORMAT_A8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_X1A7R8G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X8R8G8B8,                 // NV_SURFACE_FORMAT_X8R8G8B8
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A1R5G5B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_R5G6B5
    ~0,                                                         // NV_SURFACE_FORMAT_I8_A4R4G4B4
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8,              // NV_SURFACE_FORMAT_I8_A8R8G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5,             // NV_SURFACE_FORMAT_DXT1_A1R5G5B5
    NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8,            // NV_SURFACE_FORMAT_DXT23_A8R8G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8,            // NV_SURFACE_FORMAT_DXT45_A8R8G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5,           // NV_SURFACE_FORMAT_IMAGE_A1R5G5B5
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5,             // NV_SURFACE_FORMAT_IMAGE_R5G6B5
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8,           // NV_SURFACE_FORMAT_IMAGE_A8R8G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8,                 // NV_SURFACE_FORMAT_IMAGE_Y8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8,                // NV_SURFACE_FORMAT_IMAGE_SY8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9,              // NV_SURFACE_FORMAT_IMAGE_X7SY9
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8,               // NV_SURFACE_FORMAT_IMAGE_R8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8,               // NV_SURFACE_FORMAT_IMAGE_G8B8
    NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8,             // NV_SURFACE_FORMAT_IMAGE_SG8SB8
    ~0,                                                         // NV_SURFACE_FORMAT_Z16
    ~0,                                                         // NV_SURFACE_FORMAT_Z24S8
    ~0,                                                         // NV_SURFACE_FORMAT_Z24X8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_G8B8,                     // NV_SURFACE_FORMAT_DV8DU8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5,                   // NV_SURFACE_FORMAT_L6DV5DU5
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X8R8G8B8,                 // NV_SURFACE_FORMAT_X8L8DV8DU8
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_YB_16_YA_16,              // NV_SURFACE_FORMAT_HILO_1_V16U16
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_YB_16_YA_16,              // NV_SURFACE_FORMAT_HILO_HEMI_V16U16
    NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8B8G8R8,                 // NV_SURFACE_FORMAT_A8B8G8R8
    ~0                                                          // NV_SURFACE_FORMAT_UNKNOWN

};
#endif

//--------------------------------------------------------------------------
// context DMAs
//--------------------------------------------------------------------------

const DWORD nv054TextureContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV054_FORMAT_CONTEXT_DMA_A,
    NV054_FORMAT_CONTEXT_DMA_B
};

const DWORD nv055TextureContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV055_FORMAT_CONTEXT_DMA_A,
    NV055_FORMAT_CONTEXT_DMA_B
};

#if (NVARCH >= 0x010)
const DWORD nv056TextureContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_A,
    NV056_SET_TEXTURE_FORMAT_CONTEXT_DMA_B
};
#endif

#if (NVARCH >= 0x020)
const DWORD nv097TextureContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_A,
    NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA_B
};
#endif

#if (NVARCH >= 0x010)
const DWORD nv056PaletteContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV056_SET_TEXTURE_PALETTE_CONTEXT_DMA_A,
    NV056_SET_TEXTURE_PALETTE_CONTEXT_DMA_B
};
#endif

#if (NVARCH >= 0x020)
const DWORD nv097PaletteContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV097_SET_TEXTURE_PALETTE_CONTEXT_DMA_A,
    NV097_SET_TEXTURE_PALETTE_CONTEXT_DMA_B
};

const DWORD nv097VertexContextDma[NV_CONTEXT_DMA_COUNT] =
{
    ~0,
    NV097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_A,
    NV097_SET_VERTEX_DATA_ARRAY_OFFSET_CONTEXT_DMA_VERTEX_B,
};
#endif


//--------------------------------------------------------------------------
// D3D conversion stuff
//--------------------------------------------------------------------------

#if (NVARCH >= 0x010)
// note we define these in terms of STENCIL_OP_FAIL, but the values are
// actually the same for STENCIL_OP_ZFAIL and STENCIL_OP_ZPASS
const DWORD nv056StencilOp[D3D_STENCIL_OP_COUNT] =
{
    ~0,                                     // undefined
    NV056_SET_STENCIL_OP_FAIL_V_KEEP,       // D3DSTENCILOP_KEEP    = 1
    NV056_SET_STENCIL_OP_FAIL_V_ZERO,       // D3DSTENCILOP_ZERO    = 2
    NV056_SET_STENCIL_OP_FAIL_V_REPLACE,    // D3DSTENCILOP_REPLACE = 3
    NV056_SET_STENCIL_OP_FAIL_V_INCRSAT,    // D3DSTENCILOP_INCRSAT = 4
    NV056_SET_STENCIL_OP_FAIL_V_DECRSAT,    // D3DSTENCILOP_DECRSAT = 5
    NV056_SET_STENCIL_OP_FAIL_V_INVERT,     // D3DSTENCILOP_INVERT  = 6
    NV056_SET_STENCIL_OP_FAIL_V_INCR,       // D3DSTENCILOP_INCR    = 7
    NV056_SET_STENCIL_OP_FAIL_V_DECR        // D3DSTENCILOP_DECR    = 8
};

// note we define these in terms of STENCIL_OP_FAIL, but the values are
// actually the same for STENCIL_OP_ZFAIL and STENCIL_OP_ZPASS
const DWORD nv097StencilOp[D3D_STENCIL_OP_COUNT] =
{
    ~0,                                     // undefined
    NV097_SET_STENCIL_OP_FAIL_V_KEEP,       // D3DSTENCILOP_KEEP    = 1
    NV097_SET_STENCIL_OP_FAIL_V_ZERO,       // D3DSTENCILOP_ZERO    = 2
    NV097_SET_STENCIL_OP_FAIL_V_REPLACE,    // D3DSTENCILOP_REPLACE = 3
    NV097_SET_STENCIL_OP_FAIL_V_INCRSAT,    // D3DSTENCILOP_INCRSAT = 4
    NV097_SET_STENCIL_OP_FAIL_V_DECRSAT,    // D3DSTENCILOP_DECRSAT = 5
    NV097_SET_STENCIL_OP_FAIL_V_INVERT,     // D3DSTENCILOP_INVERT  = 6
    NV097_SET_STENCIL_OP_FAIL_V_INCR,       // D3DSTENCILOP_INCR    = 7
    NV097_SET_STENCIL_OP_FAIL_V_DECR        // D3DSTENCILOP_DECR    = 8
};

const DWORD nv056StencilFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV056_SET_STENCIL_FUNC_V_NEVER,         // D3DCMP_NEVER        = 1
    NV056_SET_STENCIL_FUNC_V_LESS,          // D3DCMP_LESS         = 2
    NV056_SET_STENCIL_FUNC_V_EQUAL,         // D3DCMP_EQUAL        = 3
    NV056_SET_STENCIL_FUNC_V_LEQUAL,        // D3DCMP_LESSEQUAL    = 4
    NV056_SET_STENCIL_FUNC_V_GREATER,       // D3DCMP_GREATER      = 5
    NV056_SET_STENCIL_FUNC_V_NOTEQUAL,      // D3DCMP_NOTEQUAL     = 6
    NV056_SET_STENCIL_FUNC_V_GEQUAL,        // D3DCMP_GREATEREQUAL = 7
    NV056_SET_STENCIL_FUNC_V_ALWAYS         // D3DCMP_ALWAYS       = 8
};

const DWORD nv097StencilFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV097_SET_STENCIL_FUNC_V_NEVER,         // D3DCMP_NEVER        = 1
    NV097_SET_STENCIL_FUNC_V_LESS,          // D3DCMP_LESS         = 2
    NV097_SET_STENCIL_FUNC_V_EQUAL,         // D3DCMP_EQUAL        = 3
    NV097_SET_STENCIL_FUNC_V_LEQUAL,        // D3DCMP_LESSEQUAL    = 4
    NV097_SET_STENCIL_FUNC_V_GREATER,       // D3DCMP_GREATER      = 5
    NV097_SET_STENCIL_FUNC_V_NOTEQUAL,      // D3DCMP_NOTEQUAL     = 6
    NV097_SET_STENCIL_FUNC_V_GEQUAL,        // D3DCMP_GREATEREQUAL = 7
    NV097_SET_STENCIL_FUNC_V_ALWAYS         // D3DCMP_ALWAYS       = 8
};

const DWORD nv056AlphaFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV056_SET_ALPHA_FUNC_V_NEVER,           // D3DCMP_NEVER        = 1
    NV056_SET_ALPHA_FUNC_V_LESS,            // D3DCMP_LESS         = 2
    NV056_SET_ALPHA_FUNC_V_EQUAL,           // D3DCMP_EQUAL        = 3
    NV056_SET_ALPHA_FUNC_V_LEQUAL,          // D3DCMP_LESSEQUAL    = 4
    NV056_SET_ALPHA_FUNC_V_GREATER,         // D3DCMP_GREATER      = 5
    NV056_SET_ALPHA_FUNC_V_NOTEQUAL,        // D3DCMP_NOTEQUAL     = 6
    NV056_SET_ALPHA_FUNC_V_GEQUAL,          // D3DCMP_GREATEREQUAL = 7
    NV056_SET_ALPHA_FUNC_V_ALWAYS           // D3DCMP_ALWAYS       = 8
};

const DWORD nv097AlphaFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV097_SET_ALPHA_FUNC_V_NEVER,           // D3DCMP_NEVER        = 1
    NV097_SET_ALPHA_FUNC_V_LESS,            // D3DCMP_LESS         = 2
    NV097_SET_ALPHA_FUNC_V_EQUAL,           // D3DCMP_EQUAL        = 3
    NV097_SET_ALPHA_FUNC_V_LEQUAL,          // D3DCMP_LESSEQUAL    = 4
    NV097_SET_ALPHA_FUNC_V_GREATER,         // D3DCMP_GREATER      = 5
    NV097_SET_ALPHA_FUNC_V_NOTEQUAL,        // D3DCMP_NOTEQUAL     = 6
    NV097_SET_ALPHA_FUNC_V_GEQUAL,          // D3DCMP_GREATEREQUAL = 7
    NV097_SET_ALPHA_FUNC_V_ALWAYS           // D3DCMP_ALWAYS       = 8
};

const DWORD nv056DepthFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV056_SET_DEPTH_FUNC_V_NEVER,           // D3DCMP_NEVER        = 1
    NV056_SET_DEPTH_FUNC_V_LESS,            // D3DCMP_LESS         = 2
    NV056_SET_DEPTH_FUNC_V_EQUAL,           // D3DCMP_EQUAL        = 3
    NV056_SET_DEPTH_FUNC_V_LEQUAL,          // D3DCMP_LESSEQUAL    = 4
    NV056_SET_DEPTH_FUNC_V_GREATER,         // D3DCMP_GREATER      = 5
    NV056_SET_DEPTH_FUNC_V_NOTEQUAL,        // D3DCMP_NOTEQUAL     = 6
    NV056_SET_DEPTH_FUNC_V_GEQUAL,          // D3DCMP_GREATEREQUAL = 7
    NV056_SET_DEPTH_FUNC_V_ALWAYS           // D3DCMP_ALWAYS       = 8
};

const DWORD nv097DepthFunc[D3D_CMP_FUNC_COUNT] =
{
    ~0,                                     // undefined
    NV097_SET_DEPTH_FUNC_V_NEVER,           // D3DCMP_NEVER        = 1
    NV097_SET_DEPTH_FUNC_V_LESS,            // D3DCMP_LESS         = 2
    NV097_SET_DEPTH_FUNC_V_EQUAL,           // D3DCMP_EQUAL        = 3
    NV097_SET_DEPTH_FUNC_V_LEQUAL,          // D3DCMP_LESSEQUAL    = 4
    NV097_SET_DEPTH_FUNC_V_GREATER,         // D3DCMP_GREATER      = 5
    NV097_SET_DEPTH_FUNC_V_NOTEQUAL,        // D3DCMP_NOTEQUAL     = 6
    NV097_SET_DEPTH_FUNC_V_GEQUAL,          // D3DCMP_GREATEREQUAL = 7
    NV097_SET_DEPTH_FUNC_V_ALWAYS           // D3DCMP_ALWAYS       = 8
};

// we set these based on NV056 SFACTORs, but NV056 DFACTORs are the same
const DWORD nv056BlendFunc[D3D_BLEND_COUNT] =
{
    ~0,                                                  // undefined
    NV056_SET_BLEND_FUNC_SFACTOR_V_ZERO,                 // D3DBLEND_ZERO            = 1
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE,                  // D3DBLEND_ONE             = 2
    NV056_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR,            // D3DBLEND_SRCCOLOR        = 3
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR,  // D3DBLEND_INVSRCCOLOR     = 4
    NV056_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA,            // D3DBLEND_SRCALPHA        = 5
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA,  // D3DBLEND_INVSRCALPHA     = 6
    NV056_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA,            // D3DBLEND_DESTALPHA       = 7
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA,  // D3DBLEND_INVDESTALPHA    = 8
    NV056_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR,            // D3DBLEND_DESTCOLOR       = 9
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR,  // D3DBLEND_INVDESTCOLOR    = 10
    NV056_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE,   // D3DBLEND_SRCALPHASAT     = 11
    NV056_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA,            // D3DBLEND_BOTHSRCALPHA    = 12
    NV056_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA   // D3DBLEND_BOTHINVSRCALPHA = 13
};

// we set these based on NV097 SFACTORs, but NV097 DFACTORs are the same
const DWORD nv097BlendFunc[D3D_BLEND_COUNT] =
{
    ~0,                                                  // undefined
    NV097_SET_BLEND_FUNC_SFACTOR_V_ZERO,                 // D3DBLEND_ZERO            = 1
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE,                  // D3DBLEND_ONE             = 2
    NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR,            // D3DBLEND_SRCCOLOR        = 3
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR,  // D3DBLEND_INVSRCCOLOR     = 4
    NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA,            // D3DBLEND_SRCALPHA        = 5
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA,  // D3DBLEND_INVSRCALPHA     = 6
    NV097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA,            // D3DBLEND_DESTALPHA       = 7
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA,  // D3DBLEND_INVDESTALPHA    = 8
    NV097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR,            // D3DBLEND_DESTCOLOR       = 9
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR,  // D3DBLEND_INVDESTCOLOR    = 10
    NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE,   // D3DBLEND_SRCALPHASAT     = 11
    NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA,            // D3DBLEND_BOTHSRCALPHA    = 12
    NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA   // D3DBLEND_BOTHINVSRCALPHA = 13
};

// New DX8 blend operations
const DWORD nv056BlendOp[D3D_BLEND_OP_COUNT] = {
    ~0,
    NV056_SET_BLEND_EQUATION_V_FUNC_ADD,                     // D3DBLENDOP_ADD
    NV056_SET_BLEND_EQUATION_V_FUNC_SUBTRACT,                // D3DBLENDOP_SUBTRACT
    NV056_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT,        // D3DBLENDOP_REVSUBTRACT
    NV056_SET_BLEND_EQUATION_V_MIN,                          // D3DBLENDOP_MIN
    NV056_SET_BLEND_EQUATION_V_MAX                           // D3DBLENDOP_MAX
};

const DWORD nv097BlendOp[D3D_BLEND_OP_COUNT] = {
    ~0,
    NV097_SET_BLEND_EQUATION_V_FUNC_ADD,                     // D3DBLENDOP_ADD
    NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT,                // D3DBLENDOP_SUBTRACT
    NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT,        // D3DBLENDOP_REVSUBTRACT
    NV097_SET_BLEND_EQUATION_V_MIN,                          // D3DBLENDOP_MIN
    NV097_SET_BLEND_EQUATION_V_MAX                           // D3DBLENDOP_MAX
};

// these are defined in terms of TEXTUREADDRESSU, but values for TEXTUREADDRESSV are the same
const DWORD nv056TextureAddress[D3D_TADDRESS_COUNT] =
{
    ~0,                                                  // undefined
    NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_WRAP,       // D3DTADDRESS_WRAP     = 1
    NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_MIRROR,     // D3DTADDRESS_MIRROR   = 2
    NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_CLAMP,      // D3DTADDRESS_CLAMP    = 3
    NV056_SET_TEXTURE_FORMAT_TEXTUREADDRESSU_WRAP        // D3DTADDRESS_BORDER   = 4
};

#if (NVARCH >= 0x020)
// these are defined in terms of TEXTUREADDRESSU, but values for TEXTUREADDRESSV are the same
const DWORD nv097TextureAddress[D3D_TADDRESS_COUNT] =
{
    ~0,                                                  // undefined
    NV097_SET_TEXTURE_ADDRESS_U_WRAP,                    // D3DTADDRESS_WRAP     = 1
    NV097_SET_TEXTURE_ADDRESS_U_MIRROR,                  // D3DTADDRESS_MIRROR   = 2
    NV097_SET_TEXTURE_ADDRESS_U_CLAMP_TO_EDGE,           // D3DTADDRESS_CLAMP    = 3
    NV097_SET_TEXTURE_ADDRESS_U_BORDER                   // D3DTADDRESS_BORDER   = 4
};
#endif

const DWORD nv056TextureMagFilter[D3D_MAGFILTER_COUNT] =
{
    ~0,                                                  // undefined
    NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST,         // D3DTFG_POINT         = 1
    NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR,          // D3DTFG_LINEAR        = 2
    NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST,         // D3DTFG_FLATCUBIC     = 3
    NV056_SET_TEXTURE_FILTER_TEXTUREMAG_NEAREST,         // D3DTFG_GAUSSIANCUBIC = 4
    NV056_SET_TEXTURE_FILTER_TEXTUREMAG_LINEAR,          // D3DTFG_ANISOTROPIC   = 5  (Mag aniso is bilinear in HW, and this passes WHQL.)
};

#if (NVARCH >= 0x020)
const DWORD nv097TextureMagFilter[D3D_MAGFILTER_COUNT] =
{
    ~0,                                                  // undefined
    NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0,               // D3DTFG_POINT         = 1
    NV097_SET_TEXTURE_FILTER_MAG_TENT_LOD0,              // D3DTFG_LINEAR        = 2
    NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0,               // D3DTFG_FLATCUBIC     = 3
    NV097_SET_TEXTURE_FILTER_MAG_BOX_LOD0,               // D3DTFG_GAUSSIANCUBIC = 4
    ~0                                                   // D3DTFG_ANISOTROPIC   = 5  (invalid. we have to deal with this separately)
};
#endif

// notes on these values:
// in openGL, foo_mipmap_bar means:
// - use the 'foo' filter within mipmaps
// - use the 'bar' filter between mipmaps
// stupid DX5 (and hence the HW definitions) were/are reversed

const DWORD nv056TextureMinFilter[D3D_MINFILTER_COUNT][D3D_MIPFILTER_COUNT] =
{
    // minfilter = 0 (undefined)
    ~0,                                                   // undefined
    ~0,                                                   // D3DTFP_NONE    = 1
    ~0,                                                   // D3DTFP_POINT   = 2
    ~0,                                                   // D3DTFP_LINEAR  = 3

    // D3DTFN_POINT       = 1
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR,           // undefined (use default? HMH)
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_NEAREST,          // D3DTFP_NONE    = 1
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_MIPNEAREST,       // D3DTFP_POINT   = 2
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEARMIPNEAREST, // D3DTFP_LINEAR  = 3

    // D3DTFN_LINEAR      = 2
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR,           // undefined (use default)
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEAR,           // D3DTFP_NONE    = 1
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_MIPLINEAR,        // D3DTFP_POINT   = 2
    NV056_SET_TEXTURE_FILTER_TEXTUREMIN_LINEARMIPLINEAR,  // D3DTFP_LINEAR  = 3

    // D3DTFN_ANISOTROPIC = 3 (invalid. we have to deal with this separately)
    ~0,                                                   // undefined
    ~0,                                                   // D3DTFP_NONE    = 1
    ~0,                                                   // D3DTFP_POINT   = 2
    ~0,                                                   // D3DTFP_LINEAR  = 3
};

#if (NVARCH >= 0x020)
const DWORD nv097TextureMinFilter[D3D_MINFILTER_COUNT][D3D_MIPFILTER_COUNT] =
{
    // minfilter = 0 (undefined)
    ~0,                                                   // undefined
    ~0,                                                   // D3DTFP_NONE    = 1
    ~0,                                                   // D3DTFP_POINT   = 2
    ~0,                                                   // D3DTFP_LINEAR  = 3

    // D3DTFN_POINT       = 1
    ~0,                                                   // undefined
    NV097_SET_TEXTURE_FILTER_MIN_BOX_LOD0,                // D3DTFP_NONE    = 1
    NV097_SET_TEXTURE_FILTER_MIN_BOX_NEARESTLOD,          // D3DTFP_POINT   = 2
    NV097_SET_TEXTURE_FILTER_MIN_BOX_TENT_LOD,            // D3DTFP_LINEAR  = 3

    // D3DTFN_LINEAR      = 2
    NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0,               // undefined (use default)
    NV097_SET_TEXTURE_FILTER_MIN_TENT_LOD0,               // D3DTFP_NONE    = 1
    NV097_SET_TEXTURE_FILTER_MIN_TENT_NEARESTLOD,         // D3DTFP_POINT   = 2
    NV097_SET_TEXTURE_FILTER_MIN_TENT_TENT_LOD,           // D3DTFP_LINEAR  = 3

    // D3DTFN_ANISOTROPIC = 3 (invalid. we have to deal with this separately)
    ~0,                                                   // undefined
    ~0,                                                   // D3DTFP_NONE    = 1
    ~0,                                                   // D3DTFP_POINT   = 2
    ~0,                                                   // D3DTFP_LINEAR  = 3
};

#endif  // NVARCH >= 0x020

const DWORD dx8MinFilterMapping[] =
{
    ~0,                   // D3DTEXF_NONE
    D3DTFN_POINT,         // D3DTEXF_POINT
    D3DTFN_LINEAR,        // D3DTEXF_LINEAR
    D3DTFN_ANISOTROPIC,   // D3DTEXF_ANISOTROPIC
    ~0,                   // D3DTEXF_FLATCUBIC
    ~0                    // D3DTEXF_GAUSSIANCUBIC
};

const DWORD dx8MagFilterMapping[] =
{
    ~0,                   // D3DTEXF_NONE
    D3DTFG_POINT,         // D3DTEXF_POINT
    D3DTFG_LINEAR,        // D3DTEXF_LINEAR
    D3DTFG_ANISOTROPIC,   // D3DTEXF_ANISOTROPIC
    D3DTFG_FLATCUBIC,     // D3DTEXF_FLATCUBIC
    D3DTFG_GAUSSIANCUBIC  // D3DTEXF_GAUSSIANCUBIC
};

const DWORD dx8MipFilterMapping[] =
{
    D3DTFP_NONE,          // D3DTEXF_NONE
    D3DTFP_POINT,         // D3DTEXF_POINT
    D3DTFP_LINEAR,        // D3DTEXF_LINEAR
    ~0,                   // D3DTEXF_ANISOTROPIC
    ~0,                   // D3DTEXF_FLATCUBIC
    ~0                    // D3DTEXF_GAUSSIANCUBIC
};

//---------------------------------------------------------------------------

// mappings between vertex attributes, streams, and HW registers

// map positions of D3D vertex attributes to indices in the driver's
// vertex attribute array and the HW's fixed-pipe input registers

const DWORD defaultInputRegMap[D3D_INPUTREGMAP_COUNT] = {
       // Microsoft                             -> NV fixed pipe mapping
    0, // D3DVSDE_POSITION     0  Position         Param[0]   Position            X,Y,Z,W
    1, // D3DVSDE_BLENDWEIGHT  1  BlendWeight      Param[1]   Skin Weights        W,W,W,W
    5, // D3DVSDE_BLENDINDICES 2  BlendIndices     Param[5]   Fog(OGL)            F,*,*,*
    2, // D3DVSDE_NORMAL       3  Normal           Param[2]   Normal              X,Y,Z,*
    6, // D3DVSDE_PSIZE        4  PointSize        Param[6]   Point Size          P,*,*,*
    3, // D3DVSDE_DIFFUSE      5  Diffuse          Param[3]   Diffuse Color       R,G,B,A
    4, // D3DVSDE_SPECULAR     6  Specular         Param[4]   Specular Color      R,G,B,A
    9, // D3DVSDE_TEXCOORD0    7  Texcoord0        Param[9]   Texture0            S,T,R,Q
   10, // D3DVSDE_TEXCOORD1    8  Texcoord1        Param[10]  Texture1            S,T,R,Q
   11, // D3DVSDE_TEXCOORD2    9  Texcoord2        Param[11]  Texture2            S,T,R,Q
   12, // D3DVSDE_TEXCOORD3    10 Texcoord3        Param[12]  Texture3            S,T,R,Q
   13, // D3DVSDE_TEXCOORD4    11 Texcoord4        Param[13]  Texture4            S,T,R,Q
   14, // D3DVSDE_TEXCOORD5    12 Texcoord5        Param[14]  Texture5            S,T,R,Q
   15, // D3DVSDE_TEXCOORD6    13 Texcoord6        Param[15]  Texture6            S,T,R,Q
    7, // D3DVSDE_TEXCOORD7    14 Texcoord7        Param[7]   Texture7            S,T,R,Q
    8, // D3DVSDE_POSITION2    15 n/a              Param[8]   unused              *,*,*,*
    8, // D3DVSDE_NORMAL2      16 n/a              Param[8]   unused              *,*,*,*
};

// map output of MS shader program to HW's vertex shader output registers

const DWORD defaultOutputRegMap[D3D_OUTPUTREGMAP_COUNT] = {
    0, // 0  vertex
    3, // 1  diffuse
    4, // 2  specular
    7, // 3  backfacing diffuse - unused
    8, // 4  backfacing specular - unused
    5, // 5  fog
    6, // 6  point size
    9, // 7  texture 0
   10, // 8  texture 1
   11, // 9  texture 2
   12, // 10 texture 3
   13, // 11 texture 4 (undefined in NV20)
   14, // 12 texture 5 (undefined in NV20)
   15, // 13 texture 6 (undefined in NV20)
    1, // 14 undefined in NV20
    2, // 15 undefined in NV20
};

#endif  // NVARCH >= 0x010

#endif  // NVARCH >= 0x04
