///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       s3tchelp.cpp
//  Content:    S3 texture compression helpper functions
//
///////////////////////////////////////////////////////////////////////////

#include "pchtex.h"

//-----------------------------------------------------------------------------
// This band-aid is here temporarily (we hope) until S3 fixes their
// EncodeBlockRGB function to stop ignoring the transparency/opacity
// info in the alpha components of the 4x4 block of source pixels.
// For now, if any pixels are transparent, we have to define a color-
// key value and set the transparent pixels to the color-key value.
// Because this encoding format can store only a single bit of alpha
// for each pixel, we just look at the MSB of the pixel's 8-bit alpha.
// Hence, alpha values in the range 0 to 127 are transparent.

void 
XXEncodeBlockRGB(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockRGB *blockdst)
{
    int i;
    DWORD colorKey;
    UINT32 *pargb = (UINT32 *)colorSrc;

    // Determine if any of the pixels in the 4x4 block are transparent.
    // Transparency is determined by the MSB of the 8-bit alpha value,
    // which is the sign bit if we treat the 32-bit pixel as a "long."
    for (i = 0; i < 16; ++i)
    {
        if (*(long *)&pargb[i] >= 0)
        {
            // Yes, this pixel is transparent.
            break;
        }
    }

    // Are any of the pixels transparent?
    if (i == 16)
    {
        // No, none of the 16 pixels is transparent.
        EncodeBlockRGB(colorSrc, blockdst);
        return;
    }

    // One or more pixels is transparent.  We need to find a color-key
    // value that does not match any of the opaque pixels in the block.
    for (colorKey = 0; ; ++colorKey)
    {
        for (i = 0; i < 16; ++i)
        {
            if (*(long *)&pargb[i] >= 0)
            {
                // This pixel is not opaque, so skip to next pixel.
                continue;
            }
            if ((pargb[i] & 0x00ffffff) == colorKey)
            {
                // This is not a good color-key value because it matches
                // one of the opaque pixels in the block.  Try again.
                break;
            }
        }
        // Did we find a good color-key value?
        if (i == 16)
        {
            // Yes, we've got a good color-key value.
            break;    
        }
    }

    // Replace any transparent pixels with the color-key value.
    for (i = 0; i < 16; ++i)
    {
        // Is this pixel transparent?
        if (*(long *)&pargb[i] >= 0)
        {
            // Yes, replace transparent pixel with color-key value.
            pargb[i] = colorKey;
        }
    }
    EncodeBlockRGBColorKey(colorSrc, blockdst,
                *(S3TC_COLOR *)&colorKey, *(S3TC_COLOR *)&colorKey);
}  


//-----------------------------------------------------------------------------
// This band-aid is here temporarily (we hope) until S3 fixes their
// DecodeBlockRGB function to zero the color components of any pixels
// whose alpha components are zero.

void 
XXDecodeBlockRGB(S3TCBlockRGB *blocksrc, S3TC_COLOR colordst[S3TC_BLOCK_PIXELS])
{
    int i;
    UINT32 *pargb = (UINT32 *)colordst;

    DecodeBlockRGB(blocksrc, colordst);
    for (i = 0; i < 16; ++i)
    {
        if ((pargb[i] >> 24) == 0)
        {
            pargb[i] = 0;   // zero RGB components
        }
    }
}   

