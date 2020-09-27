/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	idct.cpp

Abstract:

	Inverse DCT routines

Author:

	Marc Holder (marchold@microsoft.com)    05/98
    Bruce Lin (blin@microsoft.com)          06/98
    Ming-Chieh Lee (mingcl@microsoft.com)   06/98

Revision History:

*************************************************************************/
#include "bldsetup.h"

#include "xplatform.h"
#include "typedef.hpp"
#include "idctdec_wmv2.h"
#include "wmvdec_api.h"

#ifndef __DCT_COEF_INT_
#define __DCT_COEF_INT_

#define W1 2841L /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676L /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408L /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609L /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108L /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565L  /* 2048*sqrt(2)*cos(7*pi/16) */

#define W1a 1892L /* 2048*cos(1*pi/8) */
#define W2a 1448L /* 2048*cos(2*pi/8) */
#define W3a 784L  /* 2048*cos(3*pi/8) */

#define W1_W7 2276L //(W1 - W7)
#define W1pW7 3406L //(W1 + W7)
#define W3_W5 799L  //(W3 - W5)
#define W3pW5 4017L //(W3 + W5)
#define W2_W6 1568L //(W2 - W6)
#define W2pW6 3784L //(W2 + W6)

#endif // __DCT_COEF_INT_

#define SATURATE8(x)   (x < 0 ? 0 : (x > 255 ? 255 : x))

#define _USE_C_IDCT_


///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////

