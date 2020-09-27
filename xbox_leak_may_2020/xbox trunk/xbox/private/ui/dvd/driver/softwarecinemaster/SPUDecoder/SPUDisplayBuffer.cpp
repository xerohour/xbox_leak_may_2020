////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "SPUDisplayBuffer.h"
#include "..\common\timedfibers.h"
#include "library\common\vddebug.h"

#define ADVANCENB(n)	\
	if (bits >= n)					\
		{								\
		buff <<= n;					\
		bits -= n;					\
		}								\
	else								\
		{								\
		buff <<= bits;				\
		buff |= *map++;			\
		if (map >= limit) return;	\
		buff <<= (n - bits);		\
		bits = bits + 8 - n;		\
		}

////////////////////////////////////////////////////////////////////
//
//  SPU Display Buffer
//
////////////////////////////////////////////////////////////////////

void SPUDisplayBuffer::CalculateNonTransparentRegion(void)
	{
	nonTransparentTop = top;
	nonTransparentHeight = height;
	}

////////////////////////////////////////////////////////////////////
//
//  Decoded SPU Display Buffer
//
////////////////////////////////////////////////////////////////////

void DecodedSPUDisplayBuffer::SetColorTable(SPUColorEntry * colorTable)
	{
	int i;

	for(i=0; i<16; i++)
		nativeColorTable[i] = colorTable[i];

	RebuildPalette();
	}

void DecodedSPUDisplayBuffer::RebuildPalette(void)
	{
	int c, t, i;

	for(i=0; i<usedColorSets*4; i++)
		{
		c = XTBF(16 + 4 * (i & 3), 4, colorSet[i >> 2]);
		t = XTBF(     4 * (i & 3), 4, colorSet[i >> 2]);
		translatedColorTable[i] = nativeColorTable[c];
		if (t == 0)
			translatedColorTable[i].t = 0;
		else
			translatedColorTable[i].t = t + 1;
		}
	}

void DecodedSPUDisplayBuffer::SetImage(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height, DWORD color, BYTE * colcon, DWORD uniqueID)
	{
	int chstart, chstop, chnum, chcolpred, chcolumn;
	DWORD chColor;
	int i, j;

	this->uniqueID = uniqueID;
	buttonChangePending = FALSE;

	DecodeRectangle(odd, even, limit, left, top, width, height);

	colorSet[0] = color;
	usedColorSets = 1;

	if (colcon)
		{
		colcon += 2;
		chstart = MAKEWORD(colcon[1], colcon[0]);
		chstop  = MAKEWORD(colcon[3], colcon[2]);
		while (chstart != 0x0fff && chstop != 0xffff)
			{
			chnum = chstop >> 12;
			chstop &= 0x0fff;

			colcon += 4;

			if (chnum)
				{
				chcolpred = MAKEWORD(colcon[1], colcon[0]);
				for(i=1; i<=chnum; i++)
					{
					chColor = MAKELONG4(colcon[5], colcon[4], colcon[3], colcon[2]);

					if (i != chnum)
						chcolumn = MAKEWORD(colcon[7], colcon[6]);
					else
						chcolumn = (left + width);

					for(j=0; j<usedColorSets; j++) if (chColor == colorSet[j]) break;
					if (j > 0 && j < 4)
						{
						if (j == usedColorSets)
							{
							colorSet[j] = chColor;
							usedColorSets++;
							}

						MarkAlternateColorRect(chcolpred, chstart, chcolumn - chcolpred, chstop - chstart + 1, j);
						}

					chcolpred = chcolumn;

					colcon += 6;
					}
				}

			chstart = MAKEWORD(colcon[1], colcon[0]);
			chstop  = MAKEWORD(colcon[3], colcon[2]);
			}
		}

	changed = TRUE;
	}

//
//  Set Button
//

