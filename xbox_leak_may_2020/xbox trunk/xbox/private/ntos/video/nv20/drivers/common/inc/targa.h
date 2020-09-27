// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: targa.h
//      header files for targa.c
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        12Jun00         filched from architects
//
// **************************************************************************

#ifndef __TARGA_H
#define __TARGA_H

typedef char int8;
typedef int int16;
typedef int int32;
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned int uint32;

/* Header definition. */
typedef struct TGA_Header_  {
    unsigned char ImageIDLength;        /* length of Identifier String. */
    unsigned char CoMapType;            /* 0 = no map */
    unsigned char ImgType;              /* image type (see below for values) */
    unsigned char Index_lo, Index_hi;   /* index of first color map entry */
    unsigned char Length_lo, Length_hi; /* number of entries in color map */
    unsigned char CoSize;               /* size of color map entry (15,16,24,32) */
    unsigned char X_org_lo, X_org_hi;   /* x origin of image */
    unsigned char Y_org_lo, Y_org_hi;   /* y origin of image */
    unsigned char Width_lo, Width_hi;   /* width of image */
    unsigned char Height_lo, Height_hi; /* height of image */
    unsigned char PixelSize;            /* pixel size (8,16,24,32) */
    unsigned char Desc;         /* 4 bits, number of attribute bits per pixel */
} TGA_Header;

/* if we create the tga file, we will put the following in the ImageID field */
typedef struct TGA_ImageId_ {
        unsigned char Magic;            /* must be a specific magic value */
        unsigned char ImageFmt;         /* encodes special raster formats */
} TGA_ImageId;

/* Definitions for image types. */
#define TGA_NULL 0
#define TGA_MAP 1
#define TGA_RGB 2
#define TGA_MONO 3
#define TGA_RLEMAP 9
#define TGA_RLERGB 10
#define TGA_RLEMONO 11

#define TGA_DESC_ALPHA_MASK     ((unsigned char)0xF)    /* number of alpha channel bits */
#define TGA_DESC_ORG_MASK       ((unsigned char)0x30)   /* origin mask */
#define TGA_ORG_BOTTOM_LEFT     0x00
#define TGA_ORG_BOTTOM_RIGHT    0x10
#define TGA_ORG_TOP_LEFT        0x20
#define TGA_ORG_TOP_RIGHT       0x30har)0x30)   /* origin mask */
#define TGA_ORG_BOTTOM_LEFT     0x00
#define TGA_ORG_BOTTOM_RIGHT    0x10

#define TGA_NVIDIA_MAGIC        0xAF            /* not an ascii char so unlikely... */

typedef enum RasterFormat_ {
/* WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
 * **** The RasterFormat defs _must_ be identical to the defs in vmodels/rasterDefs.vh ***
 * **** Do not change this enum without also updating rasterDefs.vh!                   ***
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING */
        RASTER_A1R5G5B5 = 0,
        RASTER_X1R5G5B5 = 1,
        RASTER_A4R4G4B4 = 2,
        RASTER_R5G6B5 = 3,
        RASTER_A8R8G8B8 = 4,
        RASTER_X8R8G8B8 = 5,
        RASTER_Y8 = 6,
        RASTER_V8YB8U8YA8 = 7,
        RASTER_YB8V8YA8U8 = 8,
        RASTER_A8V8U8Y8 = 9,                    // A8Y8U8V8 was incorrect channel ordering, name changed
        RASTER_A4V6YB6A4U6YA6 = 10,
        RASTER_AY8 = 11,
        RASTER_Z16 = 12,                                // 16 bit Z/W buffer
        RASTER_Z24S8 = 13,                              // 24 bit Z/W plus 8 bit stencil in low 8 bits
// additional non-Nvidia formats for ease of use with tga
        RASTER_VOID32 = 100,                    // generic 32 bits/pixel format
        RASTER_R8G8B8 = 101,                    // 24 bits/pixel
        RASTER_VOID16 = 102,                    // generic 16 bits/pixel format
        RASTER_FORCELONG = 0x7FFFFFFFL  // force this enum to be a long
} RasterFormat;

// Read targa file "filename"
// Put pixels into **xbgr, and set *width and *height
// Pixels are xbgr words on the PC
// This routine copes with raw RGB and run-length encoded RGB targa files only

int read_targa_file(char *filename, unsigned char **xrgb, int *width, int *height);

#endif  // __TARGA_H