#ifndef OPT_IDCT_ARM
// C Version
// Input: 32-bit
// Output: 8-bit
Void_WMV g_IDCTDec_WMV2 (U8_WMV* piDst, I32_WMV iOffsetToNextRowForDCT, const PixelI32 __huge* rgiCoefRecon)
{
    I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    PixelI32 rgblk [64];
    PixelI32* blk = rgblk;
    I32_WMV iTest;

    I32_WMV  i;
	for (i = 0; i < BLOCK_SIZE; i++, blk += BLOCK_SIZE, rgiCoefRecon += BLOCK_SIZE) {
		x4 = rgiCoefRecon [1];
		x3 = rgiCoefRecon [2];
		x7 = rgiCoefRecon [3];
		x1 = (I32_WMV)rgiCoefRecon [4] << 11 ;
		x6 = rgiCoefRecon [5];
		x2 = rgiCoefRecon [6];
		x5 = rgiCoefRecon [7];
		x0 = ((I32_WMV)rgiCoefRecon [0] << 11) + 128L; /* for proper rounding in the fourth stage */
		/* first stage */
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1 - W7) * x4;
		x5 = x8 - (W1 + W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3 - W5) * x6;
		x7 = x8 - (W3 + W5) * x7;

		/* second stage */
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2 + W6) * x2;
		x3 = x1 + (W2 - W6) * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;

		/* fourth stage */
		blk [0] = (PixelI32) ((x7 + x1) >> 8);
		blk [1] = (PixelI32) ((x3 + x2) >> 8);
		blk [2] = (PixelI32) ((x0 + x4) >> 8);
		blk [3] = (PixelI32) ((x8 + x6) >> 8);
		blk [4] = (PixelI32) ((x8 - x6) >> 8);
		blk [5] = (PixelI32) ((x0 - x4) >> 8);
		blk [6] = (PixelI32) ((x3 - x2) >> 8);
		blk [7] = (PixelI32) ((x7 - x1) >> 8);
	}

    {
        U8_WMV * blk0 = piDst;
        U8_WMV * blk1 = blk0 + iOffsetToNextRowForDCT;
        U8_WMV * blk2 = blk1 + iOffsetToNextRowForDCT;
        U8_WMV * blk3 = blk2 + iOffsetToNextRowForDCT;
        U8_WMV * blk4 = blk3 + iOffsetToNextRowForDCT;
        U8_WMV * blk5 = blk4 + iOffsetToNextRowForDCT;
        U8_WMV * blk6 = blk5 + iOffsetToNextRowForDCT;
        U8_WMV * blk7 = blk6 + iOffsetToNextRowForDCT;
        
        PixelI32* pBlk0 = rgblk;
        PixelI32* pBlk1 = pBlk0 + BLOCK_SIZE;
        PixelI32* pBlk2 = pBlk1 + BLOCK_SIZE;
        PixelI32* pBlk3 = pBlk2 + BLOCK_SIZE;
        PixelI32* pBlk4 = pBlk3 + BLOCK_SIZE;
        PixelI32* pBlk5 = pBlk4 + BLOCK_SIZE;
        PixelI32* pBlk6 = pBlk5 + BLOCK_SIZE;
        PixelI32* pBlk7 = pBlk6 + BLOCK_SIZE;
        
        for (i = 0; i < BLOCK_SIZE; i++){
            x0 = (I32_WMV) ((I32_WMV)*pBlk0++ << 8) + 8192L;
            x1 = (I32_WMV) *pBlk4++ << 8;
            x2 = *pBlk6++;
            x3 = *pBlk2++;
            x4 = *pBlk1++;
            x5 = *pBlk7++;
            x6 = *pBlk5++;
            x7 = *pBlk3++;
            
            /* first stage */
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + (W1 - W7) * x4) >> 3;
            x5 = (x8 - (W1 + W7) * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - (W3 - W5) * x6) >> 3;
            x7 = (x8 - (W3 + W5) * x7) >> 3;
            
            /* second stage */
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - (W2 + W6) * x2) >> 3;
            x3 = (x1 + (W2 - W6) * x3) >> 3;
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (181 * (x4 + x5) + 128) >> 8;
            x4 = (181 * (x4 - x5) + 128) >> 8;
            
            /* fourth stage */
            x5 = (x7 + x1) >> 14;
            x1 = (x7 - x1) >> 14;
            iTest = x1 | x5;
            x7 = (x3 + x2) >> 14;
            iTest |= x7;
            x2 = (x3 - x2) >> 14;
            iTest |= x2;
            x3 = (x0 + x4) >> 14;
            iTest |= x3;
            x4 = (x0 - x4) >> 14;
            iTest |= x4;
            x0 = (x8 + x6) >> 14;
            iTest |= x0;
            x6 = (x8 - x6) >> 14;
            iTest |= x6;
            
            if (iTest & ~255L)
            {
                x0 = SATURATE8(x0);
                x1 = SATURATE8(x1);
                x2 = SATURATE8(x2);
                x3 = SATURATE8(x3);
                x4 = SATURATE8(x4);
                x5 = SATURATE8(x5);
                x6 = SATURATE8(x6);
                x7 = SATURATE8(x7);
            }
            
            *blk0++ = (U8_WMV) x5;
            *blk1++ = (U8_WMV) x7;
            *blk2++ = (U8_WMV) x3;
            *blk3++ = (U8_WMV) x0;
            *blk4++ = (U8_WMV) x6;
            *blk5++ = (U8_WMV) x4;
            *blk6++ = (U8_WMV) x2;
            *blk7++ = (U8_WMV) x1;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Inter IDCT Functions
//
///////////////////////////////////////////////////////////////////////////