void DecodedSPUDisplayBuffer::SetButton(int buttonLeft, int buttonTop, int buttonWidth, int buttonHeight, DWORD color)
	{
	if (usedColorSets > 1)
		{
		if (color == colorSet[0])
			{
			RemoveAlternateColors();
			usedColorSets = 1;
			RebuildPalette();

			buttonChangePending = FALSE;
			changed = TRUE;
			}
		else
			{
			if (this->buttonLeft != buttonLeft || this->buttonTop != buttonTop ||
				 this->buttonWidth != buttonWidth || this->buttonHeight != buttonHeight)
				{
				buttonChangePending = TRUE;

				this->buttonLeft = buttonLeft;
				this->buttonTop = buttonTop;
				this->buttonWidth = buttonWidth;
				this->buttonHeight = buttonHeight;
				}
			else if (!buttonChangePending && color != colorSet[1])
				{
				colorSet[1] = color;
				RebuildPalette();
				}

			this->buttonColor = color;

			changed = TRUE;
			}
		}
	else if (color != colorSet[0])
		{
		MarkAlternateColorRect(buttonLeft, buttonTop, buttonWidth, buttonHeight, 1);
		this->buttonLeft = buttonLeft;
		this->buttonTop = buttonTop;
		this->buttonWidth = buttonWidth;
		this->buttonHeight = buttonHeight;
		this->buttonColor = color;

		usedColorSets = 2;
		colorSet[1] = color;
		RebuildPalette();

		buttonChangePending = FALSE;
		changed = TRUE;
		}
	}

//
//  Perform Button Change
//

void DecodedSPUDisplayBuffer::PerformButtonChange(void)
	{
	if (buttonChangePending)
		{
		RemoveAlternateColors();
		colorSet[1] = buttonColor;
		RebuildPalette();
		MarkAlternateColorRect(buttonLeft, buttonTop, buttonWidth, buttonHeight, 1);

		buttonChangePending = FALSE;
		}
	}

