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

#include "FrameStore.h"
#include "..\common\TimedFibers.h"
#include "library\common\vddebug.h"

#define MEASURE_SCALE_TIME		0
#define MEASURE_OSD_TIME		0

#pragma warning (disable : 4799 4731)

FrameStore::FrameStore(void)
	{
	width = height = bytesPerRow = 0;
	stripeMask = 0;
	iyb = yb = iuvb = uvb = NULL;
	precision = 8;
	}

FrameStore::FrameStore(GenericMMXYUVVideoConverter	*	yuvConverter, int width, int height, bool stripe)
	{
	this->width = width;
	this->height = height;
	this->yuvConverter = yuvConverter;

	bytesPerRow = ((width + 31) & ~31);

	if (stripe)
		mainStripeMask = 0x0000001f;
	else
		mainStripeMask = 0xffffffff;

	stripeMask = 0xffffffff;

	iyb = new BYTE[bytesPerRow * (height + 32)+ 28];
	yb = iyb + ((32 - ((int)iyb & 31)) & 31);
	iuvb = new BYTE[bytesPerRow * (height / 2 + 32) + 28];
	uvb = iuvb + ((32 - ((int)iuvb & 31)) & 31);

	precision = 8;

	maxybFrame = yb + (height - 15) * bytesPerRow;
	maxybField = yb + (height - 14) * bytesPerRow;
	}

FrameStore::~FrameStore(void)
	{
	if (iyb)  delete[] iyb;
	if (iuvb) delete[] iuvb;
	}

void FrameStore::EnableStripeMode(bool stripe)
	{
	if (stripe)
		stripeMask = mainStripeMask;
	else
		stripeMask = 0xffffffff;
	}

void FrameStore::DrawRect(int x, int y, int w, int h, unsigned yuv)
	{
	BYTE yy, uu, vv;
	int xi, yi;
	BPTR yp, uvp;
	int stride;

	yy = ((yuv >> 16) & 0xff) >> (8 - precision);
	uu = ((yuv >>  8) & 0xff) >> (8 - precision);
	vv = ((yuv      ) & 0xff) >> (8 - precision);

	GetPP(x, y, yp, uvp, stride);

	for(yi=0; yi<h; yi++)
		{
		for(xi=0; xi<w; xi+=2)
			{
			yp[xi] = yy; yp[xi+1] = yy;
			uvp[xi] = uu; uvp[xi+1] = vv;
			}
		yp += stride;
		if (yi & 1) uvp += stride;
		}
	}


static inline void ConvertLineToRGB32_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	static MMXQShort short80 = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort muluv1 = {453, 359, 453, 359};
	static MMXQShort muluv2 = {88, 183, 88, 183};


	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, w
			pxor			mm7, mm7
loop1:
			movd			mm0, [edx]
			movd			mm4, [ebx]

			punpcklbw	mm0, mm7
			punpcklbw	mm4, mm7

			psubsw		mm0, [short80]		// V3 U2 V1 U0

			psllw			mm0, 8
			movq			mm1, mm0

			pmulhw		mm0, [muluv1]

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			movq			mm0, mm2				// V3 V1 U2 U0
			punpcklwd	mm0, mm0				// U2 U2 U0 U0
			punpckhwd	mm2, mm2				// V3 V3 V1 V1

			paddsw		mm0, mm4				// Y + U	* 1.772 -> B
			paddsw		mm2, mm4				// Y + V * 1.402 -> R

			pmaddwd		mm1, [muluv2]		// v * 0.714 + u * 0.344
			psrld			mm1, 16
			movq			mm3, mm1
			pslld			mm3, 16
			por			mm1, mm3				// VU VU VU VU
			psubsw		mm4, mm1				// Y - 0.714 * V - 0.344 * U

			packuswb		mm0, mm2				// R R R R B B B B
			packuswb		mm4, mm7				// 0 0 0 0 G G G G

			movq			mm1, mm0
			punpcklbw	mm0, mm4				// G B G B G B G B
			punpckhbw	mm1, mm4				// 0 R 0 R 0 R 0 R

			movq			mm2, mm0
			punpcklwd	mm0, mm1				// 0 R G B 0 R G B
			punpckhwd	mm2, mm1

			movq			[eax], mm0
			movq			[eax + 8], mm2

			add			edx, 4
			add			ebx, 4
			add			eax, 16
			sub			ecx, 4
			jne			loop1
			}
		}
	}

static inline void ConvertLineToRGB32_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	if (w)
		{
		}
	}

static inline void ConvertLineToRGB32_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w, int s)
	{
	if (w)
		{
		}
	}

static inline void ConvertLineToRGB15_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	static MMXQShort short80 = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort muluv1 = {453, 359, 453, 359};
	static MMXQShort muluv2 = {88, 183, 88, 183};
	static MMXQShort addround = {0x7f00, 0x7f00, 0x7f00, 0x7f00};
	static MMXQShort mask15 = {0x00f8, 0x00f8, 0x00f8, 0x00f8};
	static MMXQShort maskn15 = {0x0007, 0x0007, 0x0007, 0x0007};
	static MMXQShort error = {0x4231, 0x1234, 0x1531, 0x4bdf};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, w
			pxor			mm7, mm7
			movq			mm6, [addround]
			movq			mm5, [error]
loop1:
			movd			mm0, [edx]
			movd			mm4, [ebx]

			punpcklbw	mm0, mm7

			punpcklbw	mm4, mm7
			paddsw		mm4, mm6
			movq			mm1, mm5
			pand			mm1, [maskn15]
			paddsw		mm4, mm1

			psubsw		mm0, [short80]		// V3 U2 V1 U0

			psllw			mm0, 8
			movq			mm1, mm0

			pmulhw		mm0, [muluv1]

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			movq			mm0, mm2				// V3 V1 U2 U0
			punpcklwd	mm0, mm0				// U2 U2 U0 U0
			punpckhwd	mm2, mm2				// V3 V3 V1 V1

			paddsw		mm0, mm4				// Y + U	* 1.772 -> B
			paddsw		mm2, mm4				// Y + V * 1.402 -> R

			pmaddwd		mm1, [muluv2]		// v * 0.714 + u * 0.344
			psrld			mm1, 16
			movq			mm3, mm1
			pslld			mm3, 16
			por			mm1, mm3				// VU VU VU VU
			psubsw		mm4, mm1				// Y - 0.714 * V - 0.344 * U

			psubusw		mm0, mm6
			psubusw		mm2, mm6
			psubusw		mm4, mm6

			movq			mm1, mm5
			psrlq			mm5, 1
			paddd			mm5, mm1

			pand			mm2, [mask15]
			pand			mm4, [mask15]

			psllw			mm4, 2
			psllw			mm2, 7
			psrlw			mm0, 3
			por			mm2, mm4
			por			mm0, mm2

			movq			[eax], mm0

			add			edx, 4
			add			ebx, 4
			add			eax, 8
			sub			ecx, 4
			jne			loop1

			movq			[error], mm5
			}
		}
	}


static inline void ConvertLineToRGB15ThroughOSD_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w, BYTE * po, DDWORD * dd)
	{
	static MMXQShort short80 = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort muluv1 = {453, 359, 453, 359};
	static MMXQShort muluv2 = {88, 183, 88, 183};
	static MMXQShort addround = {0x7f00, 0x7f00, 0x7f00, 0x7f00};
	static MMXQShort mask15 = {0x00f8, 0x00f8, 0x00f8, 0x00f8};
	static MMXQShort maskn15 = {0x0007, 0x0007, 0x0007, 0x0007};
	static MMXQShort error = {0x4231, 0x1234, 0x1531, 0x4bdf};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edi, [puv]
			mov			esi, [po]

			mov			ecx, [w]
			push			ebp
			mov			ebp, [dd]

			movq			mm6, [addround]
			movq			mm5, [error]
loop1:
			movd			mm0, [edi]
			movd			mm4, [ebx]

			pxor			mm7, mm7
			punpcklbw	mm0, mm7
			punpcklbw	mm4, mm7
			//
			// OSD Processing
			//

			movzx			edx, BYTE PTR [esi]
			movq			mm7, [ebp + edx * 8 + 2048]

			punpckldq	mm1, mm7
			punpckhwd	mm1, mm7

			movzx			edx, BYTE PTR [esi+1]
			movq			mm7, [ebp + edx * 8 + 2048]

			punpckldq	mm2, mm7
			punpckhwd	mm2, mm7

			movq			mm3, mm1
			punpckldq	mm3, mm2
			punpckhdq	mm1, mm2

			pmullw		mm0, mm1
			pmullw		mm4, mm3

			movzx			edx, BYTE PTR [esi]
			movq			mm7, [ebp + edx * 8]

			punpckldq	mm1, mm7
			punpckhwd	mm1, mm7

			movzx			edx, BYTE PTR [esi+1]
			movq			mm7, [ebp + edx * 8]

			punpckldq	mm2, mm7
			punpckhwd	mm2, mm7

			movq			mm3, mm1
			punpckldq	mm3, mm2
			punpckhdq	mm1, mm2

			paddw			mm0, mm1
			paddw			mm4, mm3

			psrlw			mm0, 4
			psrlw			mm4, 4

			//
			//	Post OSD Processing
			//
			paddsw		mm4, mm6
			movq			mm1, mm5
			pand			mm1, [maskn15]
			paddsw		mm4, mm1

			psubsw		mm0, [short80]		// V3 U2 V1 U0

			psllw			mm0, 8
			movq			mm1, mm0

			pmulhw		mm0, [muluv1]

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			movq			mm0, mm2				// V3 V1 U2 U0
			punpcklwd	mm0, mm0				// U2 U2 U0 U0
			punpckhwd	mm2, mm2				// V3 V3 V1 V1

			paddsw		mm0, mm4				// Y + U	* 1.772 -> B
			paddsw		mm2, mm4				// Y + V * 1.402 -> R

			pmaddwd		mm1, [muluv2]		// v * 0.714 + u * 0.344
			psrld			mm1, 16
			movq			mm3, mm1
			pslld			mm3, 16
			por			mm1, mm3				// VU VU VU VU
			psubsw		mm4, mm1				// Y - 0.714 * V - 0.344 * U

			psubusw		mm0, mm6
			psubusw		mm2, mm6
			psubusw		mm4, mm6

			movq			mm1, mm5
			psrlq			mm5, 1
			paddd			mm5, mm1

			pand			mm2, [mask15]
			pand			mm4, [mask15]

			psllw			mm4, 2
			psllw			mm2, 7
			psrlw			mm0, 3
			por			mm2, mm4
			por			mm0, mm2

			movq			[eax], mm0

			add			esi, 2
			add			edi, 4
			add			ebx, 4
			add			eax, 8
			sub			ecx, 4
			jne			loop1

			pop			ebp

			movq			[error], mm5
			}
		}
	}

static inline void ConvertLineToRGB15ThroughOSD_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w, BYTE * po, DDWORD * dd)
	{
	}

static inline void ConvertLineToRGB15ThroughOSD_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w,
																	BYTE * po, DDWORD * dd, int s)
	{
	}