// C Version
// Input:  16-bit
// Output: 16-bit
Void_WMV g_IDCTDec16_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
#   if !defined(_SH4_)
	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#	endif

	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
    const PixelI __huge* rgiCoefRecon = piSrc->i16;
	PixelI __huge* blk = piDst->i16;
    register I32_WMV iDCTVertFlag = 0;

    I32_WMV  i;  
    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        if (!(iDCTHorzFlags & (1 << i))) 
        {
            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
            // If so, we don't have to do anything more for this row
            if (rgiCoefRecon [0] != 0)
			{
                // Column zero cofficient is nonzero so all coefficients in this row are DC
                blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
                // If this is not the first row, tell the second stage that it can't use DC
                // shortcut - it has to do the full transform for all the columns
                iDCTVertFlag = i;
			}
			continue;
		}

        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
		iDCTVertFlag = i;

		x4 = rgiCoefRecon [1];		
		x3 = rgiCoefRecon [2];
		x7 = rgiCoefRecon [3];
		x1 = (I32_WMV)rgiCoefRecon [4] << 11;
		x6 = rgiCoefRecon [5];
		x2 = rgiCoefRecon [6];		
		x5 = rgiCoefRecon [7];

        x0 = (I32_WMV)((I32_WMV)rgiCoefRecon [0] << 11) + 128L; /* for proper rounding in the fourth stage */

        // first stage
        x8 = W7 * (x4 + x5);
		x4 = x8 + W1_W7 * x4;
		x5 = x8 - W1pW7 * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - W3_W5 * x6;
		x7 = x8 - W3pW5 * x7;

		// second stage 
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - W2pW6 * x2;
		x3 = x1 + W2_W6 * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV)(181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV)(181L * (x4 - x5) + 128L) >> 8;

		/* fourth stage */
		blk [0] = (PixelI) ((x7 + x1) >> 8);
		blk [1] = (PixelI) ((x3 + x2) >> 8);
		blk [2] = (PixelI) ((x0 + x4) >> 8);
		blk [3] = (PixelI) ((x8 + x6) >> 8);
		blk [4] = (PixelI) ((x8 - x6) >> 8);
		blk [5] = (PixelI) ((x0 - x4) >> 8);
		blk [6] = (PixelI) ((x3 - x2) >> 8);
		blk [7] = (PixelI) ((x7 - x1) >> 8);
	}

    {
        
        PixelI __huge *blk0 = piDst->i16;
        
        PixelI __huge *blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI __huge *blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI __huge *blk3 = blk2 + iOffsetToNextRowForDCT;
        PixelI __huge *blk4 = blk3 + iOffsetToNextRowForDCT;
        PixelI __huge *blk5 = blk4 + iOffsetToNextRowForDCT;
        PixelI __huge *blk6 = blk5 + iOffsetToNextRowForDCT;
        PixelI __huge *blk7 = blk6 + iOffsetToNextRowForDCT;
        
        for (i = 0; i < BLOCK_SIZE; i++){
            // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
            // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
            // can use the DC shortcut and just copy blk[0] to the rest of the column.
            if (iDCTVertFlag == 0)
            {
                // If blk[0] is nonzero, copy the value to the other 7 rows in this column		
                if (blk0[i] != 0)
                    blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
                continue;
            }
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
            // first stage 
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + W1_W7 * x4) >> 3;
            x5 = (x8 - W1pW7 * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - W3_W5 * x6) >> 3;
            x7 = (x8 - W3pW5 * x7) >> 3;
            
            // second stage 
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - W2pW6 * x2) >> 3;
            x3 = (x1 + W2_W6 * x3) >> 3;
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
            x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;
            
            /* fourth stage */
            blk0[i] = (PixelI) ((x7 + x1) >> 14);
            blk1[i] = (PixelI) ((x3 + x2) >> 14);
            blk2[i] = (PixelI) ((x0 + x4) >> 14);
            blk3[i] = (PixelI) ((x8 + x6) >> 14);
            blk4[i] = (PixelI) ((x8 - x6) >> 14);
            blk5[i] = (PixelI) ((x0 - x4) >> 14);
            blk6[i] = (PixelI) ((x3 - x2) >> 14);
            blk7[i] = (PixelI) ((x7 - x1) >> 14); 
        }
    }
#   if !defined(_SH4_)
    FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

}
#endif //OPT_IDCT_ARM