void DecodedSPUDisplayBuffer::DecodeFieldRectangle2BPP(BYTE * map, BYTE * limit, int preLeft, int width, bool odd, int & topEmpty, int & bottomEmpty, DWORD * nonZeroLines)
	{
	static DWORD maps[] = {0x00000000, 0x55555555, 0xaaaaaaaa, 0xffffffff};
	static DWORD masks[] = {0x00000000, 0xc0000000, 0xf0000000, 0xfc000000,
		                     0xff000000, 0xffc00000, 0xfff00000, 0xfffc0000,
									0xffff0000, 0xffffc000, 0xfffff000, 0xfffffc00,
									0xffffff00, 0xffffffc0, 0xfffffff0, 0xfffffffc,
									0xffffffff};
	int x, y, px;
	int buff;
	int bits;
	int ntwo;
	int pix;
	DWORD color;
	int stride = bytesPerRow / 2 - (preLeft + width + 15) / 16;

	DWORD * rowoptr = (DWORD *)(image + bytesPerRow * odd);
	DWORD obuff;
	int  obits;
	int	framePixOr;
	int	linePixOr;

	topEmpty = bottomEmpty = 0;
	framePixOr = 0;

	buff = (int)(*map++) << 8;
	bits = 0;
	for(y = odd; y < height; y+=2)
		{
        DWORD * optr = rowoptr;
		ADVANCENB(bits)
		linePixOr = 0;
		obits = 16 - preLeft;
		obuff = 0;
		x = 0;
		while (x < width)
			{
			if      (buff & 0xc000)
				{
				ntwo = (buff >> 14) & 3;
				pix = (buff >> 12) & 3;
				ADVANCENB(4)
				}
			else if (buff & 0xf000)
				{
				ntwo = (buff >> 10) & 15;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else if (buff & 0xfc00)
				{
				ADVANCENB(4)
				ntwo = (buff >> 10) & 63;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else if (buff & 0xff00)
				{
				ADVANCENB(6)
				ntwo = (buff >> 8) & 255;
				ADVANCENB(2)
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else
				{
				ADVANCENB(8)
				ntwo = width - x;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}

			linePixOr |= pix;

            px = x;
            x += ntwo;

            if (x > width)
            {
                ntwo -= x - width;
                x = width;
            }

			color = maps[pix];

			if (obits > ntwo)
				{
				obuff = (obuff >> 2 * ntwo) | (color & masks[ntwo]);
				obits -= ntwo;
				}
			else
				{
				if (obits < 16)
					{
					obuff = (obuff >> 2 * obits) | (color & masks[obits]);
					*optr++ = obuff;
					ntwo-=obits;
					}
				while (ntwo >= 16)
					{
					*optr++ = color;
					ntwo -= 16;
					}
				obits = 16 - ntwo;
				obuff = color;
				}
			}

		if (obits < 16)
			{
			obuff >>= 2 * obits;
			*optr++ = obuff;
			}

		rowoptr += stride * 2;
		framePixOr |= linePixOr;
		if (!framePixOr) topEmpty++;
		bottomEmpty++;
		if (linePixOr) bottomEmpty = 0;

		if (!(y & 0x1e)) YieldTimedFiber(0);
		}
	}

void DecodedSPUDisplayBuffer::DecodeFieldRectangle4BPP(BYTE * map, BYTE * limit, int preLeft, int width, bool odd, int & topEmpty, int & bottomEmpty, DWORD * nonZeroLines)
	{
	static DWORD maps[] = {0x00000000, 0x11111111, 0x22222222, 0x33333333};
	static DWORD masks[] = {0x00000000, 0xf0000000, 0xff000000, 0xfff00000,
									0xffff0000, 0xfffff000, 0xffffff00, 0xfffffff0,
									0xffffffff};
	int x, y, px;
	int buff;
	int bits;
	int ntwo;
	int pix;
	DWORD color, nonZeroLine;

	DWORD * rowoptr = (DWORD *)(image + bytesPerRow * odd);
	DWORD obuff;
	int  obits;
	int	framePixOr;
	int	linePixOr;

	topEmpty = bottomEmpty = 0;
	framePixOr = 0;

	buff = (int)(*map++) << 8;
	bits = 0;
	nonZeroLine = 0;
	for(y = odd; y < height; y+=2)
		{
        DWORD * optr = rowoptr;
		ADVANCENB(bits)
		linePixOr = 0;
		obits = 8 - preLeft;
		obuff = 0;
		x = 0;
		while (x < width)
			{
			if      (buff & 0xc000)
				{
				ntwo = (buff >> 14) & 3;
				pix = (buff >> 12) & 3;
				ADVANCENB(4)
				}
			else if (buff & 0xf000)
				{
				ntwo = (buff >> 10) & 15;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else if (buff & 0xfc00)
				{
				ADVANCENB(4)
				ntwo = (buff >> 10) & 63;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else if (buff & 0xff00)
				{
				ADVANCENB(6)
				ntwo = (buff >> 8) & 255;
				ADVANCENB(2)
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}
			else
				{
				ADVANCENB(8)
				ntwo = width - x;
				pix = (buff >> 8) & 3;
				ADVANCENB(8)
				}

			linePixOr |= pix;

            px = x;
            x += ntwo;

            if (x > width)
            {
                ntwo -= x - width;
                x = width;
            }

			color = maps[pix];

			if (obits > ntwo)
				{
				obuff = (obuff >> 4 * ntwo) | (color & masks[ntwo]);
				obits -= ntwo;
				}
			else
				{
				if (obits < 8)
					{
					obuff = (obuff >> 4 * obits) | (color & masks[obits]);
					*optr++ = obuff;
					ntwo-=obits;
					}
				while (ntwo >= 8)
					{
					*optr++ = color;
					ntwo -= 8;
					}
				obits = 8 - ntwo;
				obuff = color;
				}
			}

		if (obits < 8)
			{
			obuff >>= 4 * obits;
			*optr++ = obuff;
			}

		rowoptr += bytesPerRow / sizeof(DWORD) * 2;
		framePixOr |= linePixOr;
		if (!framePixOr) topEmpty++;
		bottomEmpty++;
		if (linePixOr) bottomEmpty = 0;
		nonZeroLine >>= 2;
		if (linePixOr) nonZeroLine |= 0xc0000000;
		if ((y & 0x1e) == 0x1e) nonZeroLines[y >> 5] = nonZeroLine;


		if (!(y & 0x1e)) YieldTimedFiber(0);
		}

	if ((height & 0x1e) != 0)
		{
		nonZeroLines[height >> 5] = nonZeroLine >> (32 - (height & 0x1e));
		}
	}

//
//  Constructor
//

DecodedSPUDisplayBuffer::DecodedSPUDisplayBuffer(void)
	{
	image = NULL;
	halfImage = NULL;
	}

//
//  Destructor
//

DecodedSPUDisplayBuffer::~DecodedSPUDisplayBuffer(void)
	{
	if (image) delete[] image;
	if (halfImage) delete[] halfImage;
	}

//
//  Calculate Non Transparent Region
//

void DecodedSPUDisplayBuffer::CalculateNonTransparentRegion(void)
	{
	if (usedColorSets == 1 && translatedColorTable[0].t == 0)
		{
		nonTransparentTop = top + topEmpty;
		nonTransparentHeight = height - topEmpty - bottomEmpty;

		if (nonTransparentHeight < 0)
			{
			nonTransparentHeight = 0;
			topEmpty = bottomEmpty = 0;
			}
		}
	else
		SPUDisplayBuffer::CalculateNonTransparentRegion();
	}

////////////////////////////////////////////////////////////////////
//
//  Decoded SPU Display Buffer 4 Bits/Pixel
//
////////////////////////////////////////////////////////////////////

void DecodedSPUDisplayBuffer4BPP::DecodeRectangle(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height)
	{
	int preLeft, i;
	int evenTopEmpty, oddTopEmpty, evenBottomEmpty, oddBottomEmpty;

	preLeft = left & 7;

	this->left = left - preLeft;
	this->top = top;
	this->width = (width + preLeft + 7) & ~7;
	this->height = height;

	bytesPerRow = this->width >> 1;

	if (halfImage)
		{
		delete[] halfImage;
		halfImage = NULL;
		}
	if (image) delete[] image;
	image = new BYTE[bytesPerRow * (height + 2)];

	DecodeFieldRectangle4BPP(even, limit, preLeft, width, FALSE, evenTopEmpty, evenBottomEmpty, mainNonZeroLinesMap);
	DecodeFieldRectangle4BPP(odd, limit, preLeft, width, TRUE, oddTopEmpty, oddBottomEmpty, extNonZeroLinesMap);

	for(i=0; i<18; i++)
		{
		mainNonZeroLinesMap[i] = (mainNonZeroLinesMap[i] & 0x55555555) | (extNonZeroLinesMap[i] & 0xaaaaaaaa);
		}
	nonZeroLinesMap = mainNonZeroLinesMap;

	topEmpty = 2 * min(evenTopEmpty, oddTopEmpty);
	bottomEmpty = 2 * min(evenBottomEmpty, oddBottomEmpty);

	minAlternateTop = height;
	maxAlternateBottom = 0;
	}

//
//  Mark alternate color rectangle
//

void DecodedSPUDisplayBuffer4BPP::MarkAlternateColorRect(int alternateLeft, int alternateTop, int alternateWidth, int alternateHeight, int val)
	{
	int x, y, w, lx, rx, h;
	DWORD * mp;
	DWORD mask, lmask, rmask;

	if (val)
		{
		//
		//  Correct top border and height
		//

		for(y=0; y<18; y++)
			extNonZeroLinesMap[y] = mainNonZeroLinesMap[y];

		if (alternateTop < top)
			{
			alternateHeight -= top - alternateTop;
			alternateTop = 0;
			}
		else
			alternateTop -= top;

		//
		//  Correct left border and width
		//

		if (alternateLeft < left)
			{
			alternateWidth -= left - alternateLeft;
			alternateLeft = 0;
			}
		else
			alternateLeft -= left;

		h = alternateHeight;

		if (alternateLeft + alternateWidth > width) alternateWidth = width - alternateLeft;
		if (alternateTop + alternateHeight > height) h = height - alternateTop;

		if (alternateTop < minAlternateTop) minAlternateTop = alternateTop;
		if (h + alternateTop > maxAlternateBottom) maxAlternateBottom = h + alternateTop;

		lx = alternateLeft & 7;
		rx = (alternateLeft + alternateWidth) & 7;

		w = ((int)alternateWidth - (8-lx) - rx) >> 3;
		mask = (DWORD)val * 0x44444444;	// Dithering mask
		lmask = mask << 4 * lx;				// Left rim mask
		rmask = mask >> 4 * (8 - rx);		// Right rim mask

		mp = (DWORD *)(image + alternateTop * bytesPerRow) + (alternateLeft >> 3);
		for(y=0; y<h; y++)
			{
			mp[0] |= lmask;
			for(x=0; x<w; x++)
				{
				mp[x+1] |= mask;
				}
			if (rx && rmask)
				mp[w+1] |= rmask;
			mp += bytesPerRow >> 2;
			}

		for(y=alternateTop; y<alternateTop + h; y++)
			{
			extNonZeroLinesMap[y >> 5] |= (1 << (y & 31));
			}

		nonZeroLinesMap = extNonZeroLinesMap;
		}

	if (halfImage)
		{
		delete[] halfImage;
		halfImage = NULL;
		}
	}

//
//  Remove alternate colors
//

void DecodedSPUDisplayBuffer4BPP::RemoveAlternateColors(void)
	{
	int x, y, w;
	DWORD * mp;

	w = width >> 3;

	mp = (DWORD *)image;

	for(y=0; y<height; y++)
		{
		for(x=0; x<w; x++)
			{
			mp[x] &= 0x33333333;
			}
		mp += bytesPerRow >> 2;
		}

	minAlternateTop = height;
	maxAlternateBottom = 0;

	nonZeroLinesMap = mainNonZeroLinesMap;

	if (halfImage)
		{
		delete[] halfImage;
		halfImage = NULL;
		}
	}

////////////////////////////////////////////////////////////////////
//
//  Full Width MMX SPU Display Buffer
//
////////////////////////////////////////////////////////////////////

void FullWidthMMXSPUDisplayBuffer::RebuildPalette(void)
	{
	int i, j, k;
	DWORD tmul[16], yu[16], yv[16], y[16], u[16], v[16];
	int num;

	DecodedSPUDisplayBuffer::RebuildPalette();

	num = 4 * usedColorSets;

	for(i=0; i<num; i++)
		{
		k = translatedColorTable[i].t;
		if (k == 15) k = 16;
		tmul[i] = (DWORD)(16 - k) * 0x00010001;
		y[i] = (DWORD)(translatedColorTable[i].y) * k;
		u[i] = (DWORD)(translatedColorTable[i].u) * k;
		v[i] = (DWORD)(translatedColorTable[i].v) * k;

		yu[i] = (u[i] << 16) | y[i];
		yv[i] = (v[i] << 16) | y[i];
		}

	for(i=0; i<num; i++)
		{
		for(j=0; j<num; j++)
			{
			k = i * 16 + j;

			colorLookup[k].l = yv[j];
			colorLookup[k].h = yu[i];
			transparencyLookup[k].l = tmul[j];
			transparencyLookup[k].h = tmul[i];

			byteColorTransparencyLookup[k].l =
				(y[j] >> 4) |
				(y[i] >> 4 << 8) |
				(v[j] >> 4 << 16) |
				(u[i] >> 4 << 24);
			byteColorTransparencyLookup[k].h =
				tmul[j] | (tmul[i] << 8);

			hcolorLookup1[k].l = yv[j] + (yu[i] & 0x0000ffff);
			hcolorLookup1[k].h = yu[i] & 0xffff0000;
			htransparencyLookup1[k].l = tmul[j] + (tmul[i] & 0x0000ffff);
			htransparencyLookup1[k].h = tmul[i] & 0xffff0000;
			hcolorLookup2[k].l = yv[j] & 0xffff0000;
			hcolorLookup2[k].h = yu[i] + (yv[j] & 0x0000ffff);
			htransparencyLookup2[k].l = tmul[j] & 0xffff0000;
			htransparencyLookup2[k].h = tmul[i] + (tmul[j] & 0x0000ffff);
			}
		}

	}

static inline DWORD PackDWORDEvenBits(DWORD d)
	{
	d = (d & 0x11111111) | ((d >> 1) & 0x22222222);
	d = (d & 0x03030303) | ((d >> 2) & 0x0c0c0c0c);
	d = (d & 0x000f000f) | ((d >> 4) & 0x00f000f0);
	d = (d & 0x000000ff) | ((d >> 8) & 0x0000ff00);

	return d;
	}

void FullWidthMMXSPUDisplayBuffer::GetDisplayParameters(int decodeWidth, int decodeHeight,
			                                               int & xosdstart, int & xosdstop,
			                                               int & yosdstart, int & yosdstop,
																		  DWORD * & nonZeroLinesMap, int & yoffset,
			                                               BYTE * & po, int & obpr)
	{
	int x, y, lo, ro;
	BYTE * spo, * dpo;

	switch (decodeWidth)
		{
		case 720:
		case 360:
			xosdstart = left;
			xosdstop  = left + width;
			obpr      = bytesPerRow;
			po        = image + bytesPerRow * (nonTransparentTop - top);
			break;
		case 704:
			xosdstart = left - 8;
			xosdstop  = left + width - 16;
			obpr      = bytesPerRow;
			po        = image + bytesPerRow * (nonTransparentTop - top);
			if (xosdstart < 0)
				{
				po -= (xosdstart >> 1);
				xosdstart = 0;
				}
			if (xosdstop > decodeWidth)
				xosdstop = decodeWidth;
			yoffset   = top;
			nonZeroLinesMap = this->nonZeroLinesMap;
			break;
		case 352:
			xosdstart = left - 8;
			xosdstop  = left + width - 16;
			xosdstart >>= 1;
			xosdstop >>= 1;

			if (xosdstart < 0)
				{
				po -= (xosdstart >> 2);
				xosdstart = 0;
				}
			if (xosdstop > decodeWidth)
				xosdstop = decodeWidth;

			obpr      = bytesPerRow >> 1;
			if (xosdstart & 4)
				{
				xosdstart -= 4;
				obpr += 2;
				lo = 2;
				}
			else
				lo = 0;

			if (xosdstop & 4)
				{
				xosdstop += 4;
				obpr += 2;
				ro = 2;
				}
			else
				ro = 0;

			if (!halfImage)
				{
				halfImage = new BYTE[obpr * height];

				spo = image;
				dpo = halfImage;
				for(y=0; y<height; y++)
					{
					for(x=0; x<lo; x++)
						dpo[x] = 0;
					for(x=0; x<ro; x++)
						dpo[obpr-ro+x] = 0;
					for(x=0; x<bytesPerRow; x+=2)
						{
						dpo[(lo + x) >> 1] = (spo[x+1] & 0xf0) | (spo[x] >> 4);
						}
					dpo += obpr;
					spo += bytesPerRow;
					}
				}


			po        = halfImage + obpr * (nonTransparentTop - top);
			break;
		}

	switch (decodeHeight)
		{
		case 480:
		case 576:
			yosdstart = nonTransparentTop;
			yosdstop  = nonTransparentTop + nonTransparentHeight;

			nonZeroLinesMap = this->nonZeroLinesMap;
			yoffset   = top;
			break;
		case 240:
		case 288:
//			po += obpr * (nonTransparentTop - top);
			yosdstart = nonTransparentTop >> 1;
			yosdstop = (nonTransparentTop + nonTransparentHeight) >> 1;
			obpr *= 2;
			yoffset   = top >> 1;

			for(y = 0; y<9; y++)
				{
				halfNonZeroLinesMap[y] = PackDWORDEvenBits(this->nonZeroLinesMap[2*y]) | (PackDWORDEvenBits(this->nonZeroLinesMap[2*y+1]) << 16);
				}
			nonZeroLinesMap = halfNonZeroLinesMap;

			break;
		}

	if (translatedColorTable[0].t != 0) nonZeroLinesMap = NULL;
	}