static inline void ConvertLineToRGB15_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	static MMXQShort short80a = {0x0100, 0x0100, 0x0100, 0x0100};
	static MMXQShort muluv1 = {453, 359, 453, 359};
	static MMXQShort muluv2 = {88, 183, 88, 183};
	static MMXQShort addround = {0x7f00, 0x7f00, 0x7f00, 0x7f00};
	static MMXQShort mask15 = {0x00f8, 0x00f8, 0x00f8, 0x00f8};
	static MMXQShort maskn15 = {0x0007, 0x0007, 0x0007, 0x0007};
	static MMXQShort error = {0x4231, 0x1234, 0x1531, 0x4bdf};
	static MMXQShort maskY = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, w
			pxor			mm7, mm7
			movq			mm6, [addround]
			movq			mm5, [error]
loop1:
			movq			mm0, [edx]
			movq			mm4, [ebx]

			movq			mm1, [maskY]
			pand			mm1, mm4
			psrlw			mm4, 8
			paddw			mm4, mm1
			psrlw			mm4, 1

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			punpckldq	mm0, mm2
			punpckhbw	mm0, mm2

			movq			mm1, [maskY]
			pand			mm1, mm0
			psrlw			mm0, 8
			paddw			mm0, mm1

			paddsw		mm4, mm6
			movq			mm1, mm5
			pand			mm1, [maskn15]
			paddsw		mm4, mm1

			psubsw		mm0, [short80a]		// V3 U2 V1 U0

			psllw			mm0, 7
			movq			mm1, mm0

			pmulhw		mm0, [muluv1]

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			movq			mm0, mm2				// V3 V1 U2 U0
			punpcklwd	mm0, mm0				// U2 U2 U0 U0
			punpckhwd	mm2, mm2				// V3 V3 V1 V1

			paddsw		mm0, mm4				// Y + U	* 1.772 -> B
			paddsw		mm2, mm4				// Y + V * 1.402 -> R

			pmaddwd		mm1, [muluv2]		// v * 0.714 + u * 0.344
			psrld			mm1, 16
			movq			mm3, mm1
			pslld			mm3, 16
			por			mm1, mm3				// VU VU VU VU
			psubsw		mm4, mm1				// Y - 0.714 * V - 0.344 * U

			psubusw		mm0, mm6
			psubusw		mm2, mm6
			psubusw		mm4, mm6

			movq			mm1, mm5
			psrlq			mm5, 1
			paddd			mm5, mm1

			pand			mm2, [mask15]
			pand			mm4, [mask15]

			psllw			mm4, 2
			psllw			mm2, 7
			psrlw			mm0, 3
			por			mm2, mm4
			por			mm0, mm2

			movq			[eax], mm0

			add			edx, 8
			add			ebx, 8
			add			eax, 8
			sub			ecx, 4
			jne			loop1

			movq			[error], mm5
			}
		}
	}

static inline void ConvertLineToRGB15_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w, int s)
	{
	int x;
	int xs;

	int y, u, v, r, g, b, ry, gy, by;

	WORD * pp = (WORD *)pm;

	xs = 1 << s;

	for(x = 0; x < w; x += 2 * xs)
		{
		u = puv[x]   + puv[x+xs]   - 256;
		v = puv[x+1] + puv[x+xs+1] - 256;

		r =  (359 * v)          >> 9;
		g = -(183 * v + 88 * u) >> 9;
		b =  (453 * u)          >> 9;

		y = py[x];

		ry = r + y;
		gy = g + y;
		by = b + y;

		*pp++ = ((ry & 0xf8) << 7) | ((gy & 0xf8) << 2) | ((by & 0xf8) >> 3);

		y = py[x+xs];

		ry = r + y;
		gy = g + y;
		by = b + y;

		*pp++ = ((ry & 0xf8) << 7) | ((gy & 0xf8) << 2) | ((by & 0xf8) >> 3);
		}
	}

static inline void ConvertLineToRGB16_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	static MMXQShort short80 = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort muluv1 = {453, 359, 453, 359};
	static MMXQShort muluv2 = {88, 183, 88, 183};
	static MMXQShort addround = {0x7f00, 0x7f00, 0x7f00, 0x7f00};
	static MMXQShort mask15 = {0x00f8, 0x00f8, 0x00f8, 0x00f8};
	static MMXQShort maskn15 = {0x0007, 0x0007, 0x0007, 0x0007};
	static MMXQShort error = {0x4231, 0x1234, 0x1531, 0x4bdf};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, w
			pxor			mm7, mm7
			movq			mm6, [addround]
			movq			mm5, [error]
loop1:
			movd			mm0, [edx]
			movd			mm4, [ebx]

			punpcklbw	mm0, mm7

			punpcklbw	mm4, mm7
			paddsw		mm4, mm6
			movq			mm1, mm5
			pand			mm1, [maskn15]
			paddsw		mm4, mm1

			psubsw		mm0, [short80]		// V3 U2 V1 U0

			psllw			mm0, 8
			movq			mm1, mm0

			pmulhw		mm0, [muluv1]

			punpckldq	mm2, mm0
			punpckhwd	mm2, mm0

			movq			mm0, mm2				// V3 V1 U2 U0
			punpcklwd	mm0, mm0				// U2 U2 U0 U0
			punpckhwd	mm2, mm2				// V3 V3 V1 V1

			paddsw		mm0, mm4				// Y + U	* 1.772 -> B
			paddsw		mm2, mm4				// Y + V * 1.402 -> R

			pmaddwd		mm1, [muluv2]		// v * 0.714 + u * 0.344
			psrld			mm1, 16
			movq			mm3, mm1
			pslld			mm3, 16
			por			mm1, mm3				// VU VU VU VU
			psubsw		mm4, mm1				// Y - 0.714 * V - 0.344 * U

			psubusw		mm0, mm6
			psubusw		mm2, mm6
			psubusw		mm4, mm6

			movq			mm1, mm5
			psrlq			mm5, 1
			paddd			mm5, mm1

			pand			mm2, [mask15]
			pand			mm4, [mask15]

			psllw			mm4, 3
			psllw			mm2, 8
			psrlw			mm0, 3
			por			mm2, mm4
			por			mm0, mm2

			movq			[eax], mm0

			add			edx, 4
			add			ebx, 4
			add			eax, 8
			sub			ecx, 4
			jne			loop1

			movq			[error], mm5
			}
		}
	}

static inline void ConvertLineToRGB16_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	if (w)
		{
		}
	}

static inline void ConvertLineToRGB16_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w, int s)
	{
	if (w)
		{
		}
	}

void FrameStore::ConvertToYUVStripe(BYTE * topMap, BYTE * bottomMap,
			                           int mwidth, int mheight, int mstride,
												int sstart, int sheight,
												bool lace, bool singleField, PictureStructure fmode,
											   VideoDownscaleFilterLevelSet filterLevel,
												FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	BYTE * pmb, * pmt;
	GenericMMXYUVVideoConverter::ConvertStripeFunction		convertStripe;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	if (!(singleField && fmode == PS_BOTTOM_FIELD))
		{
		convertStripe = yuvConverter->convertStripes[precision - 7][osd != NULL][filterLevelX][filterLevelY];

		vcp.xfraction = xfraction;
		vcp.yfraction = yfraction;
		vcp.xscale = xscale;

		vcp.width = width;
		vcp.height = height;
		vcp.mwidth = mwidth;

		vcp.stripeMask = stripeMask;

		vcp.bpr = bytesPerRow;
		vcp.mbpr = mstride;

		if (bottomMap != NULL) lace = true;

		if (osd)
			{
			vcp.dd = osd->colorLookup;
			osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                       vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
			}

		if (yfraction != 0x10000)
			{
			switch (filterLevelY)
				{
				case VSQL_BINARY:
				case VSQL_DROP:
					break;
				case VSQL_LINEAR:
				case VSQL_FOUR_TAP:
					if (sstart > 0)
						{
						sstart -= 2;
						sheight += 2;
						}

					if (sstart + sheight < height)
						sheight -= 2;
					break;
				case VSQL_EIGHT_TAP:
					if (sstart > 0)
						{
						sstart -= 4;
						sheight += 4;
						}

					if (sstart + sheight < height)
						sheight -= 4;
					break;
				}
			}

		if (fmode == PS_FRAME_PICTURE)
			{
			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}
		else
			{
			vcp.bpr *= 2;
			vcp.obpr *= 2;

			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}

		vcp.py = yb;
		vcp.puv = uvb;

		if (fmode == PS_BOTTOM_FIELD)
			{
			vcp.py += bytesPerRow;
			vcp.puv += bytesPerRow;
			}

		if (fmode == PS_FRAME_PICTURE && lace && !singleField)
			{
			vcp.ystart >>= 1;
			vcp.ystop >>= 1;
			vcp.yosdstart >>= 1;
			vcp.yosdstop >>= 1;

			if (bottomMap)
				pmb = bottomMap + vcp.ystart * vcp.mbpr;
			else
				{
				pmb = topMap + (2 * vcp.ystart + 1) * vcp.mbpr;
				vcp.mbpr *= 2;
				}
			pmt = topMap + vcp.ystart * vcp.mbpr;

			vcp.ycount = vcp.ystart * yfraction;
			vcp.bpr *= 2;
			vcp.obpr *= 2;
			vcp.stripeMask >>= 1;

			vcp.pm = pmt;
			(yuvConverter->*convertStripe)(vcp);
			vcp.pm = pmb;
			vcp.py += bytesPerRow;
			vcp.puv += bytesPerRow;
			vcp.po += vcp.obpr >> 1;
			(yuvConverter->*convertStripe)(vcp);
			}
		else
			{
			if (lace && singleField)
				{
				vcp.yfraction >>= 1;
				vcp.yosdstart >>= 1;
				vcp.yosdstop >>= 1;

				if (fmode == PS_FRAME_PICTURE)
					{
					vcp.bpr *= 2;
					vcp.obpr *= 2;
					vcp.stripeMask >>= 1;
					}
				else
					{
					vcp.ystart *= 2;
					vcp.ystop *= 2;
					}

				vcp.pm = topMap + vcp.ystart * vcp.mbpr;
				}
			else
				{
				switch (fmode)
					{
					case PS_FRAME_PICTURE:
						vcp.pm  = topMap + vcp.ystart * vcp.mbpr;
						break;
					case PS_TOP_FIELD:
						if (!bottomMap) vcp.mbpr *= 2;
						vcp.pm  = topMap + vcp.ystart * vcp.mbpr;
						break;
					case PS_BOTTOM_FIELD:
						if (bottomMap)
							vcp.pm  = bottomMap + vcp.ystart * vcp.mbpr;
						else
							{
							vcp.mbpr *= 2;
							vcp.pm = topMap + mstride + vcp.ystart * vcp.mbpr;
							}
						break;
					}
				}

			vcp.ycount = vcp.ystart * vcp.yfraction;

			(yuvConverter->*convertStripe)(vcp);
			}
		}
	}