#ifndef _EMB_WMV2_
#ifdef _USE_C_IDCT_
// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_IDCTDec_WMV2 (Buffer __huge* piDst, Buffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags)
{
#   if !defined(_SH4_)
	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#	endif

	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;

	const PixelI32 __huge* rgiCoefRecon = piSrc->i32;
	PixelI32 __huge* blk = piDst->i32;


    register I32_WMV iDCTVertFlag = 0;

    I32_WMV  i;  
    for (i = 0; i < BLOCK_SIZE; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE){
        // Check to see if this row has all zero coefficients in columns 1-7 (0 is first column)
        if (!(iDCTHorzFlags & (1 << i))) 
        {
            // Columns 1-7 must be zero. Check to see if column zero coefficient is zero. 
            // If so, we don't have to do anything more for this row
            if (rgiCoefRecon [0] != 0)
			{
                // Column zero cofficient is nonzero so all coefficients in this row are DC
                blk [0] = blk [1] = blk [2] = blk [3] = blk [4] = blk [5] = blk [6] = blk [7] = rgiCoefRecon [0] << 3;
                // If this is not the first row, tell the second stage that it can't use DC
                // shortcut - it has to do the full transform for all the columns
                iDCTVertFlag = i;
			}
			continue;
		}

        // If this is not the first row, tell the second stage that it can't use DC shortcut 
        // - it has to do the full transform for all the columns
		iDCTVertFlag = i;

		x4 = rgiCoefRecon [1];		
		x3 = rgiCoefRecon [2];
		x7 = rgiCoefRecon [3];
		x1 = (I32_WMV)rgiCoefRecon [4] << 11;
		x6 = rgiCoefRecon [5];
		x2 = rgiCoefRecon [6];		
		x5 = rgiCoefRecon [7];

        x0 = (I32_WMV)((I32_WMV)rgiCoefRecon [0] << 11) + 128L; /* for proper rounding in the fourth stage */

        // first stage
        x8 = W7 * (x4 + x5);
		x4 = x8 + W1_W7 * x4;
		x5 = x8 - W1pW7 * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - W3_W5 * x6;
		x7 = x8 - W3pW5 * x7;

		// second stage 
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - W2pW6 * x2;
		x3 = x1 + W2_W6 * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV)(181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV)(181L * (x4 - x5) + 128L) >> 8;

		/* fourth stage */
		blk [0] =  ((x7 + x1) >> 8);
		blk [1] =  ((x3 + x2) >> 8);
		blk [2] =  ((x0 + x4) >> 8);
		blk [3] =  ((x8 + x6) >> 8);
		blk [4] =  ((x8 - x6) >> 8);
		blk [5] =  ((x0 - x4) >> 8);
		blk [6] =  ((x3 - x2) >> 8);
		blk [7] =  ((x7 - x1) >> 8);
	}


    {
        PixelI32 __huge *blk0 = piDst->i32;
        PixelI32 __huge *blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk3 = blk2 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk4 = blk3 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk5 = blk4 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk6 = blk5 + iOffsetToNextRowForDCT;
        PixelI32 __huge *blk7 = blk6 + iOffsetToNextRowForDCT;
        
        
        
        for (i = 0; i < BLOCK_SIZE; i++){
            // If iDCTVertFlag is zero then we know that blk[1] thru blk[7] are zero.
            // Therefore, if blk[0] is zero we can skip to the next column. Otherwise we
            // can use the DC shortcut and just copy blk[0] to the rest of the column.
            if (iDCTVertFlag == 0)
            {
                // If blk[0] is nonzero, copy the value to the other 7 rows in this column		
                if (blk0[i] != 0)
                {
                    
                    blk0[i] = blk1[i] = blk2[i] = blk3[i] = blk4[i] = blk5[i] = blk6[i] = blk7[i] = (blk0[i] + 32) >> 6;
                }
                
                
                continue;
            }
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
            // first stage 
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + W1_W7 * x4) >> 3;
            x5 = (x8 - W1pW7 * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - W3_W5 * x6) >> 3;
            x7 = (x8 - W3pW5 * x7) >> 3;
            
            // second stage 
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - W2pW6 * x2) >> 3;
            x3 = (x1 + W2_W6 * x3) >> 3;
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
            x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;
            
            
            /* fourth stage */
            blk0[i] =  ((x7 + x1) >> 14);
            blk1[i] =  ((x3 + x2) >> 14);
            blk2[i] =  ((x0 + x4) >> 14);
            blk3[i] =  ((x8 + x6) >> 14);
            blk4[i] =  ((x8 - x6) >> 14);
            blk5[i] =  ((x0 - x4) >> 14);
            blk6[i] =  ((x3 - x2) >> 14);
            blk7[i] =  ((x7 - x1) >> 14); 
            
            
            
            
        }
    }
