// !!!!!!!!! DO NOT RELEASE THIS SOURCE FILE !!!!!!!

/*
 *   Copyright (c) 1997-8  S3 Inc.  All Rights Reserved.
 *
 *   Module Name:  s3_intrf.h
 *
 *   Purpose:  Constant, structure, and prototype definitions for S3TC
 *             interface to DX surface
 *
 *   Author:  Dan Hung, Martin Hoffesommer
 *
 *   Revision History:
 *      04.03.01 - Cleaned up a bit and added to XGraphics.lib (JHarding)
 *  version Beta 1.00.00-98-03-26
 */

// Highlevel interface

#ifndef _S3_INTRF_H_
#define _S3_INTRF_H_

#if XBOX
    #include "xgrphseg.h"
    #include "xtl.h"
#else
    #include "windows.h"
    #include "d3d8-xbox.h"
#endif

#include "xgraphics.h"

// RGB encoding types
#define S3TC_ENCODE_RGB_FULL            0x0
#define S3TC_ENCODE_RGB_COLOR_KEY       0x1
#define S3TC_ENCODE_RGB_ALPHA_COMPARE   0x2
#define _S3TC_ENCODE_RGB_MASK           0xff

// alpha encoding types
#define S3TC_ENCODE_ALPHA_NONE          0x000
#define S3TC_ENCODE_ALPHA_EXPLICIT      0x100
#define S3TC_ENCODE_ALPHA_INTERPOLATED  0x200
#define _S3TC_ENCODE_ALPHA_MASK         0xff00

// additional alpha encoding flags
#define S3TC_ENCODE_ALPHA_NEED0             0x10000
#define S3TC_ENCODE_ALPHA_NEED1             0x20000
#define S3TC_ENCODE_ALPHA_PROTECTNONZERO    0x40000

// common encoding types
//@@@TBD

#if 0
// determine number of bytes needed to compress given source image
unsigned S3TCgetEncodeSize(LPDIRECT3DTEXTURE8 lpDesc,   // [in]
                           unsigned dwEncodeType    // [in]
                           );
// encode (compress) given source image to given destination surface
void S3TCencode(void *lpSrcBufIn, DWORD log2Width, DWORD log2Height,
                void *lpDestBufIn, DWORD dwLevel, float *weight);

// determine number of bytes needed do decompress given compressed image
unsigned S3TCgetDecodeSize(LPDIRECT3DTEXTURE8 lpDesc);

// decode (decompress) to ARGB8888
void S3TCdecode(LPDIRECT3DTEXTURE8 lpSrc,       // [in]
                LPDIRECT3DTEXTURE8 lpDest,      // [out]
                void *lpDestBuf             // [in]
                );
#endif // 0

#endif
