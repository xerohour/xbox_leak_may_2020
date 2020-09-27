/*
 *   Copyright (c) 1997-8  S3 Inc.  All Rights Reserved.
 *
 *   Module Name:  s3tc.h
 *
 *   Purpose:  Constant, structure, and prototype definitions for S3TC
 *           texture map compression.
 *
 *   Author:  Konstantine Iourcha, Dan McCabe
 *
 *   Revision History:
 *       lthompso 98Jan05 Code beautification.
 *
 */

#ifndef    _S3TC_H_
#define    _S3TC_H_


// number of pixels in block
#define S3TC_BLOCK_PIXELS    16

typedef    struct    {
    BYTE    rgba[4];
} S3TC_COLOR;

typedef    WORD        RGB565;        // packed color
typedef    DWORD        PIXBM;        // 2 BPP bitmap


typedef struct    {
    RGB565        rgb0;        // color for index 0
    RGB565        rgb1;        // color for index 1
    PIXBM        pixbm;        // pixel bitmap
} S3TCBlockRGB;

typedef    struct    {
    WORD        alphabm[4];    // alpha bitmap at 4 BPP
    S3TCBlockRGB    rgb;        // color block
} S3TCBlockAlpha4;

typedef    struct    {
    BYTE        alpha0;        // alpha for index 0
    BYTE        alpha1;        // alpha for index 1
    BYTE        alphabm[6];    // alpha bitmap at 3 BPP
    S3TCBlockRGB    rgb;        // color block
} S3TCBlockAlpha3;


// prototypes for encoders and decoders
void EncodeBlockRGB(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockRGB *pblockDst);
void DecodeBlockRGB(S3TCBlockRGB *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS]);

void EncodeBlockRGBColorKey(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockRGB *pblockDst, S3TC_COLOR colorLo, S3TC_COLOR colorHi);

void EncodeBlockAlpha4(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockAlpha4 *pblockDst);
void DecodeBlockAlpha4(S3TCBlockAlpha4 *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS]);

void EncodeBlockAlpha3(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockAlpha3 *pblockDst);
void DecodeBlockAlpha3(S3TCBlockAlpha3 *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS]);


#endif    _S3TC_H_