#   if !defined(_SH4_)
    FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

}
#endif
#endif //_EMB_WMV2_



#ifndef _EMB_WMV2_
// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_8x4IDCTDec_WMV2 (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{
#   if !defined(_SH4_)
	FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#	endif


    const PixelI32 __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
	PixelI32 __huge* piDst = piDstBuf->i32 + (iHalf*32);
	PixelI32 * blk = piDst;

	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

    for (i = 0; i < 4; i++, blk += iOffsetToNextRowForDCT, rgiCoefRecon += BLOCK_SIZE)
    {
        x4 = rgiCoefRecon [1];		
        x3 = rgiCoefRecon [2];
	    x7 = rgiCoefRecon [3];
	    x1 = (I32_WMV)rgiCoefRecon [4] << 11;
	    x6 = rgiCoefRecon [5];
	    x2 = rgiCoefRecon [6];		
	    x5 = rgiCoefRecon [7];

        *blk = rgiCoefRecon [0];

		x0 = (I32_WMV)((I32_WMV)blk [0] << 11) + 128L; /* for proper rounding in the fourth stage */
		/* first stage */
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1 - W7) * x4;
		x5 = x8 - (W1 + W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3 - W5) * x6;
		x7 = x8 - (W3 + W5) * x7;

		/* second stage */
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2 + W6) * x2;
		x3 = x1 + (W2 - W6) * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		/* third stage */
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (I32_WMV)(181L * (x4 + x5) + 128L) >> 8;
		x4 = (I32_WMV)(181L * (x4 - x5) + 128L) >> 8;

		/* fourth stage */
		blk [0] =  ((x7 + x1) >> 8);
		blk [1] =  ((x3 + x2) >> 8);
		blk [2] =  ((x0 + x4) >> 8);
		blk [3] =  ((x8 + x6) >> 8);
		blk [4] =  ((x8 - x6) >> 8);
		blk [5] =  ((x0 - x4) >> 8);
		blk [6] =  ((x3 - x2) >> 8);
		blk [7] =  ((x7 - x1) >> 8);
    }

    
    {
        PixelI32* blk0 = piDst;
        PixelI32* blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI32* blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI32* blk3 = blk2 + iOffsetToNextRowForDCT;
        
        
        
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            x4 = blk0[i];
            x5 = blk1[i];
            x6 = blk2[i];
            x7 = blk3[i];
            
            x0 = (x4 + x6)*W2a;
            x1 = (x4 - x6)*W2a;
            x2 = x5*W1a + x7*W3a;
            x3 = x5*W3a - x7*W1a;
            
            
            blk0[i] = (PixelI32)((x0 + x2 + 32768L)>>16);
            blk1[i] = (PixelI32)((x1 + x3 + 32768L)>>16);
            blk2[i] = (PixelI32)((x1 - x3 + 32768L)>>16);
            blk3[i] = (PixelI32)((x0 - x2 + 32768L)>>16);
            
        }
    }
#   if !defined(_SH4_)
    FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

}

// C Version
// Input:  32-bit
// Output: 32-bit
Void_WMV g_4x8IDCTDec_WMV2 (Buffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const Buffer __huge* rgiCoefReconBuf, I32_WMV iHalf)
{

#   if !defined(_SH4_)
   FUNCTION_PROFILE_DECL_START(fpDecode,IDCT_PROFILE);
#	endif

    const PixelI32 __huge* rgiCoefRecon = rgiCoefReconBuf->i32;
	PixelI32 __huge* piDst = piDstBuf->i32 + (iHalf*4);
	PixelI32 * blk = piDst;


	I32_WMV x0, x1, x2, x3, x4, x5, x6, x7, x8;
	I32_WMV i;

    for (i = 0; i < BLOCK_SIZE; i++,  rgiCoefRecon += 4){

        x4 = rgiCoefRecon[0];
        x5 = rgiCoefRecon[1];
        x6 = rgiCoefRecon[2];
        x7 = rgiCoefRecon[3];

        x0 = (x4 + x6)*W2a;
        x1 = (x4 - x6)*W2a;
        x2 = x5*W1a + x7*W3a;
        x3 = x5*W3a - x7*W1a;

        blk[0] = (PixelI32)((x0 + x2 + 64)>>7);
        blk[1] = (PixelI32)((x1 + x3 + 64)>>7);
        blk[2] = (PixelI32)((x1 - x3 + 64)>>7);
        blk[3] = (PixelI32)((x0 - x2 + 64)>>7);
        blk += iOffsetToNextRowForDCT;

    }
    
    {
        
        PixelI32* blk0 = piDst;
        PixelI32* blk1 = blk0 + iOffsetToNextRowForDCT;
        PixelI32* blk2 = blk1 + iOffsetToNextRowForDCT;
        PixelI32* blk3 = blk2 + iOffsetToNextRowForDCT;
        PixelI32* blk4 = blk3 + iOffsetToNextRowForDCT;
        PixelI32* blk5 = blk4 + iOffsetToNextRowForDCT;
        PixelI32* blk6 = blk5 + iOffsetToNextRowForDCT;
        PixelI32* blk7 = blk6 + iOffsetToNextRowForDCT;
        
        
        for (i = 0; i < 4; i++)
        {
            
            x0 = (I32_WMV)((I32_WMV)blk0[i] << 8) + 8192L;
            x1 = (I32_WMV)blk4[i] << 8;
            x2 = blk6[i];
            x3 = blk2[i];
            x4 = blk1[i];
            x5 = blk7[i];
            x6 = blk5[i];
            x7 = blk3[i];
            
            
            /* first stage */
            x8 = W7 * (x4 + x5) + 4;
            x4 = (x8 + (W1 - W7) * x4) >> 3;
            x5 = (x8 - (W1 + W7) * x5) >> 3;
            x8 = W3 * (x6 + x7) + 4;
            x6 = (x8 - (W3 - W5) * x6) >> 3;
            x7 = (x8 - (W3 + W5) * x7) >> 3;
            
            /* second stage */
            x8 = x0 + x1;
            x0 -= x1;
            x1 = W6 * (x3 + x2) + 4;
            x2 = (x1 - (W2 + W6) * x2) >> 3;
            x3 = (x1 + (W2 - W6) * x3) >> 3;
            x1 = x4 + x6;
            x4 -= x6;
            x6 = x5 + x7;
            x5 -= x7;
            
            /* third stage */
            x7 = x8 + x3;
            x8 -= x3;
            x3 = x0 + x2;
            x0 -= x2;
            x2 = (I32_WMV) (181L * (x4 + x5) + 128L) >> 8;
            x4 = (I32_WMV) (181L * (x4 - x5) + 128L) >> 8;
            
            /* fourth stage */
            
            blk0[i] = (PixelI32) ((x7 + x1) >> 14);
            blk1[i] = (PixelI32) ((x3 + x2) >> 14);
            blk2[i] = (PixelI32) ((x0 + x4) >> 14);
            blk3[i] = (PixelI32) ((x8 + x6) >> 14);
            blk4[i] = (PixelI32) ((x8 - x6) >> 14);
            blk5[i] = (PixelI32) ((x0 - x4) >> 14);
            blk6[i] = (PixelI32) ((x3 - x2) >> 14);
            blk7[i] = (PixelI32) ((x7 - x1) >> 14);
            
        }
    }

#   if !defined(_SH4_)
				FUNCTION_PROFILE_STOP(&fpDecode);
#	endif

}

// NOTE: These are not currently MMX versions of 8x4IDCT and 4x8IDCT. They are C versions
//  The only difference between these and the non-MMX versions are 
//  that these produce 16-bit output samples instead of 32-bit. This is to be compatible 
//  with the MMX motion comp routines which follow the IDCT. 
#endif //_EMB_WMV2_