void FrameStore::ConvertToYUVStripePanScan(BYTE * topMap, BYTE * bottomMap,
			                                  int mwidth, int mheight, int mstride, int xoffset,
												       int sstart, int sheight,
												       bool lace, bool singleField, PictureStructure fmode,
												       VideoDownscaleFilterLevelSet filterLevel,
												       FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	BYTE * pmb, * pmt;
	GenericMMXYUVVideoConverter::ConvertStripeFunction		convertStripe;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	if (!(singleField && fmode == PS_BOTTOM_FIELD))
		{
		if (osd != NULL)
			convertStripe = GenericMMXYUVVideoConverter::ConvertStripeToYUVPSThroughOSD_P8F2;
		else
			convertStripe = GenericMMXYUVVideoConverter::ConvertStripeToYUVPS_P8F2;

		vcp.xfraction = xfraction;
		vcp.yfraction = yfraction;
		vcp.xscale = xscale;
		vcp.xoffset = xoffset;

		vcp.width = width;
		vcp.height = height;
		vcp.mwidth = mwidth;

		vcp.stripeMask = stripeMask;

		vcp.bpr = bytesPerRow;
		vcp.mbpr = mstride;

		if (bottomMap != NULL) lace = true;

		if (osd)
			{
			vcp.dd = osd->colorLookup;
			osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                       vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
			}

		if (yfraction != 0x10000)
			{
			switch (filterLevelY)
				{
				case VSQL_BINARY:
				case VSQL_DROP:
					break;
				case VSQL_LINEAR:
				case VSQL_FOUR_TAP:
					if (sstart > 0)
						{
						sstart -= 2;
						sheight += 2;
						}

					if (sstart + sheight < height)
						sheight -= 2;
					break;
				case VSQL_EIGHT_TAP:
					if (sstart > 0)
						{
						sstart -= 4;
						sheight += 4;
						}

					if (sstart + sheight < height)
						sheight -= 4;
					break;
				}
			}

		if (fmode == PS_FRAME_PICTURE)
			{
			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}
		else
			{
			vcp.bpr *= 2;
			vcp.obpr *= 2;

			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}

		vcp.py = yb;
		vcp.puv = uvb;

		if (fmode == PS_BOTTOM_FIELD)
			{
			vcp.py += bytesPerRow;
			vcp.puv += bytesPerRow;
			}

		if (fmode == PS_FRAME_PICTURE && lace && !singleField)
			{
			vcp.ystart >>= 1;
			vcp.ystop >>= 1;
			vcp.yosdstart >>= 1;
			vcp.yosdstop >>= 1;

			if (bottomMap)
				pmb = bottomMap + vcp.ystart * vcp.mbpr;
			else
				{
				pmb = topMap + (2 * vcp.ystart + 1) * vcp.mbpr;
				vcp.mbpr *= 2;
				}
			pmt = topMap + vcp.ystart * vcp.mbpr;

			vcp.ycount = vcp.ystart * yfraction;
			vcp.bpr *= 2;
			vcp.obpr *= 2;
			vcp.stripeMask >>= 1;

			vcp.pm = pmt;
			(yuvConverter->*convertStripe)(vcp);
			vcp.pm = pmb;
			vcp.py += bytesPerRow;
			vcp.puv += bytesPerRow;
			vcp.po += vcp.obpr >> 1;
			(yuvConverter->*convertStripe)(vcp);
			}
		else
			{
			if (lace && singleField)
				{
				vcp.yfraction >>= 1;
				vcp.yosdstart >>= 1;
				vcp.yosdstop >>= 1;

				if (fmode == PS_FRAME_PICTURE)
					{
					vcp.bpr *= 2;
					vcp.obpr *= 2;
					vcp.stripeMask >>= 1;
					}
				else
					{
					vcp.ystart *= 2;
					vcp.ystop *= 2;
					}

				vcp.pm = topMap + vcp.ystart * vcp.mbpr;
				}
			else
				{
				switch (fmode)
					{
					case PS_FRAME_PICTURE:
						vcp.pm  = topMap + vcp.ystart * vcp.mbpr;
						break;
					case PS_TOP_FIELD:
						if (!bottomMap) vcp.mbpr *= 2;
						vcp.pm  = topMap + vcp.ystart * vcp.mbpr;
						break;
					case PS_BOTTOM_FIELD:
						if (bottomMap)
							vcp.pm  = bottomMap + vcp.ystart * vcp.mbpr;
						else
							{
							vcp.mbpr *= 2;
							vcp.pm = topMap + mstride + vcp.ystart * vcp.mbpr;
							}
						break;
					}
				}

			vcp.ycount = vcp.ystart * vcp.yfraction;

			(yuvConverter->*convertStripe)(vcp);
			}
		}
	}

void FrameStore::ConvertToYUVStripePanScanAverageDeinterlace(BYTE * topMap,
			                                                    int mwidth, int mheight, int mstride, int xoffset,
												                         int sstart, int sheight,
												                         bool lace,
												                         VideoDownscaleFilterLevelSet filterLevel,
												                         FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xoffset = xoffset;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;

	vcp.pm  = topMap + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	if (lace)
		{
		if (osd)
			yuvConverter->ConvertStripeToYUVPSAIThroughOSD_P8F2(vcp);
		else
			yuvConverter->ConvertStripeToYUVPSAI_P8F2(vcp);
		}
	else
		{
		if (osd)
			yuvConverter->ConvertStripeToYUVPSThroughOSD_P8F2(vcp);
		else
			yuvConverter->ConvertStripeToYUVPS_P8F2(vcp);
		}
	}

void FrameStore::ConvertToYUVStripePanScanDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
                                                      int mwidth, int mheight, int mstride, int xoffset,
															         int sstart, int sheight,
															         bool topFieldFirst, DWORD flags,
															         VideoDownscaleFilterLevelSet filterLevel,
															         FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xoffset = xoffset;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;
	vcp.topField = topFieldFirst;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	if (topFieldFirst)
		{
		vcp.py = yb;
		vcp.puv = uvb;
		vcp.ppy = previous->yb;
		vcp.ppuv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.pm  = currentMap + vcp.ystart * mstride;
		vcp.ppm = previousMap + vcp.ystart * mstride;
		vcp.pmt = currentMap + vcp.ystart * mstride;
		vcp.pmb = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}
	else
		{
		vcp.ppy = yb;
		vcp.ppuv = uvb;
		vcp.py = previous->yb;
		vcp.puv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.ppm = currentMap + vcp.ystart * mstride;
		vcp.pm  = previousMap + vcp.ystart * mstride;
		vcp.pmb = currentMap + vcp.ystart * mstride;
		vcp.pmt = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVPSDI2ThroughOSD_P8F2(vcp);
	else
		yuvConverter->ConvertStripeToYUVPSDI2_P8F2(vcp);
	}

void FrameStore::ConvertToYUVStripePanScanInterpolate(BYTE * map, FrameStore * previous,
			                                             int mwidth, int mheight, int mstride, int xoffset,
												                  int sstart, int sheight,
												                  VideoDownscaleFilterLevelSet filterLevel,
												                  FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xoffset = xoffset;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;
	vcp.ppy = previous->yb;
	vcp.ppuv = previous->uvb;

	vcp.pm = map + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVPSA2ThroughOSD_P8F2(vcp);
	else
		yuvConverter->ConvertStripeToYUVPSA2_P8F2(vcp);
	}

void FrameStore::ConvertToYUVStripePanScanDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                                    int mwidth, int mheight, int mstride, int xoffset,
																             int sstart, int sheight,
																             DWORD flags,
																             VideoDownscaleFilterLevelSet filterLevel,
																             FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xoffset = xoffset;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;

	vcp.pmt = topMap + vcp.ystart * mstride;
	vcp.pmb = bottomMap + vcp.ystart * mstride;

	vcp.deinterlaceFlags = flags;

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVPSSI2ThroughOSD_P8F2(vcp);
	else
		yuvConverter->ConvertStripeToYUVPSSI2_P8F2(vcp);
	}

void FrameStore::ConvertToYUVStripeLetterbox(BYTE * topMap, BYTE * bottomMap,
			                                    int mwidth, int mheight, int mstride,
												         int sstart, int sheight,
												         bool lace, bool singleField, PictureStructure fmode,
											            VideoDownscaleFilterLevelSet filterLevel,
												         FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	BYTE * pmt, * pmb, * pm;
	int bpr, mbpr, obpr;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	int sstop;

	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	if (!(singleField && fmode == PS_BOTTOM_FIELD))
		{
		bpr = bytesPerRow;
		mbpr = mstride;

		if (bottomMap != NULL) lace = true;

		if (osd)
			{
			vcp.dd = osd->colorLookup;
			osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
											  vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, obpr);
			}

		if (yfraction != 0x10000)
			{
			switch (filterLevelY)
				{
				case VSQL_BINARY:
				case VSQL_DROP:
					break;
				case VSQL_LINEAR:
				case VSQL_FOUR_TAP:
					if (sstart > 0)
						{
						sstart -= 2;
						sheight += 2;
						}

					if (sstart + sheight < height)
						sheight -= 2;
					break;
				case VSQL_EIGHT_TAP:
					if (sstart > 0)
						{
						sstart -= 4;
						sheight += 4;
						}

					if (sstart + sheight < height)
						sheight -= 4;
					break;
				}
			}

		vcp.lboffset = height >> 3;

		sstop = sstart + sheight;
		if (sstart != 0)
			{
			sstart = ((sstart * 3) >> 2) + vcp.lboffset;
			}

		if (sstop != height)
			{
			sstop = ((sstop * 3) >> 2) + vcp.lboffset;
			}
		sheight = sstop - sstart;

		if (fmode == PS_FRAME_PICTURE)
			{
			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}
		else
			{
			bpr *= 2;
			obpr *= 2;

			vcp.ystart = sstart * mheight / height;
			vcp.ystop = (sstart + sheight) * mheight / height;
			}


		vcp.width = width;
		vcp.xscale = xscale;
		vcp.py = yb;
		vcp.puv = uvb;

		if (fmode == PS_BOTTOM_FIELD)
			{
			vcp.py += bytesPerRow;
			vcp.puv += bytesPerRow;
			}

		if (fmode == PS_FRAME_PICTURE && lace && !singleField)
			{
			vcp.ystart >>= 1;
			vcp.ystop >>= 1;
			vcp.yosdstart >>= 1;
			vcp.yosdstop >>= 1;
			vcp.lboffset >>= 1;

			if (bottomMap)
				pmb = bottomMap + vcp.ystart * mbpr;
			else
				{
				pmb = topMap + (2 * vcp.ystart + 1) * mbpr;
				mbpr *= 2;
				}
			pmt = topMap + vcp.ystart * mbpr;

			vcp.height = height >> 1;
			vcp.pm = pmt;
			vcp.bpr = bpr * 2;
			vcp.mbpr = mbpr;
			vcp.obpr = 2 * obpr;
			vcp.ycount = vcp.ystart * yfraction;
			vcp.yfraction = yfraction;
			vcp.stripeMask = stripeMask >> 1;

			if (osd)
				yuvConverter->ConvertStripeToYUVLBThroughOSD_P8DSY2(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB_P8DSY2(vcp);

			vcp.py += bpr;
			vcp.puv += bpr;
			vcp.pm = pmb;
			vcp.po += obpr;

			if (osd)
				yuvConverter->ConvertStripeToYUVLBThroughOSD_P8DSY2(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB_P8DSY2(vcp);
			}
		else
			{
			if (lace && singleField)
				{
				yfraction >>= 1;
				vcp.yosdstart >>= 1;
				vcp.yosdstop >>= 1;

				if (fmode == PS_FRAME_PICTURE)
					{
					bpr *= 2;
					obpr *= 2;
					stripeMask >>= 1;
					}
				else
					{
					vcp.ystart *= 2;
					vcp.ystop *= 2;
					}

				pm = topMap + vcp.ystart * mbpr;
				}
			else
				{
				switch (fmode)
					{
					case PS_FRAME_PICTURE:
						pm  = topMap + vcp.ystart * mbpr;
						break;
					case PS_TOP_FIELD:
						if (!bottomMap) mbpr *= 2;
						pm  = topMap + vcp.ystart * mbpr;
						break;
					case PS_BOTTOM_FIELD:
						if (bottomMap)
							pm  = bottomMap + vcp.ystart * mbpr;
						else
							{
							mbpr *= 2;
							pm = topMap + mstride + vcp.ystart * mbpr;
							}
						break;
					}
				}

			vcp.height = height;
			vcp.pm = pm;
			vcp.bpr = bpr;
			vcp.mbpr = mbpr;
			vcp.obpr = obpr;
			vcp.ycount = vcp.ystart * yfraction;
			vcp.yfraction = yfraction;
			vcp.stripeMask = stripeMask;

			if (osd)
				yuvConverter->ConvertStripeToYUVLBThroughOSD_P8DSY2(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB_P8DSY2(vcp);
			}
		}
	}

void FrameStore::ConvertToYUVStripeLetterboxAverageDeinterlace(BYTE * topMap,
																					int mwidth, int mheight, int mstride,
																					int sstart, int sheight,
																					bool lace,
																					VideoDownscaleFilterLevelSet filterLevel,
																					FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	BYTE * pm;
	int bpr, mbpr, obpr;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	int sstop;

	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	bpr = bytesPerRow;
	mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
										  vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.lboffset = height >> 3;

	sstop = sstart + sheight;
	if (sstart != 0)
		{
		sstart = ((sstart * 3) >> 2) + vcp.lboffset;
		}

	if (sstop != height)
		{
		sstop = ((sstop * 3) >> 2) + vcp.lboffset;
		}
	sheight = sstop - sstart;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.width = width;
	vcp.xscale = xscale;
	vcp.py = yb;
	vcp.puv = uvb;

	pm = topMap + vcp.ystart * mbpr;

	vcp.height = height;
	vcp.pm = pm;
	vcp.bpr = bpr;
	vcp.mbpr = mbpr;
	vcp.obpr = obpr;
	vcp.ycount = vcp.ystart * yfraction;
	vcp.yfraction = yfraction;
	vcp.stripeMask = stripeMask;

	if (lace)
		{
		if (osd)
			yuvConverter->ConvertStripeToYUVLBAIThroughOSD_P8DSY2(vcp);
		else
			yuvConverter->ConvertStripeToYUVLBAI_P8DSY2(vcp);
		}
	else
		{
		if (osd)
			yuvConverter->ConvertStripeToYUVLBThroughOSD_P8DSY2(vcp);
		else
			yuvConverter->ConvertStripeToYUVLB_P8DSY2(vcp);
		}
	}

void FrameStore::ConvertToYUVStripeLetterboxDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
                                                        int mwidth, int mheight, int mstride,
															           int sstart, int sheight,
															           bool topFieldFirst, DWORD flags,
															           VideoDownscaleFilterLevelSet filterLevel,
															           FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	int sstop;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;
	vcp.topField = topFieldFirst;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.lboffset = height >> 3;

	sstop = sstart + sheight;
	if (sstart != 0)
		{
		sstart = ((sstart * 3) >> 2) + vcp.lboffset;
		}

	if (sstop != height)
		{
		sstop = ((sstop * 3) >> 2) + vcp.lboffset;
		}
	sheight = sstop - sstart;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	if (topFieldFirst)
		{
		vcp.py = yb;
		vcp.puv = uvb;
		vcp.ppy = previous->yb;
		vcp.ppuv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.pm  = currentMap + vcp.ystart * mstride;
		vcp.ppm = previousMap + vcp.ystart * mstride;
		vcp.pmt = currentMap + vcp.ystart * mstride;
		vcp.pmb = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}
	else
		{
		vcp.ppy = yb;
		vcp.ppuv = uvb;
		vcp.py = previous->yb;
		vcp.puv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.ppm = currentMap + vcp.ystart * mstride;
		vcp.pm  = previousMap + vcp.ystart * mstride;
		vcp.pmb = currentMap + vcp.ystart * mstride;
		vcp.pmt = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVLBDI2ThroughOSD_P8DS(vcp);
	else
		yuvConverter->ConvertStripeToYUVLBDI2_P8DS(vcp);
	}

void FrameStore::ConvertToYUVStripeLetterboxInterpolate(BYTE * map, FrameStore * previous,
			                                               int mwidth, int mheight, int mstride,
												                    int sstart, int sheight,
												                    VideoDownscaleFilterLevelSet filterLevel,
												                    FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	int sstop;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	vcp.lboffset = height >> 3;

	sstop = sstart + sheight;
	if (sstart != 0)
		{
		sstart = ((sstart * 3) >> 2) + vcp.lboffset;
		}

	if (sstop != height)
		{
		sstop = ((sstop * 3) >> 2) + vcp.lboffset;
		}
	sheight = sstop - sstart;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;
	vcp.ppy = previous->yb;
	vcp.ppuv = previous->uvb;

	vcp.pm = map + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVLBA2ThroughOSD_P8DS(vcp);
	else
		yuvConverter->ConvertStripeToYUVLBA2_P8DS(vcp);
	}

void FrameStore::ConvertToYUVStripeLetterboxDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                                      int mwidth, int mheight, int mstride,
																               int sstart, int sheight,
																               DWORD flags,
																               VideoDownscaleFilterLevelSet filterLevel,
																               FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	int sstop;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.lboffset = height >> 3;

	sstop = sstart + sheight;
	if (sstart != 0)
		{
		sstart = ((sstart * 3) >> 2) + vcp.lboffset;
		}

	if (sstop != height)
		{
		sstop = ((sstop * 3) >> 2) + vcp.lboffset;
		}
	sheight = sstop - sstart;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;

	vcp.pmt = topMap + vcp.ystart * mstride;
	vcp.pmb = bottomMap + vcp.ystart * mstride;

	vcp.deinterlaceFlags = flags;

	vcp.ycount = vcp.ystart * yfraction;

	if (osd)
		yuvConverter->ConvertStripeToYUVLBSI2ThroughOSD_P8DS(vcp);
	else
		yuvConverter->ConvertStripeToYUVLBSI2_P8DS(vcp);
	}

void FrameStore::ConvertToYUVStripeLetterboxSingleInverse32Pulldown(BYTE * topMap, FrameStore * previous,
			                                                           int mwidth, int mheight, int mstride,
												                                int sstart, int sheight,
												                                Inverse32PulldownHint phint,
												                                VideoDownscaleFilterLevelSet filterLevel,
												                                FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	enum {CTY_FRAME, CTY_FRAME_FIELD, CTY_FIELD_FIELD} ctype;
	int sstop;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.lboffset = height >> 3;

	sstop = sstart + sheight;
	if (sstart != 0)
		{
		sstart = ((sstart * 3) >> 2) + vcp.lboffset;
		}

	if (sstop != height)
		{
		sstop = ((sstop * 3) >> 2) + vcp.lboffset;
		}
	sheight = sstop - sstart;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	switch (phint)
		{
		case IPDH_FRAME_FRAME1:
		case IPDH_FRAME_FRAME2:
			ctype = CTY_FRAME;
			vcp.py = yb;
			vcp.puv = uvb;
			break;
		case IPDH_TOP_FRAME:
			ctype = CTY_FRAME_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			vcp.topField = true;
			break;
		case IPDH_BOTTOM_FRAME:
			ctype = CTY_FRAME_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			vcp.topField = false;
			break;
		case IPDH_BOTTOM_TOP:
			ctype = CTY_FIELD_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			break;
		case IPDH_TOP_BOTTOM:
			ctype = CTY_FIELD_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			break;
		case IPDH_FRAME_TOP:
			ctype = CTY_FRAME_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			vcp.topField = true;
			break;
		case IPDH_FRAME_BOTTOM:
			ctype = CTY_FRAME_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			vcp.topField = false;
			break;
		}

	vcp.pm  = topMap + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;


	switch (ctype)
		{
		case CTY_FRAME:
			if (osd)
				yuvConverter->ConvertStripeToYUVLBThroughOSD_P8DSY2(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB_P8DSY2(vcp);
			break;
		case CTY_FRAME_FIELD:
			if (osd)
				yuvConverter->ConvertStripeToYUVLB32PDFrameFieldThroughOSD_P8DS(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB32PDFrameField_P8DS(vcp);
			break;
		case CTY_FIELD_FIELD:
			if (osd)
				yuvConverter->ConvertStripeToYUVLB32PDFieldFieldThroughOSD_P8DS(vcp);
			else
				yuvConverter->ConvertStripeToYUVLB32PDFieldField_P8DS(vcp);
			break;
		}
	}

void FrameStore::ConvertToYUVStripeAverageDeinterlace(BYTE * topMap,
			                                             int mwidth, int mheight, int mstride,
												                  int sstart, int sheight,
												                  bool lace,
												                  VideoDownscaleFilterLevelSet filterLevel,
												                  FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;

	vcp.pm  = topMap + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	if (lace)
		{
		switch (filterLevelY)
			{
			case VSQL_BINARY:
			case VSQL_DROP:
				switch (filterLevelX)
					{
					case VSQL_BINARY:
						if (osd)
							yuvConverter->ConvertStripeToYUVAIThroughOSD_P8DS(vcp);
						else
							yuvConverter->ConvertStripeToYUVAI_P8DS(vcp);
						break;
					case VSQL_DROP:
					case VSQL_LINEAR:
					case VSQL_FOUR_TAP:
					case VSQL_EIGHT_TAP:
						if (osd)
							yuvConverter->ConvertStripeToYUVAIThroughOSD_P8F2(vcp);
						else
							yuvConverter->ConvertStripeToYUVAI_P8F2(vcp);
						break;
					}
				break;
			case VSQL_LINEAR:
			case VSQL_FOUR_TAP:
			case VSQL_EIGHT_TAP:
				switch (filterLevelX)
					{
					case VSQL_BINARY:
						if (osd)
							yuvConverter->ConvertStripeToYUVAIThroughOSD_P8DSY2(vcp);
						else
							yuvConverter->ConvertStripeToYUVAI_P8DSY2(vcp);
						break;
					case VSQL_DROP:
					case VSQL_LINEAR:
					case VSQL_FOUR_TAP:
					case VSQL_EIGHT_TAP:
						if (osd)
							yuvConverter->ConvertStripeToYUVAIThroughOSD_P8F2Y2(vcp);
						else
							yuvConverter->ConvertStripeToYUVAI_P8F2Y2(vcp);
						break;
					}
				break;
			}
		}
	else
		{
		switch (filterLevelX)
			{
			case VSQL_BINARY:
				if (osd)
					yuvConverter->ConvertStripeToYUVThroughOSD_P8DS(vcp);
				else
					yuvConverter->ConvertStripeToYUV_P8DS(vcp);
				break;
			case VSQL_DROP:
			case VSQL_LINEAR:
			case VSQL_FOUR_TAP:
			case VSQL_EIGHT_TAP:
				if (osd)
					yuvConverter->ConvertStripeToYUVThroughOSD_P8F2(vcp);
				else
					yuvConverter->ConvertStripeToYUV_P8F2(vcp);
				break;
			}
		}
	}

void FrameStore::ConvertToYUVStripeSingleDeinterlace(BYTE * topMap, FrameStore * previous,
			                                            int mwidth, int mheight, int mstride,
												                 int sstart, int sheight,
												                 bool topField,
												                 VideoDownscaleFilterLevelSet filterLevel,
												                 FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;
	vcp.topField = topField;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;
	vcp.ppy = previous->yb;

	vcp.pm  = topMap + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	switch (filterLevelY)
		{
		case VSQL_BINARY:
		case VSQL_DROP:
			switch (filterLevelX)
				{
				case VSQL_BINARY:
					if (osd)
						yuvConverter->ConvertStripeToYUVDIThroughOSD_P8DS(vcp);
					else
						yuvConverter->ConvertStripeToYUVDI_P8DS(vcp);
					break;
				case VSQL_DROP:
				case VSQL_LINEAR:
				case VSQL_FOUR_TAP:
				case VSQL_EIGHT_TAP:
					if (osd)
						yuvConverter->ConvertStripeToYUVDIThroughOSD_P8F2(vcp);
					else
						yuvConverter->ConvertStripeToYUVDI_P8F2(vcp);
					break;
				}
			break;
		case VSQL_LINEAR:
		case VSQL_FOUR_TAP:
		case VSQL_EIGHT_TAP:
			switch (filterLevelX)
				{
				case VSQL_BINARY:
					if (osd)
						yuvConverter->ConvertStripeToYUVDIThroughOSD_P8DSY2(vcp);
					else
						yuvConverter->ConvertStripeToYUVDI_P8DSY2(vcp);
					break;
				case VSQL_DROP:
				case VSQL_LINEAR:
				case VSQL_FOUR_TAP:
				case VSQL_EIGHT_TAP:
					if (osd)
						yuvConverter->ConvertStripeToYUVDIThroughOSD_P8F2Y2(vcp);
					else
						yuvConverter->ConvertStripeToYUVDI_P8F2Y2(vcp);
					break;
				}
			break;
		}
	}

void FrameStore::ConvertToYUVStripeSingleInverse32Pulldown(BYTE * topMap, FrameStore * previous,
			                                                    int mwidth, int mheight, int mstride,
												                         int sstart, int sheight,
												                         Inverse32PulldownHint phint,
												                         VideoDownscaleFilterLevelSet filterLevel,
												                         FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;
	enum {CTY_FRAME, CTY_FRAME_FIELD, CTY_FIELD_FIELD} ctype;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	switch (phint)
		{
		case IPDH_FRAME_FRAME1:
		case IPDH_FRAME_FRAME2:
			ctype = CTY_FRAME;
			vcp.py = yb;
			vcp.puv = uvb;
			break;
		case IPDH_TOP_FRAME:
			ctype = CTY_FRAME_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			vcp.topField = true;
			break;
		case IPDH_BOTTOM_FRAME:
			ctype = CTY_FRAME_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			vcp.topField = false;
			break;
		case IPDH_BOTTOM_TOP:
			ctype = CTY_FIELD_FIELD;
			vcp.py = yb;
			vcp.puv = uvb;
			vcp.ppy = previous->yb;
			vcp.ppuv = previous->uvb;
			break;
		case IPDH_TOP_BOTTOM:
			ctype = CTY_FIELD_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			break;
		case IPDH_FRAME_TOP:
			ctype = CTY_FRAME_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			vcp.topField = true;
			break;
		case IPDH_FRAME_BOTTOM:
			ctype = CTY_FRAME_FIELD;
			vcp.py = previous->yb;
			vcp.puv = previous->uvb;
			vcp.ppy = yb;
			vcp.ppuv = uvb;
			vcp.topField = false;
			break;
		}

	vcp.pm  = topMap + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;


	switch (filterLevelX)
		{
		case VSQL_BINARY:
			switch (ctype)
				{
				case CTY_FRAME:
					if (osd)
						yuvConverter->ConvertStripeToYUVThroughOSD_P8DS(vcp);
					else
						yuvConverter->ConvertStripeToYUV_P8DS(vcp);
					break;
				case CTY_FRAME_FIELD:
					if (osd)
						yuvConverter->ConvertStripeToYUV32PDFrameFieldThroughOSD_P8DS(vcp);
					else
						yuvConverter->ConvertStripeToYUV32PDFrameField_P8DS(vcp);
					break;
				case CTY_FIELD_FIELD:
					if (osd)
						yuvConverter->ConvertStripeToYUV32PDFieldFieldThroughOSD_P8DS(vcp);
					else
						yuvConverter->ConvertStripeToYUV32PDFieldField_P8DS(vcp);
					break;
				}
		case VSQL_DROP:
		case VSQL_LINEAR:
		case VSQL_FOUR_TAP:
		case VSQL_EIGHT_TAP:
			switch (ctype)
				{
				case CTY_FRAME:
					if (osd)
						yuvConverter->ConvertStripeToYUVThroughOSD_P8F2(vcp);
					else
						yuvConverter->ConvertStripeToYUV_P8F2(vcp);
					break;
				case CTY_FRAME_FIELD:
					if (osd)
						yuvConverter->ConvertStripeToYUV32PDFrameFieldThroughOSD_P8F2(vcp);
					else
						yuvConverter->ConvertStripeToYUV32PDFrameField_P8F2(vcp);
					break;
				case CTY_FIELD_FIELD:
					if (osd)
						yuvConverter->ConvertStripeToYUV32PDFieldFieldThroughOSD_P8F2(vcp);
					else
						yuvConverter->ConvertStripeToYUV32PDFieldField_P8F2(vcp);
					break;
				}
		}
	}

void FrameStore::ConvertToYUVStripeDeinterlace(BYTE * previousMap, BYTE * currentMap, BYTE * nextMap, FrameStore * previous,
                                               int mwidth, int mheight, int mstride,
															  int sstart, int sheight,
															  bool topFieldFirst, DWORD flags,
															  VideoDownscaleFilterLevelSet filterLevel,
															  FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;
	vcp.topField = topFieldFirst;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	if (topFieldFirst)
		{
		vcp.py = yb;
		vcp.puv = uvb;
		vcp.ppy = previous->yb;
		vcp.ppuv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.pm  = currentMap + vcp.ystart * mstride;
		vcp.ppm = previousMap + vcp.ystart * mstride;
		vcp.pmt = currentMap + vcp.ystart * mstride;
		vcp.pmb = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}
	else
		{
		vcp.ppy = yb;
		vcp.ppuv = uvb;
		vcp.py = previous->yb;
		vcp.puv = previous->uvb;
		vcp.pytb = yb;
		vcp.puvtb = uvb;

		vcp.ppm = currentMap + vcp.ystart * mstride;
		vcp.pm  = previousMap + vcp.ystart * mstride;
		vcp.pmb = currentMap + vcp.ystart * mstride;
		vcp.pmt = nextMap + vcp.ystart * mstride;

		vcp.deinterlaceFlags = flags;
		}

	vcp.ycount = vcp.ystart * yfraction;

	switch (filterLevelX)
		{
		case VSQL_BINARY:
			if (osd)
				yuvConverter->ConvertStripeToYUVDI2ThroughOSD_P8DS(vcp);
			else
				yuvConverter->ConvertStripeToYUVDI2_P8DS(vcp);
			break;
		case VSQL_DROP:
		case VSQL_LINEAR:
		case VSQL_FOUR_TAP:
		case VSQL_EIGHT_TAP:
			if (osd)
				yuvConverter->ConvertStripeToYUVDI2ThroughOSD_P8F2(vcp);
			else
				yuvConverter->ConvertStripeToYUVDI2_P8F2(vcp);
			break;
		}
	}

void FrameStore::ConvertToYUVStripeDeinterlaceStretch(BYTE * topMap, BYTE * bottomMap,
			                                               int mwidth, int mheight, int mstride,
																        int sstart, int sheight,
																        DWORD flags,
																        VideoDownscaleFilterLevelSet filterLevel,
																        FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	if (sstart > 0)
		{
		sstart -= 2;
		sheight += 2;
		}

	if (sstart + sheight < height)
		sheight -= 2;

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;

	vcp.pmt = topMap + vcp.ystart * mstride;
	vcp.pmb = bottomMap + vcp.ystart * mstride;

	vcp.deinterlaceFlags = flags;

	vcp.ycount = vcp.ystart * yfraction;

	switch (filterLevelX)
		{
		case VSQL_BINARY:
			if (osd)
				yuvConverter->ConvertStripeToYUVSI2ThroughOSD_P8DS(vcp);
			else
				yuvConverter->ConvertStripeToYUVSI2_P8DS(vcp);
			break;
		case VSQL_DROP:
		case VSQL_LINEAR:
		case VSQL_FOUR_TAP:
		case VSQL_EIGHT_TAP:
			if (osd)
				yuvConverter->ConvertStripeToYUVSI2ThroughOSD_P8F2(vcp);
			else
				yuvConverter->ConvertStripeToYUVSI2_P8F2(vcp);
			break;
		}
	}

void FrameStore::ConvertToYUVStripeInterpolate(BYTE * map, FrameStore * previous,
			                                        int mwidth, int mheight, int mstride,
												             int sstart, int sheight,
												             VideoDownscaleFilterLevelSet filterLevel,
												             FullWidthMMXSPUDisplayBuffer * osd)
	{
	int xscale, yfraction, xfraction;
	VideoDownscaleFilterLevel	filterLevelX, filterLevelY;
	YUVVideoConverterParameter  vcp;

	mwidth = (mwidth + 3) & ~3;

	xfraction = 0x10000 * width / mwidth;
	yfraction = 0x10000 * height / mheight;
	xscale = 0;
	while ((mwidth << xscale) < width) xscale++;

	if (yfraction > 0x20000)
		filterLevelY = filterLevel.halfY;
	else
		filterLevelY = filterLevel.fullY;

	if (xfraction > 0x20000)
		filterLevelX = filterLevel.halfX;
	else
		filterLevelX = filterLevel.fullX;

	vcp.xfraction = xfraction;
	vcp.yfraction = yfraction;
	vcp.xscale = xscale;

	vcp.width = width;
	vcp.height = height;
	vcp.mwidth = mwidth;

	vcp.stripeMask = stripeMask;

	vcp.bpr = bytesPerRow;
	vcp.mbpr = mstride;

	if (osd)
		{
		vcp.dd = osd->colorLookup;
		osd->GetDisplayParameters(width, height, vcp.xosdstart, vcp.xosdstop, vcp.yosdstart, vcp.yosdstop,
				                    vcp.osdNonZeroMap, vcp.yosdoffset, vcp.po, vcp.obpr);
		}

	vcp.ystart = sstart * mheight / height;
	vcp.ystop = (sstart + sheight) * mheight / height;

	vcp.py = yb;
	vcp.puv = uvb;
	vcp.ppy = previous->yb;
	vcp.ppuv = previous->uvb;

	vcp.pm = map + vcp.ystart * mstride;

	vcp.ycount = vcp.ystart * yfraction;

	switch (filterLevelX)
		{
		case VSQL_BINARY:
			if (osd)
				yuvConverter->ConvertStripeToYUVA2ThroughOSD_P8DS(vcp);
			else
				yuvConverter->ConvertStripeToYUVA2_P8DS(vcp);
			break;
		case VSQL_DROP:
		case VSQL_LINEAR:
		case VSQL_FOUR_TAP:
		case VSQL_EIGHT_TAP:
			if (osd)
				yuvConverter->ConvertStripeToYUVA2ThroughOSD_P8F2(vcp);
			else
				yuvConverter->ConvertStripeToYUVA2_P8F2(vcp);
			break;
		}
	}

void FrameStore::ConvertToRGBStripe(int depth, BYTE * map,
			                 int mwidth, int mheight, int mstride,
								  int sstart, int sheight, bool lace, PictureStructure fmode,
								  BYTE * bottomMap)
	{
	switch (depth)
		{
		case 15:
			ConvertToRGB15Stripe(map, mwidth, mheight, mstride,
								      sstart, sheight, lace, fmode, bottomMap);
			break;
		case 16:
			ConvertToRGB16Stripe(map, mwidth, mheight, mstride,
								      sstart, sheight, lace, fmode, bottomMap);
			break;
		case 32:
			ConvertToRGB32Stripe(map, mwidth, mheight, mstride,
								      sstart, sheight, lace, fmode, bottomMap);
			break;
		}
	}

void FrameStore::ConvertToRGBThroughOSDStripe(int depth, BYTE * map, FullWidthMMXSPUDisplayBuffer * osd,
			                                 int mwidth, int mheight, int mstride,
														int sstart, int sheight,
														bool lace, PictureStructure fmode,
														BYTE * bottomMap)
	{
	switch (depth)
		{
		case 15:
			ConvertToRGB15ThroughOSDStripe(map, osd, mwidth, mheight, mstride,
													 sstart, sheight, lace, fmode, bottomMap);
			break;
		}
	}

void FrameStore::ConvertToRGB32Stripe(BYTE * map,
			                 int mwidth, int mheight, int mstride,
								  int sstart, int sheight, bool lace, PictureStructure fmode,
								  BYTE * bottomMap)
	{
	BYTE * py, * puv, * pm, * pmt, * pmb;
	int mbpr, ybpr, uvbpr;
	int xscale, yscale, ystep;
	int ystart, ystop;
	int w;
	int y, bpr;

	xscale = 0;
	while (mwidth << xscale < width) xscale++;
	yscale = 0;
	while (mheight << yscale < height) yscale++;

	if (!yscale || fmode != PS_BOTTOM_FIELD)
		{
		bpr = bytesPerRow;

		if (fmode != PS_FRAME_PICTURE) bpr *= 2;

		ybpr = bpr << yscale;
		if (yscale)
			uvbpr = bpr << (yscale - 1);
		else
			uvbpr = 0;
		mbpr = mstride;

		ystart = sstart;
		ystop = ystart + sheight;

		w = width;

		ystep = 1 << yscale;

		py  = yb + bpr * (ystart & stripeMask);
		puv = uvb + bpr * ((ystart & stripeMask) >> 1);

		if (fmode == PS_BOTTOM_FIELD)
			{
			py += bytesPerRow;
			puv += bytesPerRow;
			}

		if (lace && fmode == PS_FRAME_PICTURE && !yscale)
			{
			pmt = map + (ystart >> 1) * mstride;
			if (bottomMap)
				pmb = bottomMap + (ystart >> 1) * mstride;
			else
				pmb = pmt + (mheight >> 1) * mstride;

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB32_P8D1(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB32_P8D1(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB32_P8D2(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB32_P8D2(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB32_P8DS(py, puv + bytesPerRow, pmb, w, xscale);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB32_P8DS(py, puv, pmt, w, xscale);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					}
				}
			}
		else
			{
			switch (fmode)
				{
				case PS_FRAME_PICTURE:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_TOP_FIELD:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_BOTTOM_FIELD:
					if (bottomMap)
						pm  = bottomMap + (ystart >> yscale) * mstride;
					else
						pm  = map + ((ystart + (mheight >> 1)) >> yscale) * mstride;
					break;
				}

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB32_P8D1(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB32_P8D2(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB32_P8DS(py, puv, pm, w, xscale);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					}
				}
			}
		}
	}

	//
	// map     : Base pointer of display memory
   // osd     : OSD bitmap to alpha blend with
   // mwidth  : width of display buffer
   // mheight : height of display buffer (both fields)
   // mstride : distance between two lines of the same field (or
   //           frame in non laced mode)
   // sstart  : slice start in source coordinates
   // sheight : slice height in source coordinates
   // lace    : target buffer resembles interlaced format
   // fmode   : Structure of source picture
   //
void FrameStore::ConvertToRGB15ThroughOSDStripe(BYTE * map, FullWidthMMXSPUDisplayBuffer * osd,
		                                  int mwidth, int mheight, int mstride,
		 										    int sstart, int sheight,
													 bool lace, PictureStructure fmode, BYTE * bottomMap)
	{
	BYTE * py, * puv, * pm, * po, * pmt, *pmb;
	DDWORD * dd = osd->colorLookup;
	int mbpr, ybpr, uvbpr, obpr;
	int xscale, yscale, ystep;
	int ystart, ystop, yosdstart, yosdstop;
	int xosdstart, xosdstop;
	int w, lw, ow, rw;
	int y, bpr, oobpr;

	//
	// Calculate scaling factors based on binary scale using
	// frame buffer size an video display buffer size
	//
	xscale = 0;
	while (mwidth << xscale < width) xscale++;
	yscale = 0;
	while (mheight << yscale < height) yscale++;

	//
	// If picture is in native size, or not the second field
	// of field encoded material
	//
	if (!yscale || fmode != PS_BOTTOM_FIELD)
		{
		//
		// Cache bytes per row of frame buffer and osd buffer
		//
		bpr = bytesPerRow;
		oobpr = osd->bytesPerRow;

		//
		// If the source material is not frame based, we only
		// have a single field so we work with double strides
		// on the source side.
		//
		if (fmode != PS_FRAME_PICTURE)
			{
			bpr *= 2;
			oobpr *= 2;
			if (yscale) yscale--;
			}

		//
		// Calculate source and osd bytes per row based on scaling
		// factor
		//
		ybpr = bpr << yscale;
		if (yscale)
			uvbpr = bpr << (yscale - 1);
		else
			uvbpr = 0;
		obpr = oobpr << yscale;

		//
		// Target bytes per row is always the given one, because
		// we have to fill the display buffer completely
		//
		mbpr = mstride;

		//
		// Calculate first line of and first line after update
		// region
		//
		ystart = sstart;
		ystop = ystart + sheight;

		//
		// Clip OSD region into update region
		//
		yosdstart = osd->nonTransparentTop;
		if (yosdstart < ystart) yosdstart = ystart;
		else if (yosdstart > ystop) yosdstart = ystop;
		yosdstop = osd->nonTransparentTop + osd->nonTransparentHeight;
		if (yosdstop > ystop) yosdstop = ystop;

		//
		// Calculate nonscaled horizontal source coordinates
		//
		xosdstart = osd->left;
		xosdstop = osd->left + osd->width;

		w = width;
		lw = xosdstart;
		ow = xosdstop - xosdstart;
		rw = width - xosdstop;

		//
		// Step in source frame buffer based on scaling
		//
		ystep = 1 << yscale;

		//
		// Calculate source pointers
		//
		py  = yb + bpr * (ystart & stripeMask);
		puv = uvb + bpr * ((ystart & stripeMask) >> 1);
		po  = osd->image + oobpr * (yosdstart - osd->top);

		//
		// If this is the bottom field in a field encoded picture
		// we have to go one line down to reach the first source
		// line of interest
		//
		if (fmode == PS_BOTTOM_FIELD)
			{
			py += bytesPerRow;
			puv += bytesPerRow;
			po += osd->bytesPerRow;
			}

		//
		// If the frame buffer is split, and the source mode is
		// frame based, and we are not vertically subsampling,
		// then we have to use the split transfer routine.
		//
		if (lace && fmode == PS_FRAME_PICTURE && !yscale)
			{
			//
			// Calculate target address
			//
			pmt = map + (ystart >> 1) * mstride;
			if (bottomMap)
				pmb = bottomMap + (ystart >> 1) * mstride;
			else
				pmb = pmt + (mheight >> 1) * mstride;

			switch (xscale)
				{
				case 0:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D1(py, puv + bytesPerRow, pmb, w);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D1(py, puv, pmt, w);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D1(py, puv + bytesPerRow, pmb, lw);
							ConvertLineToRGB15ThroughOSD_P8D1(py + xosdstart, puv + bytesPerRow + xosdstart, pmb + (xosdstart << 1), ow, po, dd);
							ConvertLineToRGB15_P8D1(py + xosdstop, puv + bytesPerRow + xosdstop, pmb + (xosdstop << 1), rw);

							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D1(py, puv, pmt, lw);
							ConvertLineToRGB15ThroughOSD_P8D1(py + xosdstart, puv + xosdstart, pmt + (xosdstart << 1), ow, po, dd);
							ConvertLineToRGB15_P8D1(py + xosdstop, puv + xosdstop, pmt + (xosdstop << 1), rw);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						po  += obpr;
						}
					for(; y < ystop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D1(py, puv + bytesPerRow, pmb, w);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D1(py, puv, pmt, w);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					break;
				case 1:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D2(py, puv + bytesPerRow, pmb, w);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D2(py, puv, pmt, w);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D2(py, puv + bytesPerRow, pmb, lw);
							ConvertLineToRGB15ThroughOSD_P8D2(py + xosdstart, puv + bytesPerRow + xosdstart, pmb + xosdstart, ow, po, dd);
							ConvertLineToRGB15_P8D2(py + xosdstop, puv + bytesPerRow + xosdstop, pmb + xosdstop, rw);

							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D2(py, puv, pmt, lw);
							ConvertLineToRGB15ThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pmt + xosdstart, ow, po, dd);
							ConvertLineToRGB15_P8D2(py + xosdstop, puv + xosdstop, pmt + xosdstop, rw);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						po  += obpr;
						}
					for(; y < ystop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8D2(py, puv + bytesPerRow, pmb, w);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8D2(py, puv, pmt, w);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					break;
				default:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8DS(py, puv + bytesPerRow, pmb, w, xscale);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8DS(py, puv, pmt, w, xscale);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8DS(py, puv + bytesPerRow, pmb, lw, xscale);
							ConvertLineToRGB15ThroughOSD_P8DS(py + xosdstart, puv + bytesPerRow + xosdstart, pmb + (xosdstart >> (xscale - 1)), ow, po, dd, xscale);
							ConvertLineToRGB15_P8DS(py + xosdstop, puv + bytesPerRow + xosdstop, pmb + (xosdstop >> (xscale - 1)), rw, xscale);

							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8DS(py, puv, pmt, lw, xscale);
							ConvertLineToRGB15ThroughOSD_P8DS(py + xosdstart, puv + xosdstart, pmt + (xosdstart >> (xscale - 1)), ow, po, dd, xscale);
							ConvertLineToRGB15_P8DS(py + xosdstop, puv + xosdstop, pmt + (xosdstop >> (xscale - 1)), rw, xscale);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						po  += obpr;
						}
					for(; y < ystop; y += ystep)
						{
						if (y & 1)
							{
							ConvertLineToRGB15_P8DS(py, puv + bytesPerRow, pmb, w, xscale);
							pmb  += mbpr;
							if (y & 2)
								{
								puv += 2 * bytesPerRow;
								}
							}
						else
							{
							ConvertLineToRGB15_P8DS(py, puv, pmt, w, xscale);
							pmt  += mbpr;
							}

						py  += ybpr;
						puv += uvbpr;
						}
					break;
				}
			}
		else
			{
			switch (fmode)
				{
				case PS_FRAME_PICTURE:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_TOP_FIELD:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_BOTTOM_FIELD:
					if (bottomMap)
						pm  = bottomMap + (ystart >> yscale) * mstride;
					else
						pm  = map + ((ystart + (mheight >> 1)) >> yscale) * mstride;
					break;
				}

			switch (xscale)
				{
				case 0:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						ConvertLineToRGB15_P8D1(py, puv, pm, w);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						ConvertLineToRGB15_P8D1(py, puv, pm, lw);
						ConvertLineToRGB15ThroughOSD_P8D1(py + xosdstart, puv + xosdstart, pm + (xosdstart << 1), ow, po, dd);
						ConvertLineToRGB15_P8D1(py + xosdstop, puv + xosdstop, pm + (xosdstop << 1), rw);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						po  += obpr;
						if (y & 1) puv += bpr;
						}
					for(; y < ystop; y += ystep)
						{
						ConvertLineToRGB15_P8D1(py, puv, pm, w);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					break;
				case 1:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						ConvertLineToRGB15_P8D2(py, puv, pm, w);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						ConvertLineToRGB15_P8D2(py, puv, pm, lw);
						ConvertLineToRGB15ThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pm + xosdstart, ow, po, dd);
						ConvertLineToRGB15_P8D2(py + xosdstop, puv + xosdstop, pm + xosdstop, rw);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						po  += obpr;
						if (y & 1) puv += bpr;
						}
					for(; y < ystop; y += ystep)
						{
						ConvertLineToRGB15_P8D2(py, puv, pm, w);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					break;
				default:
					for(y = ystart; y < yosdstart; y += ystep)
						{
						ConvertLineToRGB15_P8DS(py, puv, pm, w, xscale);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					for(; y < yosdstop; y += ystep)
						{
						ConvertLineToRGB15_P8DS(py, puv, pm, lw, xscale);
						ConvertLineToRGB15ThroughOSD_P8DS(py + xosdstart, puv + xosdstart, pm + (xosdstart >> (xscale - 1)), ow, po, dd, xscale);
						ConvertLineToRGB15_P8DS(py + xosdstop, puv + xosdstop, pm + (xosdstop >> (xscale - 1)), rw, xscale);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						po  += obpr;
						if (y & 1) puv += bpr;
						}
					for(; y < ystop; y += ystep)
						{
						ConvertLineToRGB15_P8DS(py, puv, pm, w, xscale);

						py  += ybpr;
						puv += uvbpr;
						pm  += mbpr;
						if (y & 1) puv += bpr;
						}
					break;
				}
			}
		}
	}

void FrameStore::ConvertToRGB15Stripe(BYTE * map,
			                 int mwidth, int mheight, int mstride,
								  int sstart, int sheight, bool lace, PictureStructure fmode,
								  BYTE * bottomMap)
	{
	BYTE * py, * puv, * pm, * pmt, * pmb;
	int mbpr, ybpr, uvbpr;
	int xscale, yscale, ystep;
	int ystart, ystop;
	int w;
	int y, bpr;

	xscale = 0;
	while (mwidth << xscale < width) xscale++;
	yscale = 0;
	while (mheight << yscale < height) yscale++;

	if (!yscale || fmode != PS_BOTTOM_FIELD)
		{
		bpr = bytesPerRow;

		if (fmode != PS_FRAME_PICTURE) bpr *= 2;

		ybpr = bpr << yscale;
		if (yscale)
			uvbpr = bpr << (yscale - 1);
		else
			uvbpr = 0;
		mbpr = mstride;

		ystart = sstart;
		ystop = ystart + sheight;

		w = width;

		ystep = 1 << yscale;

		py  = yb + bpr * (ystart & stripeMask);
		puv = uvb + bpr * ((ystart & stripeMask) >> 1);

		if (fmode == PS_BOTTOM_FIELD)
			{
			py += bytesPerRow;
			puv += bytesPerRow;
			}

		if (lace && fmode == PS_FRAME_PICTURE && !yscale)
			{
			pmt = map + (ystart >> 1) * mstride;
			if (bottomMap)
				pmb = bottomMap + (ystart >> 1) * mstride;
			else
				pmb = pmt + (mheight >> 1) * mstride;

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB15_P8D1(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB15_P8D1(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB15_P8D2(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB15_P8D2(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB15_P8DS(py, puv + bytesPerRow, pmb, w, xscale);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB15_P8DS(py, puv, pmt, w, xscale);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					}
				}
			}
		else
			{
			switch (fmode)
				{
				case PS_FRAME_PICTURE:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_TOP_FIELD:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_BOTTOM_FIELD:
					if (bottomMap)
						pm  = bottomMap + (ystart >> yscale) * mstride;
					else
						pm  = map + ((ystart + (mheight >> 1)) >> yscale) * mstride;
					break;
				}

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB15_P8D1(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB15_P8D2(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB15_P8DS(py, puv, pm, w, xscale);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					}
				}
			}
		}
	}

void FrameStore::ConvertToRGB16Stripe(BYTE * map,
			                 int mwidth, int mheight, int mstride,
								  int sstart, int sheight, bool lace, PictureStructure fmode,
								  BYTE * bottomMap)
	{
	BYTE * py, * puv, * pm, * pmt, * pmb;
	int mbpr, ybpr, uvbpr;
	int xscale, yscale, ystep;
	int ystart, ystop;
	int w;
	int y, bpr;

	xscale = 0;
	while (mwidth << xscale < width) xscale++;
	yscale = 0;
	while (mheight << yscale < height) yscale++;

	if (!yscale || fmode != PS_BOTTOM_FIELD)
		{
		bpr = bytesPerRow;

		if (fmode != PS_FRAME_PICTURE) bpr *= 2;

		ybpr = bpr << yscale;
		if (yscale)
			uvbpr = bpr << (yscale - 1);
		else
			uvbpr = 0;
		mbpr = mstride;

		ystart = sstart;
		ystop = ystart + sheight;

		w = width;

		ystep = 1 << yscale;

		py  = yb + bpr * (ystart & stripeMask);
		puv = uvb + bpr * ((ystart & stripeMask) >> 1);

		if (fmode == PS_BOTTOM_FIELD)
			{
			py += bytesPerRow;
			puv += bytesPerRow;
			}

		if (lace && fmode == PS_FRAME_PICTURE && !yscale)
			{
			pmt = map + (ystart >> 1) * mstride;
			if (bottomMap)
				pmb = bottomMap + (ystart >> 1) * mstride;
			else
				pmb = pmt + (mheight >> 1) * mstride;

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB16_P8D1(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB16_P8D1(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB16_P8D2(py, puv + bytesPerRow, pmb, w);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB16_P8D2(py, puv, pmt, w);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							if (y & 1)
								{
								ConvertLineToRGB16_P8DS(py, puv + bytesPerRow, pmb, w, xscale);
								pmb  += mbpr;
								if (y & 2)
									{
									puv += 2 * bytesPerRow;
									}
								}
							else
								{
								ConvertLineToRGB16_P8DS(py, puv, pmt, w, xscale);
								pmt  += mbpr;
								}

							py  += ybpr;
							puv += uvbpr;
							}
						break;
					}
				}
			}
		else
			{
			switch (fmode)
				{
				case PS_FRAME_PICTURE:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_TOP_FIELD:
					pm  = map + (ystart >> yscale) * mstride;
					break;
				case PS_BOTTOM_FIELD:
					if (bottomMap)
						pm  = bottomMap + (ystart >> yscale) * mstride;
					else
						pm  = map + ((ystart + (mheight >> 1)) >> yscale) * mstride;
					break;
				}

			if (precision == 8)
				{
				switch (xscale)
					{
					case 0:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB16_P8D1(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					case 1:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB16_P8D2(py, puv, pm, w);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					default:
						for(y = ystart; y < ystop; y += ystep)
							{
							ConvertLineToRGB16_P8DS(py, puv, pm, w, xscale);

							py  += ybpr;
							puv += uvbpr;
							pm  += mbpr;
							if (y & 1) puv += bpr;
							}
						break;
					}
				}
			}
		}
	}


static inline int GetFrameDistance(BYTE * fp1, BYTE * fp2, int stride)
	{
	int diff;

	__asm
		{
		mov			edi, [fp1]
		mov			esi, [fp2]
		mov			eax, [stride]
		mov			ecx, 16
		pxor			mm6, mm6
		pxor			mm7, mm7
loop1:
		movq			mm0, [edi]
		movq			mm1, [esi]
		movq			mm3, [edi+8]
		movq			mm4, [esi+8]
		add			edi, eax
		add			esi, eax
		dec			ecx

		movq			mm2, mm0
		psubusb		mm0, mm1
		psubusb		mm1, mm2
		por			mm0, mm1
		movq			mm1, mm0
		punpcklbw	mm0, mm7
		punpckhbw	mm1, mm7
		pmaddwd		mm0, mm0
		pmaddwd		mm1, mm1
		paddd			mm6, mm0
		paddd			mm6, mm1

		movq			mm5, mm3
		psubusb		mm3, mm4
		psubusb		mm4, mm5
		por			mm3, mm4
		movq			mm4, mm3
		punpcklbw	mm3, mm7
		punpckhbw	mm4, mm7
		pmaddwd		mm3, mm3
		pmaddwd		mm4, mm4
		paddd			mm6, mm0
		paddd			mm6, mm1

		jne			loop1

		movq			mm0, mm6
		psrlq			mm0, 32
		paddd			mm0, mm6
		movd			[diff], mm0
		}

	return diff;
	}


static inline int GetFrameFieldDistance(BYTE * feven, BYTE * fodd, int stride)
	{
	int sumdouble, sumsingle;
	int diff;

	__asm
		{
		mov			edi, [feven]
		mov			esi, [fodd]
		mov			eax, [stride]
		mov			ecx, 8

		pxor			mm5, mm5
		pxor			mm6, mm6
		pxor			mm7, mm7
loop1:
		movq			mm0, [edi]
		movq			mm1, [esi]
		movq			mm2, [edi+eax]
		add			edi, eax
		add			esi, eax
		dec			ecx

		movq			mm3, mm0
		movq			mm4, mm2
		psubusb		mm3, mm2
		psubusb		mm4, mm0
		por			mm3, mm4

		movq			mm4, mm1
		psubusb		mm4, mm0
		psubusb		mm0, mm1
		por			mm4, mm0

		movq			mm0, mm2
		psubusb		mm0, mm1
		psubusb		mm1, mm2
		por			mm1, mm0

		paddusb		mm1, mm4

		movq			mm0, mm1
		punpcklbw	mm1, mm7
		punpckhbw	mm0, mm7
		paddw			mm5, mm1
		paddw			mm5, mm0

		movq			mm0, mm3
		punpcklbw	mm3, mm7
		punpckhbw	mm0, mm7
		paddw			mm6, mm3
		paddw			mm6, mm0

		jne			loop1

		movq			mm2, mm5
		movq			mm3, mm6
		punpcklwd	mm5, mm7
		punpcklwd	mm6, mm7
		punpckhwd	mm2, mm7
		punpckhwd	mm3, mm7
		paddd			mm5, mm2
		paddd			mm6, mm3

		movq			mm2, mm5
		movq			mm3, mm6

		psrlq			mm2, 32
		psrlq			mm3, 32

		paddd			mm5, mm2
		paddd			mm6, mm3

		movd			[sumsingle], mm5
		movd			[sumdouble], mm6
		}

	diff = (sumsingle * 4 - sumdouble * 5);

	return diff;
	}

static inline int Get32PulldownFrameDistance(BYTE * fp1, BYTE * fp2, int bpr, int width, int height)
	{
	bpr *= 2;
	int diff = 0;
	int d = bpr * (height >> 4);
	int dw = (width >> 3) & ~15;

	diff += GetFrameDistance(fp1 + 2 * d + 2 * dw, fp2 + 2 * d + 2 * dw, bpr);
	diff += GetFrameDistance(fp1 + 6 * d + 2 * dw, fp2 + 6 * d + 2 * dw, bpr);
	diff += GetFrameDistance(fp1 + 2 * d + 6 * dw, fp2 + 2 * d + 6 * dw, bpr);
	diff += GetFrameDistance(fp1 + 6 * d + 6 * dw, fp2 + 6 * d + 6 * dw, bpr);

	diff += GetFrameDistance(fp1 + 3 * d + 3 * dw, fp2 + 3 * d + 3 * dw, bpr);
	diff += GetFrameDistance(fp1 + 5 * d + 3 * dw, fp2 + 5 * d + 3 * dw, bpr);
	diff += GetFrameDistance(fp1 + 3 * d + 5 * dw, fp2 + 3 * d + 5 * dw, bpr);
	diff += GetFrameDistance(fp1 + 5 * d + 5 * dw, fp2 + 5 * d + 5 * dw, bpr);

	return diff;
	}

#define THRESHOLD32	100000

bool FrameStore::Detect32PulldownFrame(FrameStore * previousFrame, Inverse32PulldownHint & phint, DWORD & history)
	{
	BYTE * fp1, * fp2, * ip1, * ip2;
	int diff, i;
	int l1, l2;

	switch (phint)
		{
		case IPDH_UNKNOWN:
			fp1 = yb;
			fp2 = previousFrame->yb;
			ip1 = yb + bytesPerRow;
			ip2 = previousFrame->yb + bytesPerRow;

			diff = Get32PulldownFrameDistance(ip1, ip2, bytesPerRow, width, height) -
					 Get32PulldownFrameDistance(fp1, fp2, bytesPerRow, width, height);

			history <<= 2;
			if (diff < -THRESHOLD32)
				history |= 0;
			else if (diff < 0)
				history |= 1;
			else if (diff < THRESHOLD32)
				history |= 2;
			else
				history |= 3;

			l1 = l2 = 256;
			for(i=0; i<30; i+=10)
				{
				switch ((history >> (i + 0)) & 3)
					{
					case 0:
						l2 = 0;
						break;
					case 1:
						l1 = l1 >> 1;
						l2 = 0;
						break;
					case 2:
						l2 = l2 >> 1;
						l1 = 0;
						break;
					case 3:
						l1 = 0;
						break;
					}
				switch ((history >> (i + 4)) & 3)
					{
					case 0:
						l1 = 0;
						break;
					case 1:
						l2 = l2 >> 1;
						l1 = 0;
						break;
					case 2:
						l1 = l1 >> 1;
						l2 = 0;
						break;
					case 3:
						l2 = 0;
						break;
					}
				}

				{
				char buffer[100];
				wsprintf(buffer, "Diff %8d l1 %3d l2 %3d\n", diff, l1, l2);
				OutputDebugString(buffer);
				}

			return FALSE;
			break;
		case IPDH_FRAME_FRAME1:
		case IPDH_FRAME_FRAME2:
		case IPDH_BOTTOM_TOP:
		case IPDH_TOP_BOTTOM:
			return TRUE;
			break;
		case IPDH_TOP_FRAME:
		case IPDH_FRAME_TOP:
			fp1 = yb;
			fp2 = previousFrame->yb;
			ip1 = yb + bytesPerRow;
			ip2 = previousFrame->yb + bytesPerRow;
			break;
		case IPDH_BOTTOM_FRAME:
		case IPDH_FRAME_BOTTOM:
			fp1 = yb + bytesPerRow;
			fp2 = previousFrame->yb + bytesPerRow;
			ip1 = yb;
			ip2 = previousFrame->yb;
			break;
		}

	diff = Get32PulldownFrameDistance(ip1, ip2, bytesPerRow, width, height) -
		    Get32PulldownFrameDistance(fp1, fp2, bytesPerRow, width, height);

	return diff > 0;
	}

bool FrameStore::Is32PulldownFrame(FrameStore * previousFrame, Inverse32PulldownHint phint)
	{
	BYTE * fp1, * fp2, * ip1, * ip2;

	switch (phint)
		{
		case IPDH_UNKNOWN:
			return FALSE;
			break;
		case IPDH_FRAME_FRAME1:
		case IPDH_FRAME_FRAME2:
		case IPDH_BOTTOM_TOP:
		case IPDH_TOP_BOTTOM:
			return TRUE;
			break;
		case IPDH_TOP_FRAME:
		case IPDH_FRAME_TOP:
			fp1 = yb;
			fp2 = previousFrame->yb;
			ip1 = yb + bytesPerRow;
			ip2 = previousFrame->yb + bytesPerRow;
			break;
		case IPDH_BOTTOM_FRAME:
		case IPDH_FRAME_BOTTOM:
			fp1 = yb + bytesPerRow;
			fp2 = previousFrame->yb + bytesPerRow;
			ip1 = yb;
			ip2 = previousFrame->yb;
			break;
		}

	int diff = 0;
	int bpr = 2 * bytesPerRow;
	int d = bpr * (height >> 4);
	int dw = (width >> 3) & ~15;

	diff += GetFrameDistance(fp1 + 2 * d + 2 * dw, fp2 + 2 * d + 2 * dw, bpr);
	diff += GetFrameDistance(fp1 + 6 * d + 2 * dw, fp2 + 6 * d + 2 * dw, bpr);
	diff += GetFrameDistance(fp1 + 2 * d + 6 * dw, fp2 + 2 * d + 6 * dw, bpr);
	diff += GetFrameDistance(fp1 + 6 * d + 6 * dw, fp2 + 6 * d + 6 * dw, bpr);

	diff += GetFrameDistance(fp1 + 3 * d + 3 * dw, fp2 + 3 * d + 3 * dw, bpr);
	diff += GetFrameDistance(fp1 + 5 * d + 3 * dw, fp2 + 5 * d + 3 * dw, bpr);
	diff += GetFrameDistance(fp1 + 3 * d + 5 * dw, fp2 + 3 * d + 5 * dw, bpr);
	diff += GetFrameDistance(fp1 + 5 * d + 5 * dw, fp2 + 5 * d + 5 * dw, bpr);

	diff -= GetFrameDistance(ip1 + 2 * d + 2 * dw, ip2 + 2 * d + 2 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 6 * d + 2 * dw, ip2 + 6 * d + 2 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 2 * d + 6 * dw, ip2 + 2 * d + 6 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 6 * d + 6 * dw, ip2 + 6 * d + 6 * dw, bpr);

	diff -= GetFrameDistance(ip1 + 3 * d + 3 * dw, ip2 + 3 * d + 3 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 5 * d + 3 * dw, ip2 + 5 * d + 3 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 3 * d + 5 * dw, ip2 + 3 * d + 5 * dw, bpr);
	diff -= GetFrameDistance(ip1 + 5 * d + 5 * dw, ip2 + 5 * d + 5 * dw, bpr);

	return diff <= 0;
	}
void FrameStore::ConvertToRGB(BYTE * map)
	{
	int x, y;
	int cr, cg, cb;
	int cy, cu, cv;
	BYTE * py, * puv, * pm;
	int dbpr = (width * 3 + 3) & ~ 3;
	int shl = 8 - precision;

	py = yb;
	puv = uvb;
	pm = map + height * dbpr;

	for(y = 0; y<height; y++)
		{
		pm -= dbpr;
		for(x = 0; x<width / 2; x++)
			{
			cy = (*py++ << shl);
			cu = (int)(*puv++ << shl) - 128;
			cv = (int)(*puv++ << shl) - 128;

			cr = (cy*1000 + 1402 * cv) / 1000;
			cg = (cy*1000 -  714 * cv -  344*cu) / 1000;
			cb = (cy*1000 + 1772 * cu) / 1000;

			if (cr > 255) cr = 255; else if (cr<0) cr = 0;
			if (cg > 255) cg = 255; else if (cg<0) cg = 0;
			if (cb > 255) cb = 255; else if (cb<0) cb = 0;

			*pm++ = (BYTE)cb;
			*pm++ = (BYTE)cg;
			*pm++ = (BYTE)cr;

			cy = (*py++ << shl);

			cr = (cy*1000 + 1402 * cv) / 1000;
			cg = (cy*1000 -  714 * cv -  344*cu) / 1000;
			cb = (cy*1000 + 1772 * cu) / 1000;

			if (cr > 255) cr = 255; else if (cr<0) cr = 0;
			if (cg > 255) cg = 255; else if (cg<0) cg = 0;
			if (cb > 255) cb = 255; else if (cb<0) cb = 0;

			*pm++ = (BYTE)cb;
			*pm++ = (BYTE)cg;
			*pm++ = (BYTE)cr;
			}
		pm -= width * 3;
		if (!(y & 1))
			{
			puv -= width;
			}
		}
	}

TiledFrameStore::TiledFrameStore(GenericMMXYUVVideoConverter	*	yuvConverter, int width, int height, bool stripe)
	{
	this->width = width;
	this->height = height;
	this->yuvConverter = yuvConverter;

	bytesPerRow = ((width + 31) & ~31);
	bytesPerTile = ((height + 7) & ~7) * 8;

	stripeMask = 0xffffffff;

	iyb = new BYTE[bytesPerTile * (width >> 3) + 31];
	yb = iyb + ((32 - ((int)iyb & 31)) & 31);
	iuvb = new BYTE[(bytesPerTile >> 1) * (width >> 3) + 31];
	uvb = iuvb + ((32 - ((int)iuvb & 31)) & 31);
	precision = 8;

	maxybFrame = yb + width * bytesPerTile - 8 * 15;
	maxybField = yb + width * bytesPerTile - 8 * 14;
	}

#pragma warning (default : 4799 4731)
