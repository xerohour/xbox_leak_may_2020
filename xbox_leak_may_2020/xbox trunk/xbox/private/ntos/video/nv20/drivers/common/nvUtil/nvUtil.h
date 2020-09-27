/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVUTIL.H                                                          *
*   interface to NV utility functions common to most 3D drivers             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 02/15/99 - wrote it                     *
*                                                                           *
\***************************************************************************/
#ifndef _nvutil_h
#define _nvutil_h

/*
 * constants
 */
#define NV_MEMCOPY_DONTCARE         0   // alignment is not an issue
#define NV_MEMCOPY_WANTSRCALIGNED   1   // source must be aligned
#define NV_MEMCOPY_WANTDESTALIGNED  2   // dest must be aligned

#define NV_SWIZFLAG_SRCSWIZZLED     1   // source is swizzled
#define NV_SWIZFLAG_DESTSWIZZLED    2   // dest is swizzled

#define NV_FS_MMX                   0x00000001      // CPU can do MMX
#define NV_FS_KATMAI                0x00000002      // CPU is a P3 (or later)
#define NV_FS_64MB                  0x10000000      // has at least 64Mb
#define NV_FS_128MB                 0x20000000      // has at least 128Mb
#define NV_FS_256MB                 0x40000000      // has at least 256Mb

/*
 * types
 */
#ifndef _WINDOWS_
#error windows.h must be included before including nvutil.h
#endif

/*
 * macros
 */

/*
 * structs
 */
typedef struct
{
    DWORD dwCPUFeatureSet;
    DWORD dwCPUClockSpeed;
} NVCPUINFO;

/*
 * exports
 */
#ifdef __cplusplus
extern "C" 
{
#endif

/*
 * creation and destruction
 */
void nvUtilCreate  (void);
void nvUtilDestroy (void);

/*
 * CPU caps
 */
void nvGetCPUCaps (NVCPUINFO*);

/*
 * mem copy
 */
void nvMemCopy
(
    DWORD dwDest,                   // dest address
    DWORD dwSrc,                    // source address
    DWORD dwCount,                  // # of bytes to copy
    DWORD dwFlags                   // alignment flags (NV_MEMCOPY_xxx)
);

/*
 * swizzling
 */
BOOL nvSwizzleBlt
(
    DWORD         dwSrcAddr,        // base address
    DWORD         dwSrcLogW,        // log2 of texture width
    DWORD         dwSrcLogH,        // log2 of texture height
    DWORD         dwSrcX0,          // sub rectangle to swizzlw
    DWORD         dwSrcY0,
    DWORD         dwSrcX1,
    DWORD         dwSrcY1,
    DWORD         dwSrcPitch,       // pitch

    DWORD         dwDestAddr,
    DWORD         dwDestLogW,
    DWORD         dwDestLogH,
    DWORD         dwDestX,
    DWORD         dwDestY,
    DWORD         dwDestPitch,

    DWORD         dwBPP,            // bytes pre pixel (not bits per pixel!)
    DWORD         dwFlags           // swizzle flags (NV_SWIZFLAG_xxx)
);

#ifdef __cplusplus
}
#endif

#endif //!_nvutil_h