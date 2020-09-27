/*
 *   Copyright (c) 1997-8  S3 Inc.  All Rights Reserved.
 *
 *   Module Name:  s3tc.c
 *
 *   Purpose:  S3 linear interpolation encoding and decoding
 *             mechanism for texture map compression.
 *
 *   Author:  Konstantine Iourcha, Dan McCabe
 *
 *   Revision History:
 *       lthompso 98Jan05 Code beautification.
 */

//***************************************************************************
// Includes
//***************************************************************************

#include "pchtex.h"

//***************************************************************************
// Definitions
//***************************************************************************

// Primary color components (use DirextX byte ordering)
#undef BLU
#define BLU   0
#undef GRN
#define GRN   1
#undef RED
#define RED   2
#undef ALFA
#define ALFA 3

typedef	struct	{
	float		rgba[4];
} FCOLOR;	// internal color format

//
// Processing all primaries is such a common idiom
// that we define a macro for this action.
// Any self-respecting C compiler should easily optimize
// this by unrolling the loop!
//
#define	ForAllPrimaries		for( primary = 0; primary < NUM_PRIMARIES; ++primary)

// Similarly, processing all pixels in a block is a common idiom.
#define	ForAllPixels		for(pixel=0; pixel < S3TC_BLOCK_PIXELS; ++pixel)

#define NUM_PRIMARIES	3
#define	NUM_COMPONENTS	4
//
// Quantization constants for RGB565
//
#define	PRIMARY_BITS	8

#define	RED_BITS	5
#define	GRN_BITS	6
#define	BLU_BITS	5

#define	RED_SHIFT	(PRIMARY_BITS-RED_BITS)
#define	GRN_SHIFT	(PRIMARY_BITS-GRN_BITS)
#define	BLU_SHIFT	(PRIMARY_BITS-BLU_BITS)

#if 0
#define	RED_MASK	0xf8
#define	GRN_MASK	0xfc
#define	BLU_MASK	0xf8
#endif

// Weighting for each primary based on NTSC luminance
static	float	wtPrimary[NUM_PRIMARIES] = {
	0.0820f,	// blue
	0.6094f,	// green
	0.3086f		// red
};


//***************************************************************************
// Data
//***************************************************************************
// Look up table to map linear RGB indices to hardware indices.
// (Hardware expects extrema in indices 0 and 1 and interpolants in 2 and 3.)
static	int	mapRGB4[4] = {
	0,	// 0 => 0
	2,	// 1 => 2
	3,	// 2 => 3
	1	// 3 => 1
};

static	int	mapRGB3[4] = {
	0,	// 0 => 0
	2,	// 1 => 2
	1,	// 2 => 1
	3	// 3 => 3
};

//***************************************************************************
// Code
//***************************************************************************
// convert a fixed point color to a floating point color
static	void	ColorToFcolor(S3TC_COLOR *pcolor, FCOLOR *pfcolor)
{
	int	primary;

	ForAllPrimaries
		pfcolor->rgba[primary] = ((float) pcolor->rgba[primary]) * wtPrimary[primary] / 255.0f;
}

// convert a floating point color to a fixed point color
static	void	FcolorToColor(FCOLOR *pfcolor, S3TC_COLOR *pcolor)
{
	int	primary;

	ForAllPrimaries
		pcolor->rgba[primary] = (int) (pfcolor->rgba[primary] * 255.0f / wtPrimary[primary]);
}

// pack a fixed point color
static	void	ColorToRGB (S3TC_COLOR *pcolor, RGB565 *prgb)
{
	WORD	rgb;
	WORD	pri;

	pri  = pcolor->rgba[RED];
	pri >>= RED_SHIFT;
	rgb  = (BYTE) pri;

	pri  = pcolor->rgba[GRN];
	pri >>= GRN_SHIFT;
	rgb <<= GRN_BITS;
	rgb |= (BYTE) pri;

	pri  = pcolor->rgba[BLU];
	pri >>= BLU_SHIFT;
	rgb <<= BLU_BITS;
	rgb |= (BYTE) pri;

	*prgb = *((RGB565 *) &rgb);
}

// unpack a fixed point color
static	void	RGBToColor (RGB565 *prgb, S3TC_COLOR *pcolor)
{
	WORD	rgb;
	S3TC_COLOR	color;

	rgb = *((WORD *)prgb);

	// pick off bits in groups of 5, 6, and 5
	color.rgba[BLU] = (unsigned char) rgb;
	rgb >>= BLU_BITS;
	color.rgba[GRN] = (unsigned char) rgb;
	rgb >>= GRN_BITS;
	color.rgba[RED] = (unsigned char) rgb;

	// shift primaries into the appropriate LSBs
	color.rgba[BLU] <<= BLU_SHIFT;
	color.rgba[GRN] <<= GRN_SHIFT;
	color.rgba[RED] <<= RED_SHIFT;

	// replicate primaries MSBs into LSBs
	color.rgba[BLU] |= color.rgba[BLU] >> BLU_BITS;
	color.rgba[GRN] |= color.rgba[GRN] >> GRN_BITS;
	color.rgba[RED] |= color.rgba[RED] >> RED_BITS;

	*pcolor = color;
}

// Square a symmetric 3x3 matrix and store the result in another 3x3 matrix.
static	void Square3x3(
	float m[NUM_PRIMARIES][NUM_PRIMARIES],
	float m2[NUM_PRIMARIES][NUM_PRIMARIES])
{
    // It will be symmetric, so we'll only do the upper diagonal.
    // Note:  Using symmetry on the entries to be multiplied, also.

    m2[0][0] = m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2];
    m2[0][1] = m[0][0] * m[0][1] + m[0][1] * m[1][1] + m[0][2] * m[1][2];
    m2[0][2] = m[0][0] * m[0][2] + m[0][1] * m[1][2] + m[0][2] * m[2][2];
    m2[1][1] = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[1][2] * m[1][2];
    m2[1][2] = m[0][1] * m[0][2] + m[1][1] * m[1][2] + m[1][2] * m[2][2];
    m2[2][2] = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
}

// Convert floating point extrema colors into fixed point, quantized, RGB565
// colors and store in block structure.
static	void	Quantize(FCOLOR *pfcolor0, FCOLOR *pfcolor1, S3TCBlockRGB *pblock, int cOpaque)
{
	S3TC_COLOR	color;	// intermediate color

	// Convert clipped floating point extrema to fixed point block colors.
	FcolorToColor(pfcolor0, &color);
	ColorToRGB(&color, &pblock->rgb0);

	FcolorToColor(pfcolor1, &color);
	ColorToRGB(&color, &pblock->rgb1);

	// order extrema so that rgb0 > rgb1 to encode non-transparent block
	if ((cOpaque == 16) ^ (pblock->rgb0 > pblock->rgb1)) {
		RGB565	rgbSwap;

		rgbSwap = pblock->rgb0;
		pblock->rgb0 = pblock->rgb1;
		pblock->rgb1 = rgbSwap;
	}

	// and capture quantized extrema
	RGBToColor(&pblock->rgb0, &color);
	ColorToFcolor(&color, pfcolor0);
	
	RGBToColor(&pblock->rgb1, &color);
	ColorToFcolor(&color, pfcolor1);
}

// Find if extrema straddle any limits and if so, adjust accordingly.
static	void ClipExtrema(FCOLOR *plower, FCOLOR *pupper)
{
	float	w;		// weight at intersection of line and limit
	FCOLOR	*pfcolorOut;	// pointer to which extrema was outside limit
	int	primary;

	// For each primary color component, check upper and lower limits.
	ForAllPrimaries {
		// Do extrema straddle the lower limit?
		if ((plower->rgba[primary] < 0.0f)
		^   (pupper->rgba[primary] < 0.0f)) {
			// Compute intersection with upper limit.
			w = -plower->rgba[primary] / (pupper->rgba[primary] - plower->rgba[primary]);

			// Was the lower extreme outside?
			if (plower->rgba[primary] < 0.0f) {
				// Recompute lower extreme to lie on upper limit.
				pfcolorOut = plower;
			}
			else { // The upper extreme must have been outside.
				// Restate linear interpolation as starting from upper.
				w -= 1.0f;

				// Recompute upper extreme to lie on upper limit.
				pfcolorOut = pupper;
			}

			// Readjust the extrema which was outside the upper limit.
			pfcolorOut->rgba[RED] += w * (pupper->rgba[RED] - plower->rgba[RED]);
			pfcolorOut->rgba[GRN] += w * (pupper->rgba[GRN] - plower->rgba[GRN]);
			pfcolorOut->rgba[BLU] += w * (pupper->rgba[BLU] - plower->rgba[BLU]);
		}

		// Do extrema straddle the upper limit?
		if ((plower->rgba[primary] > wtPrimary[primary])
		^   (pupper->rgba[primary] > wtPrimary[primary])) {
			// Compute intersection with lower limit.
			w = (wtPrimary[primary] - plower->rgba[primary])
			  / (pupper->rgba[primary] - plower->rgba[primary]);

			// Was the lower limit outside?
			if (plower->rgba[primary] > wtPrimary[primary]) {
				// Recompute lower extreme to lie on lower limit.
				pfcolorOut = plower;
			}
			else { // The upper extreme must have been outside.
				// Restate linear interpolation as starting from upper.
				w -= 1.0f;

				// Recompute upper extreme to lie on lower limit.
				pfcolorOut = pupper;
			}

			// Readjust the extrema which was outside the upper limit.
			pfcolorOut->rgba[RED] += w * (pupper->rgba[RED] - plower->rgba[RED]);
			pfcolorOut->rgba[GRN] += w * (pupper->rgba[GRN] - plower->rgba[GRN]);
			pfcolorOut->rgba[BLU] += w * (pupper->rgba[BLU] - plower->rgba[BLU]);
		}
	}
}

// fill in a constant color block
static	void	AllSame (S3TC_COLOR *pcolor, S3TCBlockRGB *pblock, WORD wAlpha)
{
	int i;
	WORD wMask;
	DWORD dwMask;
        S3TC_COLOR color = *pcolor;

	// place color into extrema colors
	ColorToRGB(pcolor, &pblock->rgb0);
	pblock->rgb1 = pblock->rgb0;

	// use all indices of 0
	pblock->pixbm = 0;

	if (wAlpha == 0xffff)
	{
    	    return;   // all 16 pixels are opaque
	}

	// not all pixels are opaque, but all opaque pixels are same color
	for (i = 0, wMask = 1, dwMask = 3; i < 16; ++i, wMask <<= 1, dwMask <<= 2)
	{
    	    if (!(wAlpha & wMask))
	    {
		// transparent pixel -- set pixel index to 3
		pblock->pixbm |= dwMask;
	    }
	    else
	    {
    		color = pcolor[i];   // opaque pixel
	    }
	}
	ColorToRGB(&color, &pblock->rgb0);
	pblock->rgb1 = pblock->rgb0;
}

//***************************************************************************
// EncodeBlockRGB - compress a color block
//***************************************************************************
void EncodeBlockRGB (S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockRGB *pblockDst) {
	S3TC_COLOR	colorLo;
	S3TC_COLOR	colorHi;

	// force no valid transparent pixels
	colorLo.rgba[RED] =
	colorLo.rgba[GRN] =
	colorLo.rgba[BLU] = 255;

	colorHi.rgba[RED] =
	colorHi.rgba[GRN] =
	colorHi.rgba[BLU] = 0;

	EncodeBlockRGBColorKey(colorSrc, pblockDst, colorLo, colorHi);
}
//***************************************************************************
// EncodeBlockRGBColorKey - compress a color block with color key transparency
//***************************************************************************
void EncodeBlockRGBColorKey (
	S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS],
	S3TCBlockRGB *pblockDst,
	S3TC_COLOR colorLo,
	S3TC_COLOR colorHi
) {
	float	eigenvalue;	// approximated eigenvalue
	int	j, k;		// random Fortranesque loop indices
	int	primary;	// primary iteration index
	int	pixel;		// pixel iteration index
	int	pow;		// power iteration index
	int	lev=3;
	int	allsame;	// All the entries are equal.
	float	w;		// weight of color along axis
	float	wMin, wMax;     // extreme values
	float	axis2;          // axis squared
	WORD	wAlpha = 0;	// alpha at 1 BPP
	WORD	wMask;		// pick off bits of alpha mask
	int	cOpaque;
	FCOLOR	axis;           // dominant color axis
	FCOLOR	mean;		// average color
	FCOLOR  fcolor0;	// lower extreme color
	FCOLOR  fcolor1;	// upper extreme color
	FCOLOR	c[S3TC_BLOCK_PIXELS];// normalized and weighted colors
	float	t[NUM_PRIMARIES][NUM_PRIMARIES]; // moment of inertia tensor

	// if destination pointer is invalid, ...
	if (pblockDst == NULL)
		return;

	// count opaque pixels and mark them in bm
	cOpaque = 0;
	for (pixel = S3TC_BLOCK_PIXELS - 1; pixel >= 0; --pixel) {
		wAlpha <<= 1;

		// set bit if pixel is opaque
		if (colorLo.rgba[RED] <= colorSrc[pixel].rgba[RED]
		&&  colorSrc[pixel].rgba[RED] <= colorHi.rgba[RED]
		&&  colorLo.rgba[GRN] <= colorSrc[pixel].rgba[GRN]
		&&  colorSrc[pixel].rgba[GRN] <= colorHi.rgba[GRN]
		&&  colorLo.rgba[BLU] <= colorSrc[pixel].rgba[BLU]
		&&  colorSrc[pixel].rgba[BLU] <= colorHi.rgba[BLU]) {
			// pixel is transparent
			wAlpha &= ~1;
		}
		else { // pixel is opaque
			wAlpha |= 1;
			++cOpaque;
		}
	}

	// if there were no opaque pixels, ...
	if (cOpaque == 0) {
		// mark as having transparency
		pblockDst->rgb0 = 0;
		pblockDst->rgb1 = 0xffff;
		pblockDst->pixbm = 0xffffffff;

		return;
	}

	// detect a constant color block
	allsame = 1;
	ForAllPixels {
		// If we're all the same, nothing for us to do, so let's check to
		// see if we need to do all the work or can make a quick exit.
		if ((allsame) && (pixel > 0))
			if (colorSrc[pixel].rgba[RED] != colorSrc[pixel - 1].rgba[RED]
			||  colorSrc[pixel].rgba[GRN] != colorSrc[pixel - 1].rgba[GRN]
			||  colorSrc[pixel].rgba[BLU] != colorSrc[pixel - 1].rgba[BLU]) {
				allsame = 0;  // Found a different color, not all the same.
		}
	}

	// If the entries were all the same, there is only 1 color, so there's
	// no work for us to do.  Just unweight the colors and we're done.
	if (allsame) {
		AllSame(&colorSrc[0], pblockDst, wAlpha);

		return;  // We're done.
	}

	// Normalize each color entry in the block to the range [0..wtPrimary].
	ForAllPixels
		ColorToFcolor(&colorSrc[pixel], &c[pixel]);

	// Compute mean color
	ForAllPrimaries {
		mean.rgba[primary] = 0.0f;
		wMask = 1;
		ForAllPixels {
			if (wAlpha & wMask)
				mean.rgba[primary] += c[pixel].rgba[primary];
			wMask <<= 1;
		}
		mean.rgba[primary] /= (float) cOpaque;
	}

	// Relocate the mean for each primary color component (R,G,B)
	// to zero by subtracting it from each entry.
	ForAllPrimaries
		ForAllPixels
			c[pixel].rgba[primary] -= mean.rgba[primary];

	// Compute the moment of inertia tensor from the centered color values.
	// This will be t = [ sum(R^2), sum(R*G), sum(R*B) ]
	//                  [ sum(G*R), sum(G^2), sum(G*B) ]
	//                  [ sum(B*R), sum(B*G), sum(B^2) ].
	// Note that it is symmetric, so we only need to get the upper diagonal.
	ForAllPrimaries
		for (k = primary; k < NUM_PRIMARIES; k++) {
			t[primary][k] = 0.0f;
			wMask = 1;
			ForAllPixels {
				if (wAlpha & wMask) {
					t[primary][k] += c[pixel].rgba[primary] * c[pixel].rgba[k];
				}
				wMask <<= 1;
			}
		}

	// Compute principal eigenvalue and its eigen vector using the power method.
	// Rescale every iteration to avoid overflow.
	// Note:   Since the tensor matrix is symmetric, we only need to compute
	// from the diagonal up.
	// FIXME: optimization opportunity: stop looping if converged
	for (pow = 0; pow < 9; pow++) {
		float	scale;		// indicates magnitude of matrix
		float	tt[NUM_PRIMARIES][NUM_PRIMARIES]; // squared inertia tensor

		// Square the scaled matrix and store in tt.
		Square3x3(t, tt);

		// Square result again and store in t.
		Square3x3(tt, t);

		// rescale matrix to avoid overflow
		scale = t[0][0] + t[1][1] + t[2][2];
		
		// trace is positive definite. If it vanishes, moment of
		// inertia can't be diagonalized
		if (scale == 0.0f) {
			AllSame(&colorSrc[0], pblockDst, wAlpha);

			return;  // We're done.
		}
		else {
			scale = 3.0f / scale;

			// Rescale the matrix.
			ForAllPrimaries
				for (k = primary; k < NUM_PRIMARIES; k++)
					t[primary][k] *= scale;
		}
	}

	// Fill in below the diagonal for symmetric inertia tensor matrix
	t[1][0] = t[0][1];
	t[2][0] = t[0][2];
	t[2][1] = t[1][2];

	// Walk the diagonal of the power matrix to find the maximum entry.
	// This will give us the dominant eigenvalue.
	eigenvalue = 0.0f;
	ForAllPrimaries
		if (t[primary][primary] > eigenvalue) {
			eigenvalue = t[primary][primary];
			j = primary;
		}
	eigenvalue = (float) sqrt(eigenvalue);
	eigenvalue = 1.0f / eigenvalue;

	// Now get the color space axis.
	ForAllPrimaries
		axis.rgba[primary] = t[primary][j] * eigenvalue;

	// Compute the square of the magnitude of the axis vector
	axis2 = 0.0f;
	ForAllPrimaries
		axis2 += axis.rgba[primary] * axis.rgba[primary];

	// If the axis is 0, there is only 1 color, so no need for calculating
	// the extrema.  Just unweight the colors and exit.
	if (axis2 == 0.0f) {
		AllSame(&colorSrc[0], pblockDst, wAlpha);

		return;  // We're done.
	}

	// There must be more than 1 color, so let's go find the extrema.
	wMask = 1;
	// FIXME: this is really kludgy
	wMax = -99999.0f;
	wMin =  99999.0f;
	ForAllPixels {
		if (wAlpha & wMask) {
			// Compute weight along ray as w = ((c - mean) . axis)/(axis . axis)
			// Note:  c already holds c-mean
			w = 0.0f;
			ForAllPrimaries
				w += c[pixel].rgba[primary] * axis.rgba[primary];
			w /= axis2;
	
			if (w < wMin)		// If below minimum extreme,
				wMin = w;	//  capture new minimum.
			if (w > wMax)		// If above maximum extreme,
				wMax = w;	//  capture new maximum.
		}
		wMask <<= 1;
	}

	// Compute actual colors of extrema for each primary color component.
	ForAllPrimaries {
		fcolor0.rgba[primary] = mean.rgba[primary] + wMin * axis.rgba[primary];
		fcolor1.rgba[primary] = mean.rgba[primary] + wMax * axis.rgba[primary];
	}

#ifdef	DEBUG
	// fprintf(stderr, "Extrema before RGB clipping:\n");
	// fprintf(stderr, "(%g, %g, %g) - (%g, %g, %g)\n",
        //     fcolor0[RED], fcolor0[GRN], fcolor0[BLU],
        //     fcolor1[RED], fcolor1[GRN], fcolor1[BLU]);
#endif

	// Clip extrema to unit color cube.
	ClipExtrema(&fcolor0, &fcolor1);

#ifdef  DEBUG
	// fprintf(stderr, "Extrema after RGB clipping:\n");
	// fprintf(stderr, "(%g, %g, %g) - (%g, %g, %g)\n",
        //     fcolor0[RED], fcolor0[GRN], fcolor0[BLU],
        //     fcolor1[RED], fcolor1[GRN], fcolor1[BLU]);
#endif

	// Quantize extrema to fixed point RGB565.
	Quantize(&fcolor0, &fcolor1, pblockDst, cOpaque);

#ifdef  DEBUG
	// fprintf(stderr, "Extrema after quantizing:\n");
	// fprintf(stderr, "(%g, %g, %g) - (%g, %g, %g)\n",
        //     fcolor0[RED], fcolor0[GRN], fcolor0[BLU],
        //     fcolor1[RED], fcolor1[GRN], fcolor1[BLU]);
#endif

	// axis2 = sum of the squares of the differences in the two extrema.
	axis2 = 0.0f;
	ForAllPrimaries
		axis2 += (fcolor1.rgba[primary] - fcolor0.rgba[primary]) *
			 (fcolor1.rgba[primary] - fcolor0.rgba[primary]);

	// if we quantized to same color, ...
	if (axis2 == 0.0f && cOpaque == 16) {
		AllSame(&colorSrc[0], pblockDst, wAlpha);
		return;
	}

	// For all pixels in block, calculate and store the index to use.
	wMask = 0x8000;
	for (pixel = S3TC_BLOCK_PIXELS - 1; pixel >= 0; --pixel) {
		if (wAlpha & wMask) {
			// Compute weight along ray connecting extrema.
			w = 0.0f;
			ForAllPrimaries {
				// restore mean to color
				c[pixel].rgba[primary] += mean.rgba[primary];

				w += (c[pixel].rgba[primary] - fcolor0.rgba[primary]) *
				     (fcolor1.rgba[primary] - fcolor0.rgba[primary]);
			}
			w /= axis2;

			// if no transparent pixels, ...
			if (cOpaque == S3TC_BLOCK_PIXELS) {
				// Convert pixels weight from floating point to 2 bit fixed point.
				w *= 4.0f;

				// Clamp indices to valid values.
				if (w < 0.0f) {
					w = 0.0f;
				}
				else if (w >= 4.0f) {
					w = 3.0f;
				}

				// Record index in compressed block.
				pblockDst->pixbm <<= 2;
				pblockDst->pixbm |= mapRGB4[(int) w];
			}
			else {
				// Convert pixels weight from floating point to 2 bit fixed point.
				w *= 3.0f;

				// Clamp indices to valid values.
				if (w < 0.0f)
					w = 0.0f;
				else if (w >= 3.0f)
					w = 2.0f;

				// Record index in compressed block.
				pblockDst->pixbm <<= 2;
				pblockDst->pixbm |= mapRGB3[(int) w];
			}
		}
		else {	// transparent pixel
			pblockDst->pixbm <<= 2;
			pblockDst->pixbm |= 3;
		}

		wMask >>= 1;
	}
}



//***************************************************************************
// DecodeBlockRGB - decompress a color block
//***************************************************************************
void DecodeBlockRGB (S3TCBlockRGB *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS])
{
	int	lev;
	S3TC_COLOR	clut[4];
	PIXBM	pixbm;
	int	pixel;
	int	primary;

	// if source block is invalid, ...
	if (pblockSrc == NULL)
    {
        memset(colorDst, 0, sizeof(S3TC_COLOR) * S3TC_BLOCK_PIXELS);
		return;
    }

	// determine the number of color levels in the block
	lev = (pblockSrc->rgb0 <= pblockSrc->rgb1) ? 2 : 3;

	// Fill extrema values into pixel code lookup table.
	RGBToColor(&pblockSrc->rgb0, &clut[0]);
	RGBToColor(&pblockSrc->rgb1, &clut[1]);
	
	clut[0].rgba[ALFA] =
	clut[1].rgba[ALFA] =
	clut[2].rgba[ALFA] = 255;

	// Calculate linear interpolant values.
	if (lev == 3) {	// No transparency info present, all color info.
		ForAllPrimaries {
			WORD temp0 = clut[0].rgba[primary];   // jvanaken fixed overflow bug
			WORD temp1 = clut[1].rgba[primary];
			clut[2].rgba[primary] = (BYTE)((2*temp0 + temp1 + 1)/3);
			clut[3].rgba[primary] = (BYTE)((temp0 + 2*temp1 + 1)/3);
		}
		clut[3].rgba[ALFA] = 255;
	}
	else {	// transparency info.
		ForAllPrimaries {
			WORD temp0 = clut[0].rgba[primary];   // jvanaken fixed overflow bug
			WORD temp1 = clut[1].rgba[primary];
			clut[2].rgba[primary] = (BYTE)((temp0 + temp1)/2);
			clut[3].rgba[primary] = 0;     // jvanaken added this
		}
		clut[3].rgba[ALFA] = 0;
	}

	// munge a local copy
	pixbm = pblockSrc->pixbm;

	// Look up the actual pixel color in the table.
	ForAllPixels {
		// lookup color from pixel bitmap
		ForAllPrimaries
			colorDst[pixel].rgba[primary] = clut[pixbm & 3].rgba[primary];

		colorDst[pixel].rgba[ALFA] = clut[pixbm & 3].rgba[ALFA];

		// prepare to extract next index
		pixbm >>= 2;
	}
}

// EncodeBlockAlpha4 - compress a block with alpha at 4 BPP
void EncodeBlockAlpha4(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockAlpha4 *pblockDst)
{
	int	row, col;
	WORD	*palpha;

	for (row = 0; row < 4; ++row) {
		palpha = &pblockDst->alphabm[row];

		for (col = 3; col >= 0; --col) {
			*palpha <<= 4;
			*palpha |= (colorSrc[4 * row + col].rgba[ALFA] >> 4) & 0xf;
		}
	}

	EncodeBlockRGB(colorSrc, &pblockDst->rgb);
}

// DecodeBlockAlpha4 - decompress a block with alpha at 4 BPP
void DecodeBlockAlpha4(S3TCBlockAlpha4 *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS])
{
	int	row, col;
	WORD	alpha;

	DecodeBlockRGB(&pblockSrc->rgb, colorDst);

	for (row = 0; row < 4; ++row) {
		alpha = pblockSrc->alphabm[row];

		for (col = 0; col < 4; ++col) {
			colorDst[4 * row + col].rgba[ALFA] =
				  ((alpha & 0xf) << 4)
				| (alpha & 0xf);
			alpha >>= 4;
		}
	}
}

// EncodeBlockAlpha3 - compress a block with alpha at 3 BPP
void EncodeBlockAlpha3(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockAlpha3 *pblockDst)
{
    int	pixel;
    BYTE	alpha0;
    BYTE	alpha1;
    DWORD	dwBM = 0;
    BOOL    b6Alpha;
    // compute extreme alpha values in block
    alpha0 =
    alpha1 = colorSrc[0].rgba[ALFA];	
    for (pixel = 1; pixel < S3TC_BLOCK_PIXELS; ++pixel) 
    {
        if (colorSrc[pixel].rgba[ALFA] > alpha0)
            alpha0 = colorSrc[pixel].rgba[ALFA];
        if (colorSrc[pixel].rgba[ALFA] < alpha1)
            alpha1 = colorSrc[pixel].rgba[ALFA];
    }
    if ( 255 == alpha0 && 0 == alpha1)
    {
        for (pixel = 0; pixel < S3TC_BLOCK_PIXELS; ++pixel) 
        {
            if ((colorSrc[pixel].rgba[ALFA] < alpha0) 
                && (0 != colorSrc[pixel].rgba[ALFA])
               )
                alpha0 = colorSrc[pixel].rgba[ALFA];
            if ((colorSrc[pixel].rgba[ALFA] > alpha1)
                && (255 != colorSrc[pixel].rgba[ALFA])
               )
                alpha1 = colorSrc[pixel].rgba[ALFA];
        }
        if ( alpha0 < alpha1)
        {
            // result always alpha0 < alpha1
            // which is the order for 6 alpha ramp
            b6Alpha = TRUE;	
        }
        else
        {
            // result always alpha0 >= alpha1
            // which is the order for 8 alpha ramp
            b6Alpha = FALSE;
            alpha0 = 255;   //restore for 8 alpha ramp
            alpha1 = 0;
        }
    }
    else
    {
        // result always alpha0 >= alpha1
        // which is the order for 8 alpha ramp
        b6Alpha = FALSE;
    }
  
    // write extreme alphas to block
    pblockDst->alpha0 = alpha0;
    pblockDst->alpha1 = alpha1;

    if (alpha0 != alpha1) 
    {
        int dist = ((int)alpha0 - (int)alpha1);
        int dist2 = dist >> 1;
        int scale = b6Alpha ? 5 : 7;

        for (pixel = S3TC_BLOCK_PIXELS - 1; pixel >= 0; --pixel) 
        {
            // pack 3 bits at a time
            dwBM <<= 3;
            if (b6Alpha && 0 == colorSrc[pixel].rgba[ALFA])
            {
                dwBM |= 6;
            }
            else
            if (b6Alpha && 255 == colorSrc[pixel].rgba[ALFA])
            {
                dwBM |= 7;
            }
            else
            {
                // compute relative distance from extrema
                int	w = ((int)alpha0 - (int)colorSrc[pixel].rgba[ALFA]);

                // scale to 3 bits when comverted to fixed point
                w = (w * scale + dist2) / dist;

                if(w >= scale)
                    dwBM |= 1;
                else if(w > 0)
                    dwBM |= w + 1;
            }
 
            // flush alphabm every 8 pixels (every 3 bytes)
            if ((pixel & 7) == 0) 
            {
                if (pixel == 8) 
                {
                    // unpack 8 bits at a time
                    pblockDst->alphabm[3] = (BYTE) dwBM;
                    dwBM >>= 8;
                    pblockDst->alphabm[4] = (BYTE) dwBM;
                    dwBM >>= 8;
                    pblockDst->alphabm[5] = (BYTE) dwBM;
                }
                else 
                { // pixel == 0
                    // unpack 8 bits at a time
                    pblockDst->alphabm[0] = (BYTE) dwBM;
                    dwBM >>= 8;
                    pblockDst->alphabm[1] = (BYTE) dwBM;
                    dwBM >>= 8;
                    pblockDst->alphabm[2] = (BYTE) dwBM;
                }
            }
        }
    }
    else 
    {	// only one distinct alpha
        pblockDst->alphabm[0] =
        pblockDst->alphabm[1] =
        pblockDst->alphabm[2] =
        pblockDst->alphabm[3] =
        pblockDst->alphabm[4] =
        pblockDst->alphabm[5] = 0;
    }

    EncodeBlockRGB(colorSrc, &pblockDst->rgb);
}

// DecodeBlockAlpha3 - decompress a block with alpha at 3 BPP
void DecodeBlockAlpha3(S3TCBlockAlpha3 *pblockSrc, S3TC_COLOR colorDst[S3TC_BLOCK_PIXELS])
{
    int	pixel;
    int	alpha[8];	// alpha lookup table
    DWORD	dwBM = 0;	// alpha bitmap in DWORD cache

    DecodeBlockRGB(&pblockSrc->rgb, colorDst);

    alpha[0] = pblockSrc->alpha0;
    alpha[1] = pblockSrc->alpha1;

    // if 8 alpha ramp, ...
    if (alpha[0] > alpha[1]) 
    {
        // interpolate intermediate colors with rounding
        alpha[2] = (6 * alpha[0] +     alpha[1]) / 7;
        alpha[3] = (5 * alpha[0] + 2 * alpha[1]) / 7;
        alpha[4] = (4 * alpha[0] + 3 * alpha[1]) / 7;
        alpha[5] = (3 * alpha[0] + 4 * alpha[1]) / 7;
        alpha[6] = (2 * alpha[0] + 5 * alpha[1]) / 7;
        alpha[7] = (    alpha[0] + 6 * alpha[1]) / 7;
    }
    else 
    { // else 6 alpha ramp with 0 and 255
        // interpolate intermediate colors with rounding
        alpha[2] = (4 * alpha[0] +     alpha[1]) / 5;
        alpha[3] = (3 * alpha[0] + 2 * alpha[1]) / 5;
        alpha[4] = (2 * alpha[0] + 3 * alpha[1]) / 5;
        alpha[5] = (    alpha[0] + 4 * alpha[1]) / 5;
        alpha[6] = 0;
        alpha[7] = 255;
    }

    ForAllPixels {
        // reload bitmap dword cache every 8 pixels
        if ((pixel & 7) == 0) {
            if (pixel == 0) {
                // pack 3 bytes into dword
                dwBM  = pblockSrc->alphabm[2];
                dwBM <<= 8;
                dwBM |= pblockSrc->alphabm[1];
                dwBM <<= 8;
                dwBM |= pblockSrc->alphabm[0];
            }
            else {	// pixel == 8
                // pack 3 bytes into dword
                dwBM  = pblockSrc->alphabm[5];
                dwBM <<= 8;
                dwBM |= pblockSrc->alphabm[4];
                dwBM <<= 8;
                dwBM |= pblockSrc->alphabm[3];
            }
        }

        // unpack bitmap dword 3 bits at a time
        colorDst[pixel].rgba[ALFA] = (BYTE)(alpha[(dwBM & 7)]);
        dwBM >>= 3;
    }
}

