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

#include "..\common\MMXExtensionMacros.h"
#include "XMMXYUVConverter.h"
#include "..\common\TimedFibers.h"

#pragma warning (disable : 4799 4731)


#define MEASURE_SCALE_TIME				0
#define MEASURE_XSCALE_TIME			0
#define MEASURE_DEINTERLACE_TIME		0

extern MMXShort<4> sinctab[128];
extern MMXShort<4> sinctabxSet[16][128];
extern MMXShort<4> sinctaby[128];
extern MMXShort<4> sinctabySet[16][128];


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// XMMX optimized versions of the YUV converter routines
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//
// Convert a single Y line and a pair of UV lines into a YUY2 line
// by averaging the UV lines
//
static inline void ConvertLineToYUV_P8D1MUV(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]

	loop2:
			movq			mm0, [ebx]

			movq			mm1, [edx]
			movq			mm2, [edx+2*esi]

			pavgb			(mm1, mm2)

			movq			mm2, mm0
			punpcklbw	mm0, mm1
			punpckhbw	mm2, mm1

			movq			[eax], mm0

			movq			[eax+8], mm2

			add			ebx, 8
			add			edx, 8
			add			eax, 16
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D1MY(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]

	loop2:
			movq			mm1, [edx]

			movq			mm0, [ebx]
			movq			mm2, [ebx+2*esi]

			pavgb			(mm0, mm2)

			movq			mm2, mm0
			punpcklbw	mm0, mm1
			punpckhbw	mm2, mm1

			movq			[eax], mm0

			movq			[eax+8], mm2

			add			ebx, 8
			add			edx, 8
			add			eax, 16
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1MY) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D1MYUV(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr)	// XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]

	loop2:
			movq			mm0, [ebx]
			movq			mm2, [ebx+2*esi]

			pavgb			(mm0, mm2)

			movq			mm1, [edx]
			movq			mm2, [edx+2*esi]

			pavgb			(mm1, mm2)

			movq			mm2, mm0
			punpcklbw	mm0, mm1
			punpckhbw	mm2, mm1

			movq			[eax], mm0

			movq			[eax+8], mm2

			add			ebx, 8
			add			edx, 8
			add			eax, 16
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1MYUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}





static inline void ConvertLineToYUVThroughOSD_P8D1MUV(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};

	__asm
		{
		xor			ecx, ecx
		mov			esi, [po]
		mov			ebx, [pm]
		mov			edx, [puv]

		movq			mm6, [mask7]
		pxor			mm7, mm7
loop2:
		mov			edi, [py]
		movq			mm0, [edi+ecx]

		mov			edi, [bpr]
		movq			mm1, [edx]
		movq			mm2, [edx + 2 * edi]

		pavgb			(mm1, mm2)

		movq			mm2, mm0
		punpcklbw	mm0, mm1
		punpckhbw	mm2, mm1

		movq			mm1, mm0
		punpcklbw	mm0, mm7
		punpckhbw	mm1, mm7

		movq			mm3, mm2
		punpcklbw	mm2, mm7
		punpckhbw	mm3, mm7

		mov			edi, [dd]

		movzx			eax, BYTE PTR [esi]
		pmullw		mm0, [edi + eax * 8 + 2048]
		movq			mm4, [edi + eax * 8]
		paddw			mm0, mm4
		psrlw			mm0, 4

		movzx			eax, [esi+1]
		pmullw		mm1, [edi + eax * 8 + 2048]
		movq			mm5, [edi + eax * 8]
		paddw			mm1, mm5
		psrlw			mm1, 4

		packuswb		mm0, mm1
		movq			[ebx+2*ecx], mm0

		movzx			eax, BYTE PTR [esi+2]
		pmullw		mm2, [edi + eax * 8 + 2048]
		movq			mm5, [edi + eax * 8]
		paddw			mm2, mm5

		movzx			eax, BYTE PTR [esi+3]

		pmullw		mm3, [edi + eax * 8 + 2048]
		paddw			mm3, [edi + eax * 8]
		psrlw			mm2, 4
		psrlw			mm3, 4
		packuswb		mm2, mm3

		movq			[ebx+2*ecx+8], mm2

		add			esi, 4
		add			edx, 8
		add			ecx, 8
		cmp			ecx, [w]
		jne			loop2
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}



///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with binary x scaling and no y scaling
//

//
// Convert a pair of single Y and UV into a YUY2 line, downscaling horizontaly
// by a factor of two
//
static inline void ConvertLineToYUV_P8D2MUV(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]
			movq			mm6, [masklow]

	loop2:
			movq			mm0, [ebx]
			movq			mm1, [edx]
			movq			mm2, [edx+2*esi]

			movq			mm3, mm0
			pand			mm3, mm6
			psrlw			mm0, 8
			pavgb			(mm0, mm3)


			pxor			mm7, mm7
			pavgb			(mm1, mm2)
			pshufw		(mm3, mm1, 0xB1)
			pavgb			(mm3, mm1)
			pshufw		(mm1, mm3, 0xD0)
			punpckhbw   mm7, mm1

			por			mm0, mm7

			movq			[eax], mm0

			add			ebx, 8
			add			edx, 8
			add			eax, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D2MYUV(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr)	// XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]
			movq			mm6, [masklow]
			pxor			mm7, mm7

	loop2:


			movq			mm0, [ebx]
			movq			mm4, [ebx+2*esi]
			movq			mm1, [edx]
			movq			mm2, [edx+2*esi]

			pavgb			(mm0, mm4)
			movq			mm3, mm0
			psllq			mm0, 8
			pavgb			(mm0, mm3)
			psrlw			mm0, 8

			pxor			mm7, mm7
			pavgb			(mm1, mm2)
			pshufw		(mm3, mm1, 0xB1)
			pavgb			(mm3, mm1)
			pshufw		(mm1, mm3, 0xD0)
			punpckhbw   mm7, mm1


			por			mm0, mm7

			movq			[eax], mm0

			add			ebx, 8
			add			edx, 8
			add			eax, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2MYUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D2MY(BYTE * py, BYTE * puv, BYTE * pm, int w, int bpr) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [bpr]
			movq			mm6, [masklow]

	loop2:
			movq			mm0, [ebx]
			movq			mm4, [ebx+2*esi]
			movq			mm1, [edx]

			pavgb			(mm0, mm4)
			movq			mm3, mm0
			psllq			mm0, 8
			pavgb			(mm0, mm3)
			psrlw			mm0, 8

			pxor			mm7, mm7
			pshufw		(mm3, mm1, 0xB1)
			pavgb			(mm3, mm1)
			pshufw		(mm3, mm3, 0xD0)
			punpckhbw	mm7, mm3

			por			mm0, mm7

			movq			[eax], mm0

			add			ebx, 8
			add			edx, 8
			add			eax, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2MY) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

//
// Convert a single Y line and a pair of UV lines into a YUY2 line
// by averaging the UV lines and downscaling by two
//
static inline void ConvertLineToYUV_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif

	static MMXDWORD<2> lwrbytes = {0x00ff00ff, 0x00ff00ff};

	if (w)
		{
		__asm
			{
			mov	eax,	[pm]
			mov	ebx,	[py]
			mov	edx,	[puv]
			mov	ecx,  [w]

			add	eax, ecx
			add	ebx, ecx
			add	edx, ecx
			neg	ecx

			movq	mm7, [lwrbytes]

loop1:


			movq			mm0, [ebx+ecx]
			movq			mm3, mm0
			psrlq			mm3, 8
			pavgb			(mm3, mm0)
			pand			mm3, mm7


			movq			mm1, [edx+ecx]
			pshufw		(mm2, mm1, 0xB1)
			pavgb			(mm2, mm1)
			pshufw		(mm2, mm2, 0xDC)
			pxor			mm6, mm6
			punpckhbw	mm6, mm2
			por			mm6, mm3

			movq	[eax+ecx], mm6
			add	ecx, 8

			jne	loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

//
// Convert a pair of single Y and UV into a YUY2 line, downscaling
// by two and merging with an osd bitmap
//
static inline void ConvertLineToYUVThroughOSD_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif

	static MMXDWORD<2> lwrbytes = {0x00ff00ff, 0x00ff00ff};

	if (w)
		{
		__asm
			{
			mov	esi,	[pm]
			mov	ebx,	[py]
			mov	edx,	[puv]
			mov	ecx,  [w]
			mov	edi,  [po]

			push	ebp
			mov	ebp,	[dd]

			add	esi, ecx
			add	ebx, ecx
			add	edx, ecx

			shr	ecx, 1

			lea	edi, [edi+ecx]

			neg	ecx

			movq	mm7, [lwrbytes]

loop1:



			movq			mm0, [ebx+2*ecx]
			movq			mm3, mm0
			psrlq			mm3, 8
			pavgb			(mm3, mm0)
			pand			mm3, mm7


			movq			mm1, [edx+2*ecx]
			pshufw		(mm6, mm1, 0xB1)
			pavgb			(mm6, mm1)
			pshufw		(mm6, mm6, 0xDC)
			pxor			mm2, mm2
			punpckhbw	mm2, mm6
			por			mm2, mm3

			pxor			mm0, mm0

			movq			mm1, mm2
			punpcklbw	mm2, mm0

			punpckhbw	mm1, mm0

			movzx			eax, BYTE PTR [edi+ecx]

			movq			mm3, [ebp + eax * 8 + 2048 + 4096]

			movq			mm4, [ebp + eax * 8 + 4096]

			movzx			eax, BYTE PTR [edi+ecx+1]

			paddw			mm3, [ebp + eax * 8 + 2048 + 8192]

			paddw			mm4, [ebp + eax * 8 + 8192]

			pmullw		mm2, mm3

			paddw			mm2, mm4

			movzx			eax, BYTE PTR [edi+ecx+2]

			movq			mm3, [ebp + eax * 8 + 2048 + 4096]

			movq			mm4, [ebp + eax * 8 + 4096]

			movzx			eax, BYTE PTR [edi+ecx+3]

			paddw			mm3, [ebp + eax * 8 + 2048 + 8192]

			paddw			mm4, [ebp + eax * 8 + 8192]

			pmullw		mm1, mm3

			paddw			mm1, mm4

			psrlw			mm2, 5

			psrlw			mm1, 5

			packuswb		mm2, mm1

			movq	[esi+ 2 * ecx], mm2

			add	ecx, 4
			jne	loop1

			pop	ebp
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with linear x scaling and no y scaling
//



//
// convert a pair of single y and uv lines into a yuy2 line, by using linear
// scaling, always interpolating 9 pixels in line down to 8 pixels
//
// w = number of source pixels to do, w is multiple of 8
//
static inline void ConvertLineToYUV_P8F2_928FIX(BYTE * py, BYTE * puv, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif

	static MMXQUShort LeftLow   = { 0x0008, 0x0007, 0x0006, 0x0005 };
	static MMXQUShort LeftHigh  = { 0x0004, 0x0003, 0x0002, 0x0001 };

	static MMXQUShort RightLow  = { 0x0000, 0x0001, 0x0002, 0x0003 };
	static MMXQUShort RightHigh = { 0x0004, 0x0005, 0x0006, 0x0007 };

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]

			pxor			mm7, mm7

	loop2:


			// unpack 9 bytes of Y data from bytes to words
			// 0-7
			movq			mm0, [ebx]				// py7 py6 py5 py4 py3 py2 py1 py0
			movq			mm1, mm0					// py7 py6 py5 py4 py3 py2 py1 py0
			punpcklbw	mm0, mm7					//     py3     py2     py1     py0
			punpckhbw	mm1, mm7					//     py7     py6     py5     py4
			// 1-8
			movq			mm2, [ebx+1]			// py8 py7 py6 py5 py4 py3 py2 py1
			movq			mm3, mm2					// py8 py7 py6 py5 py4 py3 py2 py1
			punpcklbw	mm2, mm7					//     py4     py3     py2     py1
			punpckhbw	mm3, mm7					//     py8     py7     py6     py5

			// interpolate pixels
			// 0-3
			pmullw		mm0, [LeftLow]
			psraw			mm0, 3
			// 4-7
			pmullw		mm1, [LeftHigh]
			psraw			mm1, 3
			// 1-4
			pmullw		mm2, [RightLow]
			psraw			mm2, 3
			// 5-9
			pmullw		mm3, [RightHigh]
			psraw			mm3, 3


			// add pixels together to get new 8
			paddw			mm0, mm2					//      n3      n2      n1      n0
			paddw			mm3, mm1					//      n7      n6      n5      n4

			// pack pixels again to store
			packuswb		mm0, mm3					//  n7  n6  n5  n4  n3  n2  n1  n0
			movq			mm1, mm0					//  n7  n6  n5  n4  n3  n2  n1  n0

			// interleave y pixels with uv data
			movq			mm2, [edx]				// uv7 uv6 uv5 uv4 uv3 uv2 uv1 uv0
			punpcklbw	mm0, mm2
			punpckhbw	mm1, mm2


			// store 8 bytes
			movq			[eax], mm0
			movq			[eax + 8], mm1

			add			eax, 16
			add			edx, 8
			add			ebx, 9

			sub			ecx, 16

			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8F2_928FIX) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}





///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with no x scaling and linear y scaling
//

static inline void ConvertLineToYUV_P8D1Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int w, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort full = {0x0100, 0x0100, 0x0100, 0x0100};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			esi, [py0]
			mov			edi, [py1]
			mov			edx, [puv]
			mov			ecx, [w]

			movd			mm6, [lo]
			pshufw		(mm6, mm6, 0x00)

			movq			mm7, [full]
			psubw			mm7, mm6
			pxor			mm5, mm5

			lea			eax, [eax+2*ecx]
			add			esi, ecx
			add			edi, ecx
			add			edx, ecx
			neg			ecx

	loop2:


			movq			mm0, [esi+ecx]
			movq			mm2, mm0
			punpcklbw	mm0, mm5
			punpckhbw	mm2, mm5

			movq			mm1, [edi+ecx]
			movq			mm3, mm1
			punpcklbw	mm1, mm5
			punpckhbw	mm3, mm5

			pmullw		mm0, mm7
			pmullw		mm1, mm6
			paddw			mm0, mm1
			psrlw			mm0, 8
			pmullw		mm2, mm7
			pmullw		mm3, mm6
			paddw			mm2, mm3
			psrlw			mm2, 8
			packuswb		mm0, mm2

			movq			mm1, [edx+ecx]

			movq			mm2, mm0
			punpcklbw	mm0, mm1
			punpckhbw	mm2, mm1

			movq			[eax+2*ecx], mm0
			movq			[eax+2*ecx+8], mm2

			add			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D1Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int w,	BYTE * po, DDWORD * dd, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort full = {0x0100, 0x0100, 0x0100, 0x0100};

	__asm
		{
		mov			ecx, [w]
		xor			ebx, ebx
		mov			edi, [pm]

		movd			mm6, [lo]
		pshufw		(mm6, mm6, 0x00)

		movq			mm7, [full]
		psubw			mm7, mm6
		pxor			mm5, mm5
loop2:
		mov			esi, [py0]
		mov			edx, [py1]

		movq			mm0, [esi+2*ebx]

		movq			mm1, [edx+2*ebx]

		movq			mm2, mm0
		punpcklbw	mm0, mm5
		punpckhbw	mm2, mm5
		movq			mm3, mm1
		punpcklbw	mm1, mm5
		punpckhbw	mm3, mm5

		pmullw		mm0, mm7
		pmullw		mm1, mm6
		paddw			mm0, mm1
		psrlw			mm0, 8
		pmullw		mm2, mm7
		pmullw		mm3, mm6
		paddw			mm2, mm3
		psrlw			mm2, 8
		packuswb		mm0, mm2

		mov			esi, [puv]

		movq			mm1, [esi+2*ebx]

		movq			mm2, mm0
		punpcklbw	mm0, mm1
		punpckhbw	mm2, mm1

		movq			mm1, mm0
		punpcklbw	mm0, mm5
		punpckhbw	mm1, mm5

		movq			mm3, mm2
		punpcklbw	mm2, mm5
		punpckhbw	mm3, mm5

		mov			esi, [po]
		mov			edx, [dd]

		movzx			eax, BYTE PTR [esi+ebx]
		pmullw		mm0, [edx + eax * 8 + 2048]
		paddw			mm0, [edx + eax * 8]
		psrlw			mm0, 4

		movzx			eax, BYTE PTR [esi+ebx+1]
		pmullw		mm1, [edx + eax * 8 + 2048]
		paddw			mm1, [edx + eax * 8]
		psrlw			mm1, 4

		movzx			eax, BYTE PTR [esi+ebx+2]
		pmullw		mm2, [edx + eax * 8 + 2048]
		paddw			mm2, [edx + eax * 8]
		psrlw			mm2, 4

		movzx			eax, BYTE PTR [esi+ebx+3]
		pmullw		mm3, [edx + eax * 8 + 2048]
		paddw			mm3, [edx + eax * 8]
		psrlw			mm3, 4

		packuswb		mm0, mm1
		packuswb		mm2, mm3

		movq			[edi+4*ebx], mm0
		movq			[edi+4*ebx+8], mm2

		add			ebx, 4
		sub			ecx, 8
		jne			loop2
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}



///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with binary x scaling and linear y scaling
//

static inline void ConvertLineToYUV_P8D2Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int w, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort full = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			esi, [py0]
			mov			edi, [py1]
			mov			edx, [puv]
			mov			ecx, [w]

			add			eax, ecx
			add			edx, ecx
			add			esi, ecx
			add			edi, ecx
			neg			ecx

			movd			mm6, [lo]
			pshufw		(mm6, mm6, 0x00)

			psrlw			mm6, 1
			movq			mm7, [full]
			psubw			mm7, mm6
			pxor			mm5, mm5
loop1:



			movq			mm0, [esi+ecx]

			movq			mm1, mm0
			psrlw			mm0, 8
			pand			mm1, [masklow]
			paddw			mm0, mm1

			movq			mm1, [edi+ecx]
			movq			mm2, mm1
			psrlw			mm1, 8
			pand			mm2, [masklow]
			paddw			mm1, mm2

			pmullw		mm0, mm7
			pmullw		mm1, mm6
			paddw			mm0, mm1
			psrlw			mm0, 8
			packuswb		mm0, mm0


			movq			mm2, [edx+ecx]
			pshufw		(mm1, mm2, 0xB1)
			pavgb			(mm1, mm2)
			pshufw		(mm1, mm1, 0xDC)

			punpcklbw	mm0, mm1

			movq			[eax+ecx], mm0
			add			ecx, 8
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}



static inline void ConvertLineToYUVThroughOSD_P8D2Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int w,BYTE * po, DDWORD * dd, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort full = {0x0080, 0x0080, 0x0080, 0x0080};
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			ecx, [w]
			xor			ebx, ebx
			mov			edi, [pm]

			movd			mm6, [lo]
			pshufw		(mm6, mm6, 0x00)

			psrlw			mm6, 1
			movq			mm7, [full]
			psubw			mm7, mm6
			pxor			mm5, mm5
loop1:
			mov			esi, [py0]
			mov			edx, [py1]

			movq			mm0, [esi+2*ebx]

			movq			mm1, mm0
			psrlw			mm0, 8
			pand			mm1, [masklow]
			paddw			mm0, mm1

			movq			mm1, [edx+2*ebx]
			mov			edx, [puv]

			movq			mm2, mm1
			psrlw			mm1, 8
			pand			mm2, [masklow]
			paddw			mm1, mm2

			pmullw		mm0, mm7
			pmullw		mm1, mm6
			paddw			mm0, mm1
			psrlw			mm0, 8
			packuswb		mm0, mm0

			movq			mm2, [edx+2*ebx]
			pshufw		(mm1, mm2, 0xB1)
			pavgb			(mm1, mm2)
			pshufw		(mm1, mm1, 0xDC)

			punpcklbw	mm0, mm1

			movq			mm1, mm0
			punpcklbw	mm0, mm5
			punpckhbw	mm1, mm5

			mov			esi, [po]
			mov			edx, [dd]

			movzx			eax, BYTE PTR [esi+ebx]
			movq			mm3, [edx + eax * 8 + 2048 + 4096]
			movq			mm4, [edx + eax * 8 + 4096]
			movzx			eax, BYTE PTR [esi+ebx+1]
			paddw			mm3, [edx + eax * 8 + 2048 + 8192]
			paddw			mm4, [edx + eax * 8 + 8192]
			pmullw		mm0, mm3
			paddw			mm0, mm4

			movzx			eax, BYTE PTR [esi+ebx+2]
			movq			mm3, [edx + eax * 8 + 2048 + 4096]
			movq			mm4, [edx + eax * 8 + 4096]
			movzx			eax, BYTE PTR [esi+ebx+3]
			paddw			mm3, [edx + eax * 8 + 2048 + 8192]
			paddw			mm4, [edx + eax * 8 + 8192]
			pmullw		mm1, mm3
			paddw			mm1, mm4

			psrlw			mm0, 5
			psrlw			mm1, 5

			packuswb		mm0, mm1

			movq			[edi+ 2 * ebx], mm0

			add			ebx, 4
			sub			ecx, 8
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D2Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with linear x scaling and linear y scaling
//
static inline void ConvertLineToYUV_P8F2Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int w, int xcount, int xfraction, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort negate = {0x7fff, 0x0000, 0x7fff, 0x0000};
	static MMXQShort full = {0x0100, 0x0100, 0x0100, 0x0100};

	if (w)
		{
		__asm
			{
			mov			eax, [py0]
			mov			ebx, [py1]
			mov			edx, [pm]
			mov			ecx, [w]
			mov			esi, [xcount]

			movd			mm6, [lo]
			pshufw		(mm6, mm6, 0x00)

			movq			mm7, [full]
			psubw			mm7, mm6
			pxor			mm5, mm5
	loop2:
			mov			edi, esi
			shr			edi, 16

			movd			mm0, [eax+edi]
			movd			mm3, [ebx+edi]
			movd			mm1, esi
			add			esi, [xfraction]

			and			edi, 0xfffffffe
			add			edi, [puv]
			movd			mm4, [edi]

			mov			edi, esi
			shr			edi, 16

			movd			mm2, [eax+edi]

			punpcklwd	mm0, mm2
			punpcklbw	mm0, mm5

			movd			mm2, [ebx+edi]

			punpcklwd	mm3, mm2
			punpcklbw	mm3, mm5

			pmullw		mm0, mm7
			pmullw		mm3, mm6
			paddw			mm0, mm3
			psrlw			mm0, 8

			movd			mm3, esi
			add			esi, [xfraction]

			punpckldq	mm1, mm3
			psrld			mm1, 1
			pand			mm1, [negate]
			movq			mm3, mm1
			pslld			mm1, 16
			por			mm1, mm3
			pxor			mm1, [negate]

			pmaddwd		mm0, mm1
			psrld			mm0, 15

			packssdw		mm0, mm0
			packuswb		mm0, mm0
			punpcklbw	mm0, mm4

			movd			[edx], mm0

			add			edx, 4

			sub			ecx, 2
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8F2Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with four tap x scaling and no y scaling
//



///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with no x scaling and four tap y scaling
//

static inline void ConvertLineToYUVThroughOSD_P8D1Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int w, BYTE * po, DDWORD * dd, short * filter) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			eax, [filter]

			movq			mm0, [eax]				// f7f6 f5f4 f3f2 f1f0

			pshufw		(mm1, mm0, 0x55)
			pshufw		(mm2, mm0, 0xAA)
			pshufw		(mm3, mm0, 0xFF)
			pshufw		(mm0, mm0, 0x00)

			xor			ecx, ecx
			mov			edi, [py0]
			mov			esi, [po]
			mov			ebx, [py1]

			pxor			mm7, mm7
	loop1:
			movd			mm4, [edi]
			punpcklbw	mm4, mm7

			movd			mm5, [ebx]
			punpcklbw	mm5, mm7

			mov			edx, [py2]
			mov			eax, [py3]

			pmullw		mm4, mm0
			pmullw		mm5, mm1
			paddsw		mm4, mm5

			movd			mm6, [edx + ecx]
			movd			mm5, [eax + ecx]

			punpcklbw	mm6, mm7
			pmullw		mm6, mm2
			paddsw		mm4, mm6

			add			edi, 8
			movd			mm6, [edi - 4]
			punpcklbw	mm6, mm7

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm4, mm5

			movd			mm5, [ebx + 4]
			punpcklbw	mm5, mm7
			psraw			mm4, 6

			pmullw		mm6, mm0
			pmullw		mm5, mm1
			paddsw		mm6, mm5

			movd			mm5, [edx + ecx + 4]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm2
			paddsw		mm6, mm5

			movd			mm5, [eax + ecx + 4]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm6, mm5

			movzx			eax, BYTE PTR [esi]
			add			ebx, 8
			psraw			mm6, 6

			mov			edx, [puv]
			packuswb		mm4, mm6
			movq			mm6, [edx + ecx]

			movq			mm5, mm4
			punpcklbw	mm4, mm6
			punpckhbw	mm5, mm6

			movq			mm6, mm4
			punpcklbw	mm4, mm7
			punpckhbw	mm6, mm7

			mov			edx, [dd]

			pmullw		mm4, [edx + eax * 8 + 2048]

			paddw			mm4, [edx + eax * 8]
			psrlw			mm4, 4

			movzx			eax, BYTE PTR [esi+1]

			pmullw		mm6, [edx + eax * 8 + 2048]

			paddw			mm6, [edx + eax * 8]
			psrlw			mm6, 4

			packuswb		mm4, mm6

			movq			mm6, mm5
			punpcklbw	mm5, mm7
			punpckhbw	mm6, mm7

			movzx			eax, BYTE PTR [esi+2]
			pmullw		mm5, [edx + eax * 8 + 2048]
			paddw			mm5, [edx + eax * 8]
			psrlw			mm5, 4

			movzx			eax, BYTE PTR [esi+3]
			pmullw		mm6, [edx + eax * 8 + 2048]
			paddw			mm6, [edx + eax * 8]
			psrlw			mm6, 4

			add			ecx, 8
			add			esi, 4
			cmp			ecx, [w]

			packuswb		mm5, mm6
			mov			eax, [pm]

			movq			[eax+2*ecx-16], mm4
			movq			[eax+2*ecx-8], mm5

			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1Y4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D1Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int w, short * filter) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			eax, [filter]

			movq			mm0, [eax]


			pshufw		(mm1, mm0, 0x55)
			pshufw		(mm2, mm0, 0xAA)
			pshufw		(mm3, mm0, 0xFF)
			pshufw		(mm0, mm0, 0x00)

			mov			ecx, [w]
			xor			ebx, ebx
			mov			edi, [pm]

			pxor			mm7, mm7
	loop1:
			mov			eax, [py0]

			movd			mm4, [eax + 2 * ebx]

			mov			eax, [py1]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm4, mm7
			punpcklbw	mm5, mm7

			pmullw		mm4, mm0
			pmullw		mm5, mm1
			paddsw		mm4, mm5

			mov			eax, [py2]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm2
			paddsw		mm4, mm5

			mov			eax, [py3]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm4, mm5

			mov			eax, [py0]
			movd			mm6, [eax + 2 * ebx + 4]
			psraw			mm4, 6

			mov			eax, [py1]
			movd			mm5, [eax + 2 * ebx + 4]

			punpcklbw	mm6, mm7
			punpcklbw	mm5, mm7

			pmullw		mm6, mm0
			pmullw		mm5, mm1
			paddsw		mm6, mm5

			mov			eax, [py2]
			movd			mm5, [eax + 2 * ebx + 4]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm2
			paddsw		mm6, mm5

			mov			eax, [py3]
			movd			mm5, [eax + 2 * ebx + 4]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm6, mm5

			psraw			mm6, 6
			packuswb		mm4, mm6

			mov			eax, [puv]
			movq			mm6, [eax + 2 * ebx]
			movq			mm5, mm4
			punpcklbw	mm4, mm6
			punpckhbw	mm5, mm6

			movq			[edi+4*ebx], mm4
			movq			[edi+4*ebx+8], mm5

			add			ebx, 4
			sub			ecx, 8
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1Y4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


static inline void ConvertLineToYUV_P8D1Y4MUV(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv0, BYTE * puv1, BYTE * pm, int w, short * filter) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			eax, [filter]

			movq			mm0, [eax]


			pshufw		(mm1, mm0, 0x55)
			pshufw		(mm2, mm0, 0xAA)
			pshufw		(mm3, mm0, 0xFF)
			pshufw		(mm0, mm0, 0x00)

			mov			ecx, [w]
			xor			ebx, ebx
			mov			edi, [pm]

			pxor			mm7, mm7
	loop1:
			mov			eax, [py0]

			movd			mm4, [eax + 2 * ebx]

			mov			eax, [py1]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm4, mm7
			punpcklbw	mm5, mm7

			pmullw		mm4, mm0
			pmullw		mm5, mm1
			paddsw		mm4, mm5

			mov			eax, [py2]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm2
			paddsw		mm4, mm5

			mov			eax, [py3]
			movd			mm5, [eax + 2 * ebx]
			prefetcht0	[eax + 2 * ebx + 32]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm4, mm5

			psraw			mm4, 6


			mov			eax, [puv0]
			movd			mm5, [eax + 2 * ebx]
			mov			eax, [puv1]
			movd			mm6, [eax + 2 * ebx]

			pavgb			(mm5, mm6)

			packuswb		mm4, mm4

			punpcklbw	mm4, mm5

			movq			[edi+4*ebx], mm4

			add			ebx, 2
			sub			ecx, 4
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1Y4MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D1Y4MUV(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv0, BYTE * puv1, BYTE * pm, int w, BYTE * po, DDWORD * dd, short * filter) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			eax, [filter]

			movq			mm0, [eax]


			pshufw		(mm1, mm0, 0x55)
			pshufw		(mm2, mm0, 0xAA)
			pshufw		(mm3, mm0, 0xFF)
			pshufw		(mm0, mm0, 0x00)

			mov			ecx, [w]
			xor			ebx, ebx
			mov			edi, [pm]
			mov			esi, [po]

			pxor			mm7, mm7
	loop1:
			mov			eax, [py0]

			movd			mm4, [eax + 2 * ebx]

			mov			eax, [py1]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm4, mm7
			punpcklbw	mm5, mm7

			pmullw		mm4, mm0
			pmullw		mm5, mm1
			paddsw		mm4, mm5

			mov			eax, [py2]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm2
			paddsw		mm4, mm5

			mov			eax, [py3]
			movd			mm5, [eax + 2 * ebx]

			punpcklbw	mm5, mm7
			pmullw		mm5, mm3
			paddsw		mm4, mm5

			psraw			mm4, 6

			mov			eax, [puv0]
			movd			mm5, [eax + 2 * ebx]
			mov			eax, [puv1]
			movd			mm6, [eax + 2 * ebx]

			pavgb			(mm5, mm6)

			packuswb		mm4, mm4

			punpcklbw	mm4, mm5

			movzx			eax, BYTE PTR [esi+ebx]

			movq			mm6, mm4
			punpcklbw	mm4, mm7
			punpckhbw	mm6, mm7

			mov			edx, [dd]

			pmullw		mm4, [edx + eax * 8 + 2048]

			paddw			mm4, [edx + eax * 8]
			psrlw			mm4, 4

			movzx			eax, BYTE PTR [esi+ebx+1]

			pmullw		mm6, [edx + eax * 8 + 2048]

			paddw			mm6, [edx + eax * 8]
			psrlw			mm6, 4

			packuswb		mm4, mm6

			movq			[edi+4*ebx], mm4

			add			ebx, 2
			sub			ecx, 4
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1Y4MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


///////////////////////////////////////////////////////////////////////////////
//
// eight bit deinterlacing routines
//

///////////////////////////////////////////////////////////////////////////////
//
// eight bit scaling routines
//



static inline void ScaleYUVLines_P8Y2(BYTE * spm1, BYTE * spm2, BYTE * dpm, int w, int lo) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif

	static MMXQShort full = {0x0100, 0x0100, 0x0100, 0x0100};

	if (w)
		{
		__asm
			{
			mov			eax, [dpm]
			mov			esi, [spm1]
			mov			edi, [spm2]
			mov			ecx, [w]

			sub			esi, eax
			sub			edi, eax

			movd			mm6, [lo]

			pshufw		(mm6, mm6, 0x00)

			movq			mm7, [full]
			psubw			mm7, mm6
			pxor			mm5, mm5
	loop2:
			movq			mm0, [esi+eax]
			movq			mm2, mm0

			movq			mm1, [edi+eax]
			movq			mm3, mm1

			punpcklbw	mm0, mm5
			pmullw		mm0, mm7

			punpcklbw	mm1, mm5
			pmullw		mm1, mm6

			punpckhbw	mm2, mm5
			pmullw		mm2, mm7

			punpckhbw	mm3, mm5
			pmullw		mm3, mm6

			paddw			mm0, mm1
			psrlw			mm0, 8

			paddw			mm2, mm3
			psrlw			mm2, 8

			packuswb		mm0, mm2
			add			eax, 8

			movq			[eax-8], mm0
			sub			ecx, 4
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "OPT P8Y2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ScaleYUVLine_P8F2(BYTE * spm, BYTE * dpm, int w, int xcount, int xfraction) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQUShort negate = {0x7fff, 0x0000, 0x7fff, 0x0000};
	static MMXQUShort lower = {0x00ff, 0x00ff, 0x00ff, 0x00ff};
	static MMXQUShort upper = {0xff00, 0xff00, 0xff00, 0xff00};
	static MMXQUShort fullneg = {0xffff, 0x0000, 0xffff, 0x0000};
	static MMXQUShort mask15 = {0x7fff, 0x7fff, 0x7fff, 0x7fff};

	if (w)
		{
		__asm
			{
			mov			eax, [dpm]
			mov			ebx, [spm]
			mov			ecx, [w]
			mov			esi, [xcount]
			mov			edx, [xfraction]

			movd			mm6, esi
			movd			mm7, edx
			paddd			mm7, mm6


			punpcklwd	mm6, mm7
			punpcklwd	mm6, mm6
			psrlw			mm6, 1
			pxor			mm6, [negate]

			movd			mm7, edx
			pshufw		(mm7, mm7, 0x00)

			pxor			mm7, [fullneg]
			psubw			mm7, [fullneg]

			movq			mm4, [lower]
			movq			mm5, [mask15]
			mov			edi, esi
	loop2:
			shr			esi, 16
			movd			mm0, [ebx+2*esi]
			and			esi, 0xfffffffe

			movd			mm3, [ebx+2*esi]

			add			edi, edx
			mov			esi, edi
			shr			edi, 16

			movd			mm1, [ebx+2*edi]
			punpckldq	mm0, mm1

			add			esi, edx
			mov			edi, esi
			shr			esi, 16

			movd			mm2, [ebx+2*esi]
			pand			mm0, mm4
			and			esi, 0xfffffffe

			pmaddwd		mm0, mm6
			paddw			mm6, mm7

			psrld			mm0, 15
			pand			mm6, mm5

			movd			mm1, [ebx+2*esi]
			punpckldq	mm3, mm1

			add			edi, edx
			mov			esi, edi
			shr			edi, 16

			movd			mm1, [ebx+2*edi]
			add			esi, edx
			mov			edi, esi

			pand			mm3, [upper]
			punpckldq	mm2, mm1
			pand			mm2, mm4

			pmaddwd		mm2, mm6
			psrld			mm2, 15
			packssdw		mm0, mm2

			por			mm0, mm3
			paddw			mm6, mm7

			movq			[eax], mm0
			pand			mm6, mm5

			add			eax, 8

			sub			ecx, 4
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "OPT P8F2) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////



static inline void ConvertLineToYUV_P8D1I4(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80  = {0x80808080, 0x80808080};	// 1000 0000 1000 0000 1000 0000 1000 0000
	static MMXInt<2> mask7   = {0x7f7f7f7f, 0x7f7f7f7f};	// 0111 1111 0111 1111 0111 1111 0111 1111
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};	// 0000 1111 0000 1111 0000 1111 0000 1111
	static MMXInt<2> zero    = {0x00000000, 0x00000000};	// 0000 0000 0000 0000 0000 0000 0000 0000
	static MMXInt<2> border  = {0x0a0a0a0a, 0x0a0a0a0a};	// 0000 1010 0000 1010 0000 1010 0000 1010

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[py2]
			mov			edi,  [bpr]

			add			esi,  edi

			movq			mm3, [esi+2*edi]
	loop1:
			prefetcht0	[esi+2*edi + 48]

			movq			mm5, [ebx]
			movq			mm0, mm5

			movq			mm4, [esi]
			psubusb		mm5, mm4
			movq			mm1, mm4

			movq			mm6, [ebx+2*edi]
			psubusb		mm4, mm6
			paddusb		mm5, mm4

			movq			mm7, [border]
			psubusb		mm5, mm7

			psubusb		mm6, mm3
			paddusb		mm6, mm4

			movq			mm2, [ebx+2*edi]
			psubusb		mm6, mm7
			pxor			mm4, mm4

			pcmpeqb		mm5, mm4
			pcmpeqb		mm6, mm4

			por			mm5, mm6
			movq			mm6, mm2

			psubusb		mm6, mm1

			psubusb		mm3, mm2
			psubusb		mm1, mm0

			paddusb		mm3, mm6
			paddusb		mm1, mm6

			movq			mm6, [esi]
			psubusb		mm3, mm7
			psubusb		mm1, mm7

			pcmpeqb		mm3, mm4
			pcmpeqb		mm1, mm4

			movq			mm4, [ebx + edi]
			por			mm5, mm3

			prefetcht0	[ebx + edi + 48]
			por			mm5, mm1

			pavgb			(mm0, mm2)

			movq			mm3, mm4
			movq			mm1, mm0

			// mm0, mm1, mm2
			pmaxub		(mm4, mm0)
			pmaxub		(mm0, mm6)
			pmaxub		(mm3, mm6)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm4, mm3)

			movq			mm3, [esi+2*edi+8]

			add			esi, 8

			add			ebx, 8

			movq			mm0, [edx]
			pand			mm4, mm5

			pandn			mm5, mm1

			por			mm5, mm4

			add			edx, 8

			movq			mm4, mm5

			punpcklbw	mm4, mm0

			movq			[eax], mm4
			punpckhbw	mm5, mm0

			movq			[eax+8], mm5

			sub			ecx, 8
			lea			eax, [eax + 16]
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1I4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}




static inline void ConvertLineToYUV_P8D1I4MUV(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> zero = {0x00000000, 0x00000000};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[py2]
			mov			edi,  [bpr]

			add			esi,  edi

			movq			mm3, [esi+2*edi]
	loop1:
			prefetcht0	[esi + 2*edi + 40]
			movq			mm5, [ebx]
			movq			mm0, mm5

			movq			mm4, [esi]
			psubusb		mm5, mm4
			movq			mm1, mm4

			movq			mm6, [ebx+2*edi]
			psubusb		mm4, mm6
			paddusb		mm5, mm4

			movq			mm7, [border]
			psubusb		mm5, mm7

			psubusb		mm6, mm3
			paddusb		mm6, mm4

			movq			mm2, [ebx+2*edi]
			prefetcht0	[edx + 2*edi+48]
			psubusb		mm6, mm7
			pxor			mm4, mm4

			pcmpeqb		mm5, mm4
			pcmpeqb		mm6, mm4

			por			mm5, mm6
			movq			mm6, mm2

			psubusb		mm6, mm1

			psubusb		mm3, mm2
			psubusb		mm1, mm0

			paddusb		mm3, mm6
			paddusb		mm1, mm6

			movq			mm6, [esi]
			psubusb		mm3, mm7
			psubusb		mm1, mm7

			movq			mm7, [mask7]
			pcmpeqb		mm3, mm4
			pcmpeqb		mm1, mm4

			movq			mm4, [ebx + edi]
			por			mm5, mm3

			prefetcht0	[ebx + edi + 56]
			pavgb			(mm0, mm2)

			por			mm5, mm1

			movq			mm3, mm4
			movq			mm1, mm0


			// mm0, mm1, mm2
			pmaxub		(mm4, mm0)
			pmaxub		(mm0, mm6)
			pmaxub		(mm3, mm6)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm4, mm3)

			movq			mm3, [esi+2*edi+8]
			add			esi, 8

			add			ebx, 8

			movq			mm0, [edx]
			pand			mm4, mm5

			movq			mm6, [edx + 2 * edi]
			pandn			mm5, mm1

			por			mm5, mm4

			pavgb			(mm0, mm6)

			add			edx, 8

			movq			mm4, mm5

			punpcklbw	mm4, mm0

			movq			[eax], mm4
			punpckhbw	mm5, mm0

			movq			[eax+8], mm5

			sub			ecx, 8
			lea			eax, [eax + 16]
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1I4MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D2I4(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> zero = {0x00000000, 0x00000000};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[py2]
			mov			edi,  [bpr]

			add			esi,  edi

			movq			mm3, [esi+2*edi]
	loop1:
			movq			mm5, [ebx]
			movq			mm0, mm5

			movq			mm4, [esi]
			psubusb		mm5, mm4
			movq			mm1, mm4

			movq			mm6, [ebx+2*edi]
			psubusb		mm4, mm6
			paddusb		mm5, mm4

			movq			mm7, [border]
			psubusb		mm5, mm7

			psubusb		mm6, mm3
			paddusb		mm6, mm4

			movq			mm2, [ebx+2*edi]
			psubusb		mm6, mm7
			pxor			mm4, mm4

			pcmpeqb		mm5, mm4
			pcmpeqb		mm6, mm4

			por			mm5, mm6
			movq			mm6, mm2

			psubusb		mm6, mm1

			psubusb		mm3, mm2
			psubusb		mm1, mm0

			paddusb		mm3, mm6
			paddusb		mm1, mm6

			movq			mm6, [esi]
			psubusb		mm3, mm7
			psubusb		mm1, mm7

			pcmpeqb		mm3, mm4
			pcmpeqb		mm1, mm4

			movq			mm4, [ebx + edi]
			por			mm5, mm3

			por			mm5, mm1

			pavgb			(mm0, mm2)

			movq			mm3, mm4

			movq			mm1, mm0


			// mm0, mm1, mm2
			pmaxub		(mm4, mm0)
			pmaxub		(mm0, mm6)
			pmaxub		(mm3, mm6)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm4, mm3)

			movq			mm3, [esi+2*edi+8]
			add			esi, 8

			add			ebx, 8

			movq			mm0, [edx]
			pand			mm4, mm5

			pandn			mm5, mm1

			por			mm5, mm4

			add			edx, 8

			movq			mm4, mm5
			pand			mm4, [masklow]
			psrlw			mm5, 8

			pavgb			(mm5, mm4)

			pxor			mm7, mm7

			pshufw		(mm1, mm0, 0xD8)
			pshufw		(mm0, mm0, 0x8D)
			pavgb			(mm1, mm0)
			punpckhbw	mm7, mm1

			por			mm5, mm7

			movq			[eax], mm5

			sub			ecx, 8
			lea			eax, [eax + 8]
			jne			loop1
			}
		}

#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2I4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUV_P8D2I4MUV(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> zero = {0x00000000, 0x00000000};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[py2]
			mov			edi,  [bpr]

			add			esi,  edi

			movq			mm3, [esi+2*edi]
	loop1:
			movq			mm5, [ebx]
			movq			mm0, mm5

			movq			mm4, [esi]
			psubusb		mm5, mm4
			movq			mm1, mm4

			movq			mm6, [ebx+2*edi]
			psubusb		mm4, mm6
			paddusb		mm5, mm4

			movq			mm7, [border]
			psubusb		mm5, mm7

			psubusb		mm6, mm3
			paddusb		mm6, mm4

			movq			mm2, [ebx+2*edi]
			psubusb		mm6, mm7
			pxor			mm4, mm4

			pcmpeqb		mm5, mm4
			pcmpeqb		mm6, mm4

			por			mm5, mm6
			movq			mm6, mm2

			psubusb		mm6, mm1

			psubusb		mm3, mm2
			psubusb		mm1, mm0

			paddusb		mm3, mm6
			paddusb		mm1, mm6

			movq			mm6, [esi]
			psubusb		mm3, mm7
			psubusb		mm1, mm7

			pcmpeqb		mm3, mm4
			pcmpeqb		mm1, mm4

			movq			mm4, [ebx + edi]
			por			mm5, mm3
			por			mm5, mm1

			pavgb			(mm0, mm2)

			movq			mm3, mm4

			movq			mm1, mm0


			// mm0, mm1, mm2
			pmaxub		(mm4, mm0)
			pmaxub		(mm0, mm6)
			pmaxub		(mm3, mm6)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm4, mm3)

			movq			mm3, [esi+2*edi+8]
			add			esi, 8

			add			ebx, 8

			movq			mm0, [edx]
			pand			mm4, mm5

			movq			mm2, [edx+edi]
			pandn			mm5, mm1

			por			mm5, mm4

			add			edx, 8

			movq			mm4, mm5
			pand			mm4, [masklow]
			psrlw			mm5, 8

			pavgb			(mm5, mm4)

			pxor			mm7, mm7

			pshufw		(mm1, mm0, 0xD8)
			pshufw		(mm0, mm0, 0x8D)
			pavgb			(mm1, mm0)

			pshufw		(mm0, mm2, 0xD8)
			pshufw		(mm2, mm2, 0x8D)
			pavgb			(mm0, mm2)

			pavgb			(mm1, mm0)

			punpckhbw	mm7, mm1

			por			mm5, mm7

			movq			[eax], mm5

			sub			ecx, 8
			lea			eax, [eax + 8]
			jne			loop1

			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2I4MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D1I4(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};
	static MMXInt<2> zero = {0x00000000, 0x00000000};

	if (w)
		{
		py2 += bpr;

		__asm
			{
			mov			ecx, 0
			mov			esi, [w]
			shr			esi, 1
			mov			edi, [py]
			mov			edx, [dd]
	loop1:
			mov			eax, [bpr]
			mov			ebx, [py2]

			movq			mm0, [edi]
			movq			mm2, [edi+2*eax]

			pavgb			(mm0, mm2)

			movq			mm1, [ebx + 2 * ecx]
			movq			mm2, [edi + eax]

			// mm0, mm1, mm2
			movq			mm3, mm2
			pmaxub		(mm2, mm0)
			pmaxub		(mm0, mm1)
			pmaxub		(mm3, mm1)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm2, mm3)

			mov			eax, [puv]
			movq			mm6, [eax + 2 * ecx]

			movq			mm4, mm2
			punpcklbw	mm4, mm6
			punpckhbw	mm2, mm6

			pxor			mm7, mm7

			movq			mm0, mm4
			punpcklbw	mm0, mm7
			punpckhbw	mm4, mm7

			movq			mm1, mm2
			punpcklbw	mm1, mm7
			punpckhbw	mm2, mm7

			mov			ebx, [po]

			movzx			eax, BYTE PTR [ebx + ecx]
			pmullw		mm0, [edx + eax * 8 + 2048]
			paddw			mm0, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 1]
			pmullw		mm4, [edx + eax * 8 + 2048]
			paddw			mm4, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 2]
			pmullw		mm1, [edx + eax * 8 + 2048]
			paddw			mm1, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 3]
			pmullw		mm2, [edx + eax * 8 + 2048]
			paddw			mm2, [edx + eax * 8]

			psrlw			mm0, 4
			psrlw			mm4, 4
			psrlw			mm1, 4
			psrlw			mm2, 4

			packuswb		mm0, mm4
			packuswb		mm1, mm2

			mov			eax, [pm]
			movq			[eax + 4 * ecx], mm0
			movq			[eax + 4 * ecx + 8], mm1

			add			edi, 8
			add			ecx, 4
			cmp			ecx, esi
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1I4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D1I4MUV(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};
	static MMXInt<2> zero = {0x00000000, 0x00000000};

	if (w)
		{
		py2 += bpr;

		__asm
			{
			mov			ecx, 0
			mov			esi, [w]
			shr			esi, 1
			mov			edi, [py]
			mov			edx, [dd]
	loop1:
			mov			eax, [bpr]
			mov			ebx, [py2]

			movq			mm0, [edi]
			movq			mm2, [edi+2*eax]

			pavgb			(mm0, mm2)

			movq			mm1, [ebx + 2 * ecx]
			movq			mm2, [edi + eax]

			// mm0, mm1, mm2
			movq			mm3, mm2
			pmaxub		(mm2, mm0)
			pmaxub		(mm0, mm1)
			pmaxub		(mm3, mm1)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm2, mm3)

			mov			ebx, [puv]
			movq			mm6, [ebx + 2 * ecx]
			lea			ebx, [ebx + 2 * eax]
			movq			mm5, [ebx + 2 * ecx]

			pavgb			(mm6, mm5)

			movq			mm4, mm2
			punpcklbw	mm4, mm6
			punpckhbw	mm2, mm6

			pxor			mm7, mm7

			movq			mm0, mm4
			punpcklbw	mm0, mm7
			punpckhbw	mm4, mm7

			movq			mm1, mm2
			punpcklbw	mm1, mm7
			punpckhbw	mm2, mm7

			mov			ebx, [po]

			movzx			eax, BYTE PTR [ebx + ecx]
			pmullw		mm0, [edx + eax * 8 + 2048]
			paddw			mm0, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 1]
			pmullw		mm4, [edx + eax * 8 + 2048]
			paddw			mm4, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 2]
			pmullw		mm1, [edx + eax * 8 + 2048]
			paddw			mm1, [edx + eax * 8]

			movzx			eax, BYTE PTR [ebx + ecx + 3]
			pmullw		mm2, [edx + eax * 8 + 2048]
			paddw			mm2, [edx + eax * 8]

			psrlw			mm0, 4
			psrlw			mm4, 4
			psrlw			mm1, 4
			psrlw			mm2, 4

			packuswb		mm0, mm4
			packuswb		mm1, mm2

			mov			eax, [pm]
			movq			[eax + 4 * ecx], mm0
			movq			[eax + 4 * ecx + 8], mm1

			add			edi, 8
			add			ecx, 4
			cmp			ecx, esi
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1I4MUV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


static inline void ConvertLineToYUVThroughOSD_P8D2I4(BYTE * py, BYTE * puv, BYTE * py2, int bpr, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> full80 = {0x80808080, 0x80808080};
	static MMXInt<2> mask7 = {0x7f7f7f7f, 0x7f7f7f7f};
	static MMXInt<2> maxdiff = {0x0f0f0f0f, 0x0f0f0f0f};
	static MMXInt<2> border = {0x0a0a0a0a, 0x0a0a0a0a};
	static MMXInt<2> zero = {0x00000000, 0x00000000};
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		py2 += bpr;

		__asm
			{
			mov			ecx, 0
			mov			esi, [w]
			shr			esi, 1
			mov			edi, [py]
			mov			edx, [dd]
	loop1:
			mov			eax, [bpr]
			mov			ebx, [py2]

			movq			mm0, [edi]
			movq			mm2, [edi+2*eax]

			pavgb			(mm0, mm2)

			movq			mm1, [ebx + 2 * ecx]
			movq			mm2, [edi + eax]

			// mm0, mm1, mm2
			movq			mm3, mm2
			pmaxub		(mm2, mm0)
			pmaxub		(mm0, mm1)
			pmaxub		(mm3, mm1)

			// mm0, mm2, mm3
			pminub		(mm3, mm0)
			pminub		(mm2, mm3)

//////////////
			mov			eax, [puv]
			movq			mm6, [eax + 2 * ecx]


			movq			mm4, mm2
			pand			mm4, [masklow]
			psrlw			mm2, 8
			pavgb			(mm2, mm4)

			pxor			mm7, mm7


			pshufw		(mm1, mm6, 0xB1)
			pavgb			(mm1, mm6)
			pshufw		(mm1, mm1, 0xD0)
			punpckhbw	mm1, mm7
			psllw			mm1, 8


			por			mm2, mm1

//////////////

			pxor			mm0, mm0

			movq			mm1, mm2
			punpcklbw	mm2, mm0
			punpckhbw	mm1, mm0

			mov			ebx, [po]

			movzx			eax, BYTE PTR [ebx + ecx]
			movq			mm3, [edx + eax * 8 + 2048 + 4096]
			movq			mm4, [edx + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ebx + ecx+1]
			paddw			mm3, [edx + eax * 8 + 2048 + 8192]
			paddw			mm4, [edx + eax * 8 + 8192]

			pmullw		mm2, mm3

			paddw			mm2, mm4

			movzx			eax, BYTE PTR [ebx + ecx + 2]
			movq			mm3, [edx + eax * 8 + 2048 + 4096]
			movq			mm4, [edx + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ebx + ecx + 3]
			paddw			mm3, [edx + eax * 8 + 2048 + 8192]
			paddw			mm4, [edx + eax * 8 + 8192]

			pmullw		mm1, mm3

			paddw			mm1, mm4

			psrlw			mm2, 5

			psrlw			mm1, 5

			packuswb		mm2, mm1

			mov			eax, [pm]
			movq			[eax + 2 * ecx], mm2

			add			edi, 8
			add			ecx, 4
			cmp			ecx, esi
			jne			loop1
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D2I4) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}



static inline void ConvertLineToYUV_P8D1M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXInt<2> round = {0x00010001, 0x00010001};

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[ppy]
			mov			edi,	[ppuv]

			lea			eax, [eax+2*ecx]
			add			ebx, ecx
			add			edx, ecx
			add			esi, ecx
			add			edi, ecx
			neg			ecx

			pxor			mm6, mm6

	loop2:

			movq			mm0, [ebx+ecx]
			movq			mm2, [esi+ecx]

			pavgb			(mm0, mm2)

			movq			mm2, mm0

			movq			mm1, [edx+ecx]
			movq			mm3, [edi+ecx]

			pavgb			(mm1, mm3)

			punpcklbw	mm0, mm1

			punpckhbw	mm2, mm1

			movq			[eax+2*ecx], mm0

			movq			[eax+2*ecx+8], mm2

			add			ecx, 8

			jnc			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1M) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D1M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			ecx, 0
			mov			edi, [w]
			shr			edi, 1
			mov			edx, [dd]
			mov			esi, [po]

			pxor			mm6, mm6

	loop2:
			mov			eax, [py]
			mov			ebx, [puv]

			movq			mm0, [eax + 2 * ecx]
			movq			mm2, [ebx + 2 * ecx]
			movq			mm1, mm0
			punpcklbw	mm0, mm2
			punpckhbw	mm1, mm2

			mov			eax, [ppy]
			mov			ebx, [ppuv]

			movq			mm2, [eax + 2 * ecx]
			movq			mm4, [ebx + 2 * ecx]
			movq			mm3, mm2
			punpcklbw	mm2, mm4
			punpckhbw	mm3, mm4


			movq			mm4, mm0
			movq			mm5, mm2
			punpcklbw	mm0, mm6
			punpcklbw	mm2, mm6
			pavgb			(mm0, mm2)

			movzx			eax, BYTE PTR [esi + ecx]
			pmullw		mm0, [edx + eax * 8 + 2048]
			paddw			mm0, [edx + eax * 8]

			punpckhbw	mm4, mm6
			punpckhbw	mm5, mm6
			pavgb			(mm4, mm5)

			movzx			eax, BYTE PTR [esi + ecx + 1]
			pmullw		mm4, [edx + eax * 8 + 2048]
			paddw			mm4, [edx + eax * 8]

			movq			mm2, mm1
			movq			mm5, mm3
			punpcklbw	mm1, mm6
			punpcklbw	mm3, mm6
			pavgb			(mm1, mm3)

			movzx			eax, BYTE PTR [esi + ecx + 2]
			pmullw		mm1, [edx + eax * 8 + 2048]
			paddw			mm1, [edx + eax * 8]

			punpckhbw	mm2, mm6
			punpckhbw	mm5, mm6
			pavgb			(mm2, mm5)

			// order is mm0, mm4, mm1, mm3

			movzx			eax, BYTE PTR [esi + ecx + 3]
			pmullw		mm2, [edx + eax * 8 + 2048]
			paddw			mm2, [edx + eax * 8]

			psrlw			mm0, 4
			psrlw			mm4, 4
			psrlw			mm1, 4
			psrlw			mm2, 4

			packuswb		mm0, mm4
			packuswb		mm1, mm2

			mov			eax, [pm]

			movq			[eax + 4 * ecx], mm0

			movq			[eax + 4 * ecx+8], mm1

			add			ecx, 4
			cmp			ecx, edi
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1M) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


static inline void ConvertLineToYUV_P8D2M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};
	static MMXInt<2> round = {0x00020002, 0x00020002};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]

			mov			ebx, [py]
			mov			edx, [puv]

			mov			esi, [ppy]
			mov			edi, [ppuv]

			mov			ecx, [w]

			movq			mm6, [masklow]
			pxor			mm7, mm7

	loop2:

			/////////////////////////////////////////////////////////////////
			movq			mm0, [ebx]			//	py7 py6 py5 py4 py3 py2 py1 py0
			movq			mm4, [esi]			// pp7 pp6 pp5 pp4 pp3 pp2 pp1 pp0
			movq			mm1, [edx]			// uv7 uv6 uv5 uv4 uv3 uv2 uv1 uv0
			movq			mm2, [edi]			// pv7 pv6 pv5 pv4 pv3 pv2 pv1 pv0

			/////////////////////////////////////////////////////////////////
			movq			mm3, mm0				//	py7 py6 py5 py4 py3 py2 py1 py0
			pand			mm3, mm6				//	    py6     py4     py2     py0
			psrlw			mm0, 8				//	    py7     py5     py3     py1
			pavgb			(mm0, mm3)

			movq			mm3, mm4				// pp7 pp6 pp5 pp4 pp3 pp2 pp1 pp0
			pand			mm3, mm6				//     pp6     pp4     pp2     pp0
			psrlw			mm4, 8				//     pp7     pp5     pp3     pp1
			pavgb			(mm4, mm3)

			pavgb			(mm0, mm4)
			/////////////////////////////////////////////////////////////////

													// uv7 uv6 uv5 uv4 uv3 uv2 uv1 uv0 (mm1)
			pshufw		(mm3, mm1, 0xB1)	// uv5 uv4 uv7 uv6 uv1 uv0 uv3 uv2
			pavgb			(mm1, mm3)			// u75 u64 u57 u46 u31 u20 u13 u02
			pshufw		(mm1, mm1, 0xD0)	// u75 u64 u31 u20 xxx xxx xxx xxx
			punpckhbw	mm1, mm7				//     u75     u64     u31     u20


			pshufw		(mm3, mm2, 0xB1)
			pavgb			(mm2, mm3)
			pshufw		(mm2, mm2, 0xD0)
			punpckhbw	mm2, mm7

			pavgb			(mm1, mm2)

			psllw			mm1, 8

			por			mm0, mm1

			movq			[eax], mm0

			add			ebx, 8
			add			edx, 8
			add			eax, 8
			add			esi, 8
			add			edi, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2M) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

static inline void ConvertLineToYUVThroughOSD_P8D2M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};
	static MMXInt<2> round = {0x00020002, 0x00020002};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			esi, [po]

			mov			ebx, [py]
			mov			edx, [ppy]

			xor			ecx, ecx

			movq			mm6, [masklow]
			movq			mm5, [round]
			pxor			mm7, mm7

	loop2:
			movq			mm0, [ebx+ecx]

			movq			mm4, [edx+ecx]

			movq			mm3, mm0
			pand			mm3, mm6
			psrlw			mm0, 8
			pavgb			(mm0, mm3)

			mov			edi, [puv]
			movq			mm1, [edi+ecx]

			mov			edi, [ppuv]
			movq			mm2, [edi+ecx]

			movq			mm3, mm4
			pand			mm3, mm6
			psrlw			mm4, 8
			pavgb			(mm4, mm3)

			pavgb			(mm0, mm4)


			pshufw		(mm3, mm1, 0xB1)
			pavgb			(mm1, mm3)
			pshufw		(mm1, mm1, 0xD0)
			punpckhbw	mm1, mm7


			pshufw		(mm3, mm2, 0xB1)
			pavgb			(mm2, mm3)
			pshufw		(mm2, mm2, 0xD0)
			punpckhbw	mm2, mm7


			pavgb			(mm1, mm2)
			psllw			mm1, 8

			por			mm0, mm1

			movq			mm1, mm0
			punpcklbw	mm0, mm7
			punpckhbw	mm1, mm7

			mov			edi, [dd]

			movzx			eax, BYTE PTR [esi]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [esi+1]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm0, mm3

			paddw			mm0, mm4

			movzx			eax, BYTE PTR [esi + 2]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [esi + 3]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm1, mm3

			paddw			mm1, mm4

			psrlw			mm0, 5

			psrlw			mm1, 5

			packuswb		mm0, mm1


			mov			edi, [pm]
			movq			[edi+ecx], mm0

			add			esi, 4
			add			ecx, 8
			cmp			ecx, [w]
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D2M) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


static inline void ConvertLineToYUVThroughOSD_P8D1MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		BYTE * final = py + w;

		__asm
			{
			mov			esi,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			edi,  [dd]
			mov			ecx,  [po]

			pxor			mm7, mm7

	loop2:
			mov			eax,	[bpr]

			movq			mm0, [ebx]
			movq			mm1, [ebx + eax]
			movq			mm2, [ebx + 2 * eax]
			pavgb			(mm0, mm2)
			pavgb			(mm1, mm0)
			movq			mm0, mm1
			punpcklbw	mm0, mm7
			punpckhbw	mm1, mm7

			movq			mm4, [edx]
			movq			mm5, [edx + eax]
			movq			mm6, [edx + 2 * eax]
			pavgb			(mm4, mm6)
			pavgb			(mm5, mm4)
			movq			mm4, mm5
			punpcklbw	mm4, mm7
			punpckhbw	mm5, mm7

			movq			mm2, mm0
			movq			mm3, mm1
			punpcklwd	mm0, mm4
			punpckhwd	mm2, mm4
			punpcklwd	mm1, mm5
			punpckhwd	mm3, mm5

			movzx			eax, BYTE PTR [ecx]
			pmullw		mm0, [edi + eax * 8 + 2048]
			paddw			mm0, [edi + eax * 8]

			movzx			eax, BYTE PTR [ecx + 1]
			pmullw		mm2, [edi + eax * 8 + 2048]
			paddw			mm2, [edi + eax * 8]

			movzx			eax, BYTE PTR [ecx + 2]
			pmullw		mm1, [edi + eax * 8 + 2048]
			paddw			mm1, [edi + eax * 8]

			movzx			eax, BYTE PTR [ecx + 3]
			pmullw		mm3, [edi + eax * 8 + 2048]
			paddw			mm3, [edi + eax * 8]

			psrlw			mm0, 4
			psrlw			mm2, 4
			psrlw			mm1, 4
			psrlw			mm3, 4

			packuswb		mm0, mm2
			packuswb		mm1, mm3

			movq			[esi], mm0
			movq			[esi+8], mm1

			add			esi, 16
			add			ebx, 8
			add			edx, 8
			add			ecx, 4
			cmp			ebx, [final]
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D1MV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


// new       -> 01/26/2000
// optimized -> 02/09/2000
static inline void ConvertLineToYUVThroughOSD_P8D1MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int w, BYTE * po, DDWORD * dd)
	{
	if (w)
		{
		__asm
			{
			xor			ecx, ecx
			mov			ebx, [po]
			mov			edx, [pm]

			pxor			mm7, mm7

	loop2:
			mov			esi, [py0]
			mov			edi, [py1]
			mov			eax, [py2]

			movq			mm0, [edi+ecx]
			movq			mm1, [esi+ecx]
			movq			mm2, [eax+ecx]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			mov			esi, [puv0]
			mov			edi, [puv1]
			mov			eax, [puv2]

			movq			mm4, [edi+ecx]
			movq			mm5, [esi+ecx]
			movq			mm6, [eax+ecx]
			pavgb			(mm5, mm6)
			pavgb			(mm4, mm5)

			movq			mm1, mm0
			punpcklbw	mm0, mm4
			punpckhbw	mm1, mm4
			movq			mm2, mm0
			punpcklbw	mm0, mm7
			punpckhbw	mm2, mm7
			movq			mm3, mm1
			punpcklbw	mm1, mm7
			punpckhbw	mm3, mm7

			mov			edi, [dd]

			movzx			eax, BYTE PTR [ebx]
			pmullw		mm0, [edi + eax * 8 + 2048]
			paddw			mm0, [edi + eax * 8]

			movzx			eax, BYTE PTR [ebx + 1]
			pmullw		mm2, [edi + eax * 8 + 2048]
			paddw			mm2, [edi + eax * 8]

			movzx			eax, BYTE PTR [ebx + 2]
			pmullw		mm1, [edi + eax * 8 + 2048]
			paddw			mm1, [edi + eax * 8]

			movzx			eax, BYTE PTR [ebx + 3]
			pmullw		mm3, [edi + eax * 8 + 2048]
			paddw			mm3, [edi + eax * 8]

			psrlw			mm0, 4
			psrlw			mm2, 4
			psrlw			mm1, 4
			psrlw			mm3, 4

			packuswb		mm0, mm2
			packuswb		mm1, mm3

			movq			[edx], mm0
			movq			[edx+8], mm1

			add			edx, 16
			add			ebx, 4
			add			ecx, 8
			cmp			ecx, [w]
			jne			loop2
			}
		}

	}
// <- optimized 02/09/2000
// <- new       01/26/2000

static inline void ConvertLineToYUV_P8D1MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[bpr]

			pxor			mm6, mm6

	loop2:

			movq			mm1, [ebx]
			movq			mm0, [ebx + esi]
			movq			mm2, [ebx + 2 * esi]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			movq			mm1, [edx]
			movq			mm4, [edx + esi]
			movq			mm7, [edx + 2 * esi]
			pavgb			(mm1, mm7)
			pavgb			(mm4, mm1)

			movq			mm1, mm0
			punpcklbw	mm0, mm4
			punpckhbw	mm1, mm4

			movq			[eax], mm0
			movq			[eax+8], mm1

			add			eax, 16
			add			ebx, 8
			add			edx, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D1MV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}


// new        -> 01/26/2000
// optimized  -> 02/09/2000
static inline void ConvertLineToYUV_P8D1MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int w)
	{
	if (w)
		{
		__asm
			{
			xor			ebx,	ebx
			mov			eax,	[pm]

			pxor			mm6, mm6

	loop2:
			mov			esi, [py0]
			mov			edi, [py1]
			mov			edx, [py2]

			movq			mm0, [edi + ebx]
			movq			mm1, [esi + ebx]
			movq			mm2, [edx + ebx]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			mov			esi, [puv0]
			mov			edi, [puv1]
			mov			edx, [puv2]

			movq			mm4, [edi + ebx]
			movq			mm5, [esi + ebx]
			movq			mm6, [edx + ebx]
			pavgb			(mm5, mm6)
			pavgb			(mm4, mm5)

			movq			mm1, mm0
			punpcklbw	mm0, mm4
			punpckhbw	mm1, mm4

			movq			[eax+2*ebx], mm0
			movq			[eax+2*ebx+8], mm1

			add			ebx, 8
			cmp			ebx, [w]
			jne			loop2
			}
		}

	}
// <- optimized 02/09/2000
// <- new 01/26/2000

static inline void ConvertLineToYUVThroughOSD_P8D2MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int w, BYTE * po, DDWORD * dd) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		BYTE * final = py + w;

		__asm
			{
			mov			esi,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			edi,  [dd]
			mov			ecx,  [po]

			pxor			mm7, mm7
			movq			mm6, [masklow]

	loop2:
			mov			eax,	[bpr]

			movq			mm1, [ebx]
			movq			mm0, [ebx + eax]
			movq			mm2, [ebx + 2 * eax]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			movq			mm5, [edx]
			movq			mm4, [edx + eax]
			movq			mm3, [edx + 2 * eax]
			pavgb			(mm5, mm3)
			pavgb			(mm4, mm5)

			movq			mm1, mm0
			pand			mm1, mm6
			psrlw			mm0, 8
			pavgb			(mm0, mm1)

			punpckldq	mm1, mm4
			punpckhwd	mm1, mm4

			movq			mm2, mm1
			punpcklbw	mm1, mm7
			punpckhbw	mm2, mm7
			pavgb			(mm1, mm2)

			psllw			mm1, 8

			por			mm0, mm1

//////////////

			movq			mm1, mm0
			punpcklbw	mm0, mm7
			punpckhbw	mm1, mm7

			movzx			eax, BYTE PTR [ecx]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ecx+1]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm0, mm3

			paddw			mm0, mm4

			movzx			eax, BYTE PTR [ecx + 2]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ecx + 3]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm1, mm3

			paddw			mm1, mm4

			psrlw			mm0, 5

			psrlw			mm1, 5

			packuswb		mm0, mm1


			movq			[esi], mm0

			add			esi, 8
			add			ebx, 8
			add			edx, 8
			add			ecx, 4
			cmp			ebx, [final]
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT ThroughOSD_P8D2MV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

// new        -> 01/26/2000
// optimized  -> 02/09/2000
static inline void ConvertLineToYUVThroughOSD_P8D2MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int w, BYTE * po, DDWORD * dd)
	{
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			xor			ecx, ecx
			mov			ebx, [po]
			mov			edx, [pm]

			pxor			mm7, mm7
			movq			mm6, [masklow]

	loop2:
			mov			esi, [py0]
			mov			edi, [py1]
			mov			eax, [py2]

			movq			mm0, [edi+ecx]
			movq			mm1, [esi+ecx]
			movq			mm2, [eax+ecx]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			mov			esi, [puv0]
			mov			edi, [puv1]
			mov			eax, [puv2]

			movq			mm4, [edi+ecx]
			movq			mm5, [esi+ecx]
			movq			mm3, [eax+ecx]
			pavgb			(mm5, mm3)
			pavgb			(mm4, mm5)

			movq			mm1, mm0
			pand			mm1, mm6
			psrlw			mm0, 8
			pavgb			(mm0, mm1)

			punpckldq	mm1, mm4
			punpckhwd	mm1, mm4

			movq			mm2, mm1
			punpcklbw	mm1, mm7
			punpckhbw	mm2, mm7
			pavgb			(mm1, mm2)

			psllw			mm1, 8

			por			mm0, mm1

//////////////

			movq			mm1, mm0
			punpcklbw	mm0, mm7
			punpckhbw	mm1, mm7
			mov			edi, [dd]

			movzx			eax, BYTE PTR [ebx]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ebx+1]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm0, mm3

			paddw			mm0, mm4

			movzx			eax, BYTE PTR [ebx + 2]
			movq			mm3, [edi + eax * 8 + 2048 + 4096]
			movq			mm4, [edi + eax * 8 + 4096]
			movzx			eax, BYTE PTR [ebx + 3]
			paddw			mm3, [edi + eax * 8 + 2048 + 8192]
			paddw			mm4, [edi + eax * 8 + 8192]

			pmullw		mm1, mm3

			paddw			mm1, mm4

			psrlw			mm0, 5

			psrlw			mm1, 5

			packuswb		mm0, mm1


			movq			[edx], mm0

			add			edx, 8
			add			ebx, 4
			add			ecx, 8
			cmp			ecx, [w]
			jne			loop2

			}
		}
	}
// <- optimized 02/09/2000
// <- new       01/26/2000


static inline void ConvertLineToYUV_P8D2MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int w) // XMMX
	{
#if MEASURE_DEINTERLACE_TIME
	static __int64 dctSumTime;
	static bool initial = true;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]
			mov			esi,	[bpr]

			movq			mm7, [masklow]
			pxor			mm6, mm6

	loop2:

			movq			mm1, [ebx]
			movq			mm0, [ebx + esi]
			movq			mm2, [ebx + 2 * esi]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			movq			mm5, [edx]
			movq			mm4, [edx + esi]
			movq			mm3, [edx + 2 * esi]
			pavgb			(mm5, mm3)
			pavgb			(mm4, mm5)

			movq			mm1, mm0
			pand			mm1, mm7
			psrlw			mm0, 8
			pavgb			(mm0, mm1)

			punpckldq	mm1, mm4
			punpckhwd	mm1, mm4

			movq			mm2, mm1
			punpcklbw	mm1, mm6
			punpckhbw	mm2, mm6
			pavgb			(mm1, mm2)

			psllw			mm1, 8

			por			mm0, mm1

			movq			[eax], mm0

			add			eax, 8
			add			ebx, 8
			add			edx, 8
			sub			ecx, 8
			jne			loop2
			}
		}
#if MEASURE_DEINTERLACE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			if (initial)
				{
				dctSumTime = 0;
				dctCount = 0;
				initial = false;
				}
			else
				{
				char buffer[100];
				wsprintf(buffer, "(OPT P8D2MV) SCALE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
				OutputDebugString(buffer);
				}
			}
#endif
	}

// new        -> 01/26/2000
// optimized  -> 02/09/2000
static inline void ConvertLineToYUV_P8D2MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int w)
	{
	static MMXQShort masklow = {0x00ff, 0x00ff, 0x00ff, 0x00ff};

	if (w)
		{
		__asm
			{
			xor			ebx,	ebx
			mov			eax,	[pm]

			movq			mm7, [masklow]
			pxor			mm6, mm6

	loop2:
			mov			esi, [py0]
			mov			edi, [py1]
			mov			edx, [py2]

			movq			mm0, [edi+ebx]
			movq			mm1, [esi+ebx]
			movq			mm2, [edx+ebx]
			pavgb			(mm1, mm2)
			pavgb			(mm0, mm1)

			mov			esi, [puv0]
			mov			edi, [puv1]
			mov			edx, [puv2]


			movq			mm4, [edi + ebx]
			movq			mm5, [esi + ebx]
			movq			mm3, [edx + ebx]
			pavgb			(mm5, mm3)
			pavgb			(mm4, mm5)


			movq			mm1, mm0
			pand			mm1, mm7
			psrlw			mm0, 8
			pavgb			(mm0, mm1)

			punpckldq	mm1, mm4
			punpckhwd	mm1, mm4

			movq			mm2, mm1
			punpcklbw	mm1, mm6
			punpckhbw	mm2, mm6
			pavgb			(mm1, mm2)
			psllw			mm1, 8

			por			mm0, mm1

			movq			[eax+ebx], mm0

			add			ebx, 8
			cmp			ebx, [w]
			jne			loop2
			}

		}
	}
// <- optimized 02/09/2000
// <- new       01/26/2000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// NON XMMX optimized  YUV converter routines
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// seven bit conversion routines
//

static inline void ConvertLineToYUV_P7D1(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]

			lea			eax, [eax+2*ecx]
			add			ebx, ecx
			add			edx, ecx
			neg			ecx

			movq			mm0, [ebx+ecx]

			movq			mm1, [edx+ecx]
			psllq			mm0, 1

			psllq			mm1, 1
			add			ecx, 8
			je				done2
	loop2:
			movq			mm2, mm0
			punpcklbw	mm0, mm1

			movq			mm3, [ebx+ecx]
			punpckhbw	mm2, mm1

			movq			[eax+2*ecx-16], mm0
			movq			mm0, mm3

			movq			mm1, [edx+ecx]
			psllq			mm0, 1

			movq			[eax+2*ecx-8], mm2
			psllq			mm1, 1

			add			ecx, 8
			jne			loop2
	done2:
			movq			mm2, mm0
			punpcklbw	mm0, mm1

			movq			[eax+2*ecx-16], mm0
			punpckhbw	mm2, mm1

			movq			[eax+2*ecx-8], mm2
			}
		}
	}

static inline void ConvertLineToYUVThroughOSD_P7D1(BYTE * py, BYTE * puv, BYTE * pm, int w,
																	BYTE * po, DDWORD * dd)
	{
	__asm
		{
		mov			esi, [pm]
		mov			ebx, [py]
		mov			edx, [puv]
		mov			ecx, [w]
		mov			edi, [po]

		push			ebp
		mov			ebp, [dd]

		shr			ecx, 1

		lea			esi, [esi+4*ecx]
		lea			ebx, [ebx+2*ecx]
		lea			edx, [edx+2*ecx]
		lea			edi, [edi+ecx]
		neg			ecx

		pxor			mm7, mm7
loop2:
		movq			mm0, [ebx+2*ecx]

		movq			mm1, [edx+2*ecx]
		psllq			mm0, 1

		psllq			mm1, 1
		movq			mm2, mm0

		punpcklbw	mm0, mm1
		xor			eax, eax

		punpckhbw	mm2, mm1
		mov			al, [edi+ecx]

		movq			mm1, mm0
		punpcklbw	mm0, mm7

		pmullw		mm0, [ebp + eax * 8 + 2048]
		punpckhbw	mm1, mm7

		movq			mm4, [ebp + eax * 8]
		movq			mm3, mm2

		punpcklbw	mm2, mm7
		xor			eax, eax

		paddw			mm0, mm4
		mov			al, [edi+ecx+1]

		punpckhbw	mm3, mm7
		psrlw			mm0, 4

		pmullw		mm1, [ebp + eax * 8 + 2048]

		movq			mm5, [ebp + eax * 8]
		//

		xor			eax, eax
		//

		paddw			mm1, mm5
		mov			al, [edi+ecx+2]

		psrlw			mm1, 4
		//

		pmullw		mm2, [ebp + eax * 8 + 2048]
		packuswb		mm0, mm1

		movq			mm5, [ebp + eax * 8]
		//

		movq			[esi+4*ecx], mm0
		//

		paddw			mm2, mm5
		xor			eax, eax

		mov			al, [edi+ecx+3]

		pmullw		mm3, [ebp + eax * 8 + 2048]

		paddw			mm3, [ebp + eax * 8]
		psrlw			mm2, 4

		psrlw			mm3, 4

		packuswb		mm2, mm3

		movq			[esi+4*ecx+8], mm2

		add			ecx, 4
		jne			loop2

		pop			ebp
		}
	}

static inline void ConvertLineToYUV_P7D2(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	static MMXDWORD<2> shlmsk2y = {0x00ff00ff, 0x00ff00ff};
	static MMXDWORD<2> shlmsk2u = {0x0000ff00, 0x0000ff00};
	static MMXDWORD<2> shlmsk2v = {0xff000000, 0xff000000};

	if (w)
		{
		__asm
			{
			mov	eax,	[pm]
			mov	ebx,	[py]
			mov	edx,	[puv]
			mov	ecx,  [w]

			add	eax, ecx
			add	ebx, ecx
			add	edx, ecx
			neg	ecx

			movq	mm7, [shlmsk2y]
			movq	mm6, [shlmsk2u]
			movq	mm5, [shlmsk2v]

			movq	mm0, [ebx+ecx]

			movq	mm1, [edx+ecx]
			movq	mm3, mm0

			movq	mm2, mm1
			movq	mm4, mm1

			pand	mm0, mm7
			psllq	mm1, 8

			psllq	mm2, 16
			pand	mm1, mm6

			pand	mm2, mm5
			por	mm0, mm1

			por	mm2, mm0
			psrlq	mm3, 8

			pand	mm3, mm7
			add	ecx, 8

			movq	mm1, mm4
			psrlq	mm4, 8

			pand	mm4, mm6
			pand	mm1, mm5

			por	mm3, mm4
			je		done1
loop1:
			movq	mm0, [ebx+ecx]
			por	mm3, mm1

			movq	mm1, [edx+ecx]
			paddb	mm2, mm3

			movq	mm3, mm0
			movq	mm4, mm1

			movq	[eax+ecx-8], mm2
			movq	mm2, mm1

			pand	mm0, mm7
			psllq	mm1, 8

			psllq	mm2, 16
			pand	mm1, mm6

			pand	mm2, mm5
			por	mm0, mm1

			por	mm2, mm0
			psrlq	mm3, 8

			movq	mm1, mm4
			psrlq	mm4, 8

			pand	mm3, mm7
			pand	mm4, mm6

			pand	mm1, mm5
			add	ecx, 8

			por	mm3, mm4
			jne	loop1
done1:
			por	mm3, mm1

			paddb	mm2, mm3

			movq	[eax+ecx-8], mm2
			}
		}
	}

static inline void ConvertLineToYUVThroughOSD_P7D2(BYTE * py, BYTE * puv, BYTE * pm, int w,
	                                                BYTE * po, DDWORD * dd)
	{
	static MMXDWORD<2> shlmsk2y = {0x00ff00ff, 0x00ff00ff};
	static MMXDWORD<2> shlmsk2u = {0x0000ff00, 0x0000ff00};
	static MMXDWORD<2> shlmsk2v = {0xff000000, 0xff000000};

	if (w)
		{
		__asm
			{
			mov	esi,	[pm]
			mov	ebx,	[py]
			mov	edx,	[puv]
			mov	ecx,  [w]

			add	esi, ecx
			add	ebx, ecx
			add	edx, ecx
			neg	ecx

			movq	mm7, [shlmsk2y]
			movq	mm6, [shlmsk2u]
			movq	mm5, [shlmsk2v]

loop1:
			movq	mm0, [ebx+ecx]

			movq	mm1, [edx+ecx]
			movq	mm3, mm0

			movq	mm2, mm1
			movq	mm4, mm1

			pand	mm0, mm7

			psllq	mm1, 8

			psllq	mm2, 16
			pand	mm1, mm6

			pand	mm2, mm5
			por	mm0, mm1

			por	mm2, mm0

			psrlq	mm3, 8

			pand	mm3, mm7

			movq	mm1, mm4
			psrlq	mm4, 8

			pand	mm4, mm6
			pand	mm1, mm5

			por	mm3, mm4
			por	mm3, mm1

			paddb	mm2, mm3

			movq	[esi+ecx], mm2

			add	ecx, 8
			jne	loop1
			}
		}

	__asm
		{
		mov			esi, [pm]
		mov			edi, [po]
		mov			ebx, [dd]
		mov			ecx, [w]

		shr	ecx, 1

		lea	esi, [esi+2*ecx]
		lea	edi, [edi+ecx]
		neg	ecx

		pxor			mm0, mm0
loop2:
		movq			mm1, [esi+2*ecx]

		movq			mm2, mm1
		punpcklbw	mm1, mm0

		punpckhbw	mm2, mm0
		xor			eax, eax

		mov			al, [edi+ecx]

		movq			mm3, [ebx + eax * 8 + 2048 + 4096]

		movq			mm4, [ebx + eax * 8 + 4096]

		xor			eax, eax

		mov			al, [edi+ecx+1]

		paddw			mm3, [ebx + eax * 8 + 2048 + 8192]

		paddw			mm4, [ebx + eax * 8 + 8192]

		pmullw		mm1, mm3

		paddw			mm1, mm4

		xor			eax, eax

		mov			al, [edi+ecx+2]

		movq			mm3, [ebx + eax * 8 + 2048 + 4096]

		movq			mm4, [ebx + eax * 8 + 4096]

		xor			eax, eax

		mov			al, [edi+ecx+3]

		paddw			mm3, [ebx + eax * 8 + 2048 + 8192]

		paddw			mm4, [ebx + eax * 8 + 8192]

		pmullw		mm2, mm3

		paddw			mm2, mm4

		psrlw			mm1, 5

		psrlw			mm2, 5

		packuswb		mm1, mm2

		movq			[esi+2*ecx], mm1

		add			ecx, 4
		jne			loop2
		}
	}

static inline void ConvertLineToYUV_P7DS(BYTE * py, BYTE * puv, BYTE * pm, int w, int s)
	{
	int x;
	int xs;

	xs = 1 << s;

	for(x = 0; x < w; x += 2 * xs)
		{
		*pm++ = py[x] << 1;
		*pm++ = puv[x] << 1;
		*pm++ = py[x+xs] << 1;
		*pm++ = puv[x+xs+1] << 1;
		}
	}

static inline void ConvertLineToYUVThroughOSD_P7DS(BYTE * py, BYTE * puv, BYTE * pm, int w,
																	BYTE * po, DDWORD * dd, int s)
	{
	ConvertLineToYUV_P7DS(py, puv, pm, w, s);
	}

///////////////////////////////////////////////////////////////////////////////

void XMMXYUVVideoConverter::ConvertStripeToYUV_P7DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P7D1(lpy, lpuv, pm, p.width);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		case 1:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P7D2(lpy, lpuv, pm, p.width);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P7DS(lpy, lpuv, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P7DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUV_P7D1(lpy, lpuv, pm, p.xosdstart);
					ConvertLineToYUVThroughOSD_P7D1(lpy + p.xosdstart, lpuv + p.xosdstart, pm + (p.xosdstart << 1), p.xosdstop - p.xosdstart, lpo, p.dd);
					ConvertLineToYUV_P7D1(lpy + p.xosdstop, lpuv + p.xosdstop, pm + (p.xosdstop << 1), p.width - p.xosdstop);
					}
				else
					{
					ConvertLineToYUV_P7D1(lpy, lpuv, pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		case 1:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUV_P7D2(lpy, lpuv, pm, p.xosdstart);
					ConvertLineToYUVThroughOSD_P7D2(lpy + p.xosdstart, lpuv + p.xosdstart, pm + p.xosdstart, p.xosdstop - p.xosdstart, lpo, p.dd);
					ConvertLineToYUV_P7D2(lpy + p.xosdstop, lpuv + p.xosdstop, pm + p.xosdstop, p.width - p.xosdstop);
					}
				else
					{
					ConvertLineToYUV_P7D2(lpy, lpuv, pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUV_P7DS(lpy, lpuv, pm, p.xosdstart, p.xscale);
					ConvertLineToYUVThroughOSD_P7DS(lpy + p.xosdstart, lpuv + p.xosdstart, pm + (p.xosdstart >> (p.xscale - 1)), p.xosdstop - p.xosdstart, lpo, p.dd, p.xscale);
					ConvertLineToYUV_P7DS(lpy + p.xosdstop, lpuv + p.xosdstop, pm + (p.xosdstop >> (p.xscale - 1)), p.width - p.xosdstop, p.xscale);
					}
				else
					{
					ConvertLineToYUV_P7DS(lpy, lpuv, pm, p.width, p.xscale);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
		}
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines without scaling
//

//
// Convert a pair of single Y and UV into a YUY2 line
//
static inline void ConvertLineToYUV_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w)
	{
	if (w)
		{
		__asm
			{
			mov			eax,	[pm]
			mov			ebx,	[py]
			mov			edx,	[puv]
			mov			ecx,  [w]

			lea			eax, [eax+2*ecx]
			add			ebx, ecx
			add			edx, ecx
			neg			ecx

			movq			mm0, [ebx+ecx]

			movq			mm1, [edx+ecx]

			add			ecx, 8
			je				done
	loop2:
			movq			mm2, mm0
			punpcklbw	mm0, mm1

			movq			mm3, [ebx+ecx]
			punpckhbw	mm2, mm1

			movq			[eax+2*ecx-16], mm0
			movq			mm0, mm3

			movq			mm1, [edx+ecx]

			movq			[eax+2*ecx-8], mm2

			add			ecx, 8
			jne			loop2
	done:

			movq			mm2, mm0
			punpcklbw	mm0, mm1

			movq			[eax+2*ecx-16], mm0
			punpckhbw	mm2, mm1

			movq			[eax+2*ecx-8], mm2
			}
		}
	}



//
// Convert a pair of single Y and UV into a YUY2 line, and merging with an osd
// bitmap
//
static inline void ConvertLineToYUVThroughOSD_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int w,
																	BYTE * po, DDWORD * dd)
	{
	__asm
		{
		mov			esi, [pm]
		mov			ebx, [py]
		mov			edx, [puv]
		mov			ecx, [w]
		mov			edi, [po]

		push			ebp
		mov			ebp, [dd]

		shr			ecx, 1

		lea			esi, [esi+4*ecx]
		lea			ebx, [ebx+2*ecx]
		lea			edx, [edx+2*ecx]
		lea			edi, [edi+ecx]
		neg			ecx

		pxor			mm7, mm7
loop2:
		movq			mm0, [ebx+2*ecx]

		movq			mm1, [edx+2*ecx]

		movq			mm2, mm0

		punpcklbw	mm0, mm1
		xor			eax, eax

		punpckhbw	mm2, mm1
		mov			al, [edi+ecx]

		movq			mm1, mm0
		punpcklbw	mm0, mm7

		pmullw		mm0, [ebp + eax * 8 + 2048]
		punpckhbw	mm1, mm7

		movq			mm4, [ebp + eax * 8]
		movq			mm3, mm2

		punpcklbw	mm2, mm7
		xor			eax, eax

		paddw			mm0, mm4
		mov			al, [edi+ecx+1]

		punpckhbw	mm3, mm7
		psrlw			mm0, 4

		pmullw		mm1, [ebp + eax * 8 + 2048]

		movq			mm5, [ebp + eax * 8]
		//

		xor			eax, eax
		//

		paddw			mm1, mm5
		mov			al, [edi+ecx+2]

		psrlw			mm1, 4
		//

		pmullw		mm2, [ebp + eax * 8 + 2048]
		packuswb		mm0, mm1

		movq			mm5, [ebp + eax * 8]
		//

		movq			[esi+4*ecx], mm0
		//

		paddw			mm2, mm5
		xor			eax, eax

		mov			al, [edi+ecx+3]

		pmullw		mm3, [ebp + eax * 8 + 2048]

		paddw			mm3, [ebp + eax * 8]
		psrlw			mm2, 4

		psrlw			mm3, 4

		packuswb		mm2, mm3

		movq			[esi+4*ecx+8], mm2

		add			ecx, 4
		jne			loop2

		pop			ebp
		}
	}

static inline void ConvertLineToYUVWithOSD_P8D1(BYTE * py, BYTE * puv, BYTE * pm, int width,
																int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1(py + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1(py + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop);
	}



static inline void ConvertLineToYUVWithOSD_P8D1MUV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width,
																   int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1MUV(py, puv, pm, xosdstart, bpr);
	ConvertLineToYUVThroughOSD_P8D1MUV(py + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, bpr, po, dd);
	ConvertLineToYUV_P8D1MUV(py + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop, bpr);
	}

static inline void ConvertLineToYUVWithOSD_P8D1MY(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width,
																   int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1(py + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1(py + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop);
	}

static inline void ConvertLineToYUVWithOSD_P8D1MYUV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width,
																   int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1MUV(py, puv, pm, xosdstart, bpr);
	ConvertLineToYUVThroughOSD_P8D1MUV(py + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, bpr, po, dd);
	ConvertLineToYUV_P8D1MUV(py + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop, bpr);
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with binary x scaling and no y scaling
//











static inline void ConvertLineToYUVWithOSD_P8D2(BYTE * py, BYTE * puv, BYTE * pm, int width,
																int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2(py + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop);
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with linear x scaling and no y scaling
//

//
// convert a pair of single y and uv lines into a yuy2 line, by using linear
// scaling.
//
static inline void ConvertLineToYUV_P8F2(BYTE * py, BYTE * puv, BYTE * pm, int w, int xcount, int xfraction)
	{
	static MMXQUShort negate = {0x7fff, 0x0000, 0x7fff, 0x0000};
	static MMXQUShort fullneg = {0xffff, 0x0000, 0xffff, 0x0000};
	static MMXQUShort mask15 = {0x7fff, 0x7fff, 0x7fff, 0x7fff};

	if (w)
		{
		__asm
			{
			mov			eax, [pm]
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			mov			esi, [xcount]
			push			ebp
			mov			ebp, [xfraction]

			movd			mm6, esi
			movd			mm7, ebp
			paddd			mm7, mm6

			punpcklwd	mm6, mm7
			punpcklwd	mm6, mm6
			psrlw			mm6, 1
			pxor			mm6, [negate]

			movd			mm7, ebp
			punpcklwd	mm7, mm7
			punpcklwd	mm7, mm7
			pxor			mm7, [fullneg]
			psubw			mm7, [fullneg]

			pxor			mm5, mm5
			movq			mm4, [mask15]
	loop2:
			mov			edi, esi
			shr			esi, 16

			movd			mm0, [ebx+esi]
			add			edi, ebp

			and			esi, 0xfffffffe
			movd			mm3, [edx+esi]

			mov			esi, edi
			shr			edi, 16

			movd			mm1, [ebx+edi]
			add			esi, ebp

			punpcklwd	mm0, mm1
			punpcklbw	mm0, mm5

			pmaddwd		mm0, mm6
			psrld			mm0, 15

			paddw			mm6, mm7
			pand			mm6, mm4

			mov			edi, esi
			shr			esi, 16

			movd			mm2, [ebx+esi]
			add			edi, ebp

			and			esi, 0xfffffffe
			movd			mm1, [edx+esi]
			punpcklwd	mm3, mm1

			mov			esi, edi
			shr			edi, 16

			movd			mm1, [ebx+edi]
			add			esi, ebp

			punpcklwd	mm2, mm1
			punpcklbw	mm2, mm5

			pmaddwd		mm2, mm6
			psrld			mm2, 15

			paddw			mm6, mm7
			pand			mm6, mm4

			packssdw		mm0, mm2
			packuswb		mm0, mm0

			punpcklbw	mm0, mm3

			movq			[eax], mm0

			add			eax, 8

			sub			ecx, 4
			jne			loop2

			pop			ebp
			}
		}
	}

//
// convert a pair of single y and uv lines into a yuy2 line, by using linear
// scaling, and merging with an OSD bitmap
//
static inline void ConvertLineToYUV_P8F2ThroughOSD(BYTE * py, BYTE * puv, BYTE * pm, int w, int xcount, int xfraction, BYTE * po, DDWORD * dd)
	{
	static MMXQShort negate = {0x7fff, 0x0000, 0x7fff, 0x0000};

	if (w)
		{
		__asm
			{
			mov			ebx, [py]
			mov			edx, [puv]
			mov			ecx, [w]
			xor			esi, esi
			movd			mm5, [xcount]
			movd			mm6, [xfraction]

			pxor			mm7, mm7
	loop2:
			movd			edi, mm5
			shr			edi, 16

			movd			mm0, [ebx+edi]
			movq			mm1, mm5
			paddd			mm5, mm6

			and			edi, 0xfffe
			movd			mm4, [edx+edi]

			movd			edi, mm5
			shr			edi, 16

			movd			mm2, [ebx+edi]
			movq			mm3, mm5
			paddd			mm5, mm6

			punpcklwd	mm0, mm2
			punpcklbw	mm0, mm7

			punpckldq	mm1, mm3
			psrld			mm1, 1
			pand			mm1, [negate]
			movq			mm3, mm1
			pslld			mm1, 16
			por			mm1, mm3
			pxor			mm1, [negate]

			pmaddwd		mm0, mm1
			psrld			mm0, 15

			packssdw		mm0, mm0
			packuswb		mm0, mm0
			punpcklbw	mm0, mm4

			punpcklbw	mm0, mm7
			mov			edi, [dd]
			mov			eax, [po]
			movzx			eax, BYTE PTR [eax + esi]
			pmullw		mm0, [edi + eax * 8 + 2048]
			paddw			mm0, [edi + eax * 8]
			psrlw			mm0, 4
			packuswb		mm0, mm0

			mov			eax, [pm]
			movd			[eax+4*esi], mm0

			inc			esi

			sub			ecx, 2
			jne			loop2
			}
		}
	}

static inline void ConvertLineToYUV_P8F2WithOSD(BYTE * py, BYTE * puv, BYTE * pm, int width, int xoffset, int xfraction, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8F2(py, puv, pm, xosdstart, xoffset, xfraction);
	ConvertLineToYUV_P8F2ThroughOSD(py, puv, pm + (xosdstart << 1), xosdstop - xosdstart, xoffset + xfraction * xosdstart, xfraction, po, dd);
	ConvertLineToYUV_P8F2(py, puv, pm + (xosdstop << 1), width - xosdstop, xoffset + xfraction * xosdstop, xfraction);
	}





static inline void ConvertLineToYUVWithOSD_P8D1Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int width,
																  int xosdstart, int xosdstop, BYTE * po, DDWORD * dd, int lo)
	{
	ConvertLineToYUV_P8D1Y2(py0, py1, puv, pm, xosdstart, lo);
	ConvertLineToYUVThroughOSD_P8D1Y2(py0 + xosdstart, py1 + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd, lo);
	ConvertLineToYUV_P8D1Y2(py0 + xosdstop, py1 + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop, lo);
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with binary x scaling and linear y scaling
//





static inline void ConvertLineToYUVWithOSD_P8D2Y2(BYTE * py0, BYTE * py1, BYTE * puv, BYTE * pm, int width,
																  int xosdstart, int xosdstop, BYTE * po, DDWORD * dd, int lo)
	{
	ConvertLineToYUV_P8D2Y2(py0, py1, puv, pm, xosdstart, lo);
	ConvertLineToYUVThroughOSD_P8D2Y2(py0 + xosdstart, py1 + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd, lo);
	ConvertLineToYUV_P8D2Y2(py0 + xosdstop, py1 + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop, lo);
	}



///////////////////////////////////////////////////////////////////////////////
//
// eight bit conversion routines with four tap x scaling and no y scaling
//

static inline void ConvertLineToYUV_P8F4(BYTE * py, BYTE * puv, BYTE * pm, int w, int xcount, int xfraction, MMXQShort * filter)
	{
	BYTE * final;

	xfraction >>= 6;
	xcount >>= 6;
	final = pm + 2 * ((w + 3) & ~3);

	__asm
		{
		mov			edx, [filter]

		mov			esi, [py]
		movd			mm5, [xcount]
		movd			mm6, [xfraction]
		mov			edi, [puv]
		mov			eax, [pm]

		pxor			mm7, mm7

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm0, [esi + ebx]
		punpcklbw	mm0, mm7

		and			ebx, 0xfffe
		movzx			ecx, WORD PTR [edi + ebx]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm0, [edx + ebx]
		dec			esi

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm1, [esi + ebx]
		punpcklbw	mm1, mm7

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm1, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm2, [esi + ebx]
		punpcklbw	mm2, mm7

		and			ebx, 0xfffe
		movzx			ebx, WORD PTR [edi + ebx]
		shl			ebx, 16
		or				ecx, ebx

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm2, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm3, [esi + ebx]
		punpcklbw	mm3, mm7

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm3, [edx + ebx]

		add			eax, 8

		movq			mm4, mm0
		punpckldq	mm0, mm1
		punpckhdq	mm4, mm1
		paddd			mm4, mm0

		cmp			eax, [final]

		movq			mm1, mm2
		punpckldq	mm2, mm3
		punpckhdq	mm1, mm3
		paddd			mm2, mm1

		psrad			mm4, 14
		psrad			mm2, 14
		packssdw		mm4, mm2
		packuswb		mm4, mm4

		movd			mm1, ecx
		punpcklbw	mm4, mm1

		je				done1

		sub			eax, 8
loop1:
		movd			ebx, mm5
		shr			ebx, 10
		movd			mm0, [esi + ebx]
		punpcklbw	mm0, mm7

		and			ebx, 0xfffe
		movzx			ecx, WORD PTR [edi + ebx]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm0, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10
		movd			mm1, [esi + ebx]
		punpcklbw	mm1, mm7

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm1, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm2, [esi + ebx]
		punpcklbw	mm2, mm7

		and			ebx, 0xfffe
		movzx			ebx, WORD PTR [edi + ebx]
		shl			ebx, 16
		or				ecx, ebx

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm2, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movd			mm3, [esi + ebx]
		punpcklbw	mm3, mm7

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm3, [edx + ebx]

		movq			[eax], mm4
		add			eax, 8

		movq			mm4, mm0
		punpckldq	mm0, mm1
		punpckhdq	mm4, mm1
		paddd			mm4, mm0

		cmp			eax, [final]

		movq			mm1, mm2
		punpckldq	mm2, mm3
		punpckhdq	mm1, mm3
		paddd			mm2, mm1

		psrad			mm4, 14
		psrad			mm2, 14
		packssdw		mm4, mm2
		packuswb		mm4, mm4

		movd			mm1, ecx
		punpcklbw	mm4, mm1

		jne			loop1
done1:
		movq			[eax], mm4
		}
	}





static inline void ConvertLineToYUVWithOSD_P8D1Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd, short * filter)
	{
	ConvertLineToYUV_P8D1Y4(py0, py1, py2, py3, puv, pm, xosdstart, filter);
	ConvertLineToYUVThroughOSD_P8D1Y4(py0 + xosdstart, py1 + xosdstart, py2 + xosdstart, py3 + xosdstart, puv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd, filter);
	ConvertLineToYUV_P8D1Y4(py0 + xosdstop, py1 + xosdstop, py2 + xosdstop, py3 + xosdstop, puv + xosdstop, pm + (xosdstop << 1), width - xosdstop, filter);
	}





static inline void ConvertLineToYUVWithOSD_P8D1Y4MUV(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv0, BYTE * puv1, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd, short * filter)
	{
	ConvertLineToYUV_P8D1Y4MUV(py0, py1, py2, py3, puv0, puv1, pm, xosdstart, filter);
	ConvertLineToYUVThroughOSD_P8D1Y4MUV(py0 + xosdstart, py1 + xosdstart, py2 + xosdstart, py3 + xosdstart, puv0 + xosdstart, puv1 + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd, filter);
	ConvertLineToYUV_P8D1Y4MUV(py0 + xosdstop, py1 + xosdstop, py2 + xosdstop, py3 + xosdstop, puv0 + xosdstop, puv0 + xosdstop, pm + (xosdstop << 1), width - xosdstop, filter);
	}


static inline void ConvertLineToYUV_P8D2Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int w, short * filter)
	{
	}

static inline void ConvertLineToYUVThroughOSD_P8D2Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int w, BYTE * po, DDWORD * dd, short * filter)
	{
	}

static inline void ConvertLineToYUVWithOSD_P8D2Y4(BYTE * py0, BYTE * py1, BYTE * py2, BYTE * py3, BYTE * puv, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd, short * filter)
	{
	ConvertLineToYUV_P8D2Y4(py0, py1, py2, py3, puv, pm, xosdstart, filter);
	ConvertLineToYUVThroughOSD_P8D2Y4(py0 + xosdstart, py1 + xosdstart, py2 + xosdstart, py3 + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd, filter);
	ConvertLineToYUV_P8D2Y4(py0 + xosdstop, py1 + xosdstop, py2 + xosdstop, py3 + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop, filter);
	}


///////////////////////////////////////////////////////////////////////////////
//
// eight bit deinterlacing routines
//

///////////////////////////////////////////////////////////////////////////////
//
// eight bit scaling routines
//

static inline void ScaleYUVLine_P8F4(BYTE * spm, BYTE * dpm, int w, int xcount, int xfraction, MMXQShort * filter)
	{
	static MMXQUShort lower = {0x00ff, 0x00ff, 0x00ff, 0x00ff};
	static MMXQUShort upper = {0xff00, 0xff00, 0xff00, 0xff00};

	BYTE * final;

	xfraction >>= 6;
	xcount >>= 6;
	final = dpm + 2 * ((w + 3) & ~3);

	__asm
		{
		mov			edx, [filter]

		mov			esi, [spm]
		movd			mm5, [xcount]
		movd			mm6, [xfraction]
		mov			eax, [dpm]

		movd			ebx, mm5
		shr			ebx, 10

		movq			mm0, [esi + 2 * ebx]
		pand			mm0, [lower]

		and			ebx, 0xfffe
		movd			mm7, [esi + 2 * ebx]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm0, [edx + ebx]
		sub			esi, 2

		movd			ebx, mm5
		shr			ebx, 10

		movq			mm1, [esi + 2 * ebx]
		pand			mm1, [lower]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm1, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movq			mm2, [esi + 2 * ebx]
		pand			mm2, [lower]

		and			ebx, 0xfffe
		punpckldq	mm7, [esi + 2 * ebx + 2]
		psrlw			mm7, 8

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm2, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		movq			mm3, [esi + 2 * ebx]
		pand			mm3, [lower]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm3, [edx + ebx]

		add			eax, 8

		movq			mm4, mm0
		punpckldq	mm0, mm1
		punpckhdq	mm4, mm1
		paddd			mm4, mm0

		cmp			eax, [final]

		movq			mm1, mm2
		punpckldq	mm2, mm3
		punpckhdq	mm1, mm3
		paddd			mm2, mm1

		psrad			mm4, 14
		psrad			mm2, 14
		packssdw		mm4, mm2
		packuswb		mm4, mm4
		packuswb		mm7, mm7

		punpcklbw	mm4, mm7

		je				done1

		sub			eax, 8
loop1:
		movq			mm3, [lower]
		movd			ebx, mm5
		shr			ebx, 10

		movq			mm0, [esi + 2 * ebx]
		pand			mm0, mm3
		and			ebx, 0xfffe

		movd			mm7, [esi + 2 * ebx + 2]
		movd			ebx, mm5
		and			ebx, 0x000003f8

		pmaddwd		mm0, [edx + ebx]
		paddd			mm5, mm6

		movd			ebx, mm5
		shr			ebx, 10
		movq			mm1, [esi + 2 * ebx]

		pand			mm1, mm3
		movd			ebx, mm5
		and			ebx, 0x000003f8

		pmaddwd		mm1, [edx + ebx]
		paddd			mm5, mm6

		movd			ebx, mm5
		shr			ebx, 10
		movq			mm2, [esi + 2 * ebx]

		pand			mm2, mm3

		and			ebx, 0xfffe
		punpckldq	mm7, [esi + 2 * ebx + 2]
		psrlw			mm7, 8

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm2, [edx + ebx]

		movd			ebx, mm5
		shr			ebx, 10

		pand			mm3, [esi + 2 * ebx]

		movd			ebx, mm5
		paddd			mm5, mm6
		and			ebx, 0x000003f8
		pmaddwd		mm3, [edx + ebx]

		movq			[eax], mm4
		add			eax, 8

		movq			mm4, mm0
		punpckldq	mm0, mm1
		punpckhdq	mm4, mm1
		paddd			mm4, mm0

		cmp			eax, [final]

		movq			mm1, mm2
		punpckldq	mm2, mm3
		punpckhdq	mm1, mm3
		paddd			mm2, mm1

		psrad			mm4, 14
		psrad			mm2, 14
		packssdw		mm4, mm2
		packuswb		mm4, mm4
		packuswb		mm7, mm7

		punpcklbw	mm4, mm7

		jne			loop1
done1:
		movq			[eax], mm4
		}
	}





static inline void ScaleYUVLine_P8F2ThroughOSD(BYTE * spm, BYTE * dpm, int w, int xcount, int xfraction, BYTE * po, DDWORD * dd)
	{
	static MMXQUShort negate = {0x7fff, 0x0000, 0x7fff, 0x0000};
	static MMXQUShort lower = {0x00ff, 0x00ff, 0x00ff, 0x00ff};
	static MMXQUShort upper = {0xff00, 0xff00, 0xff00, 0xff00};
	static MMXQUShort fullneg = {0xffff, 0x0000, 0xffff, 0x0000};
	static MMXQUShort mask15 = {0x7fff, 0x7fff, 0x7fff, 0x7fff};

	if (w)
		{
		__asm
			{
			mov			ebx, [spm]
			mov			ecx, [w]
			xor			esi, esi
			movd			mm5, [xcount]
			movd			mm6, [xfraction]

			pxor			mm7, mm7
	loop2:
			movd			edi, mm5
			shr			edi, 16

			movd			mm0, [ebx+2*edi]
			movq			mm1, mm5
			paddd			mm5, mm6

			and			edi, 0xfffe
			movd			mm4, [ebx+2*edi]

			movd			edi, mm5
			shr			edi, 16

			movd			mm2, [ebx+2*edi]
			movq			mm3, mm5
			paddd			mm5, mm6

			punpckldq	mm0, mm2
			pand			mm0, [lower]

			punpckldq	mm1, mm3
			psrld			mm1, 1
			pand			mm1, [negate]
			movq			mm3, mm1
			pslld			mm1, 16
			por			mm1, mm3
			pxor			mm1, [negate]

			pmaddwd		mm0, mm1
			psrld			mm0, 15

			packssdw		mm0, mm0
			packuswb		mm0, mm0
			psrlw			mm4, 8
			packuswb		mm4, mm4
			punpcklbw	mm0, mm4

			punpcklbw	mm0, mm7
			mov			edi, [dd]
			mov			eax, [po]
			movzx			eax, BYTE PTR [eax + esi]
			pmullw		mm0, [edi + eax * 8 + 2048]
			paddw			mm0, [edi + eax * 8]
			psrlw			mm0, 4
			packuswb		mm0, mm0

			mov			eax, [dpm]
			movd			[eax+4*esi], mm0

			inc			esi

			sub			ecx, 2
			jne			loop2
			}
		}
	}

static inline void ScaleYUVLine_P8F2WithOSD(BYTE * spm, BYTE * dpm, int width, int xoffset, int xfraction, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ScaleYUVLine_P8F2(spm, dpm, xosdstart, xoffset, xfraction);
	ScaleYUVLine_P8F2ThroughOSD(spm, dpm + (xosdstart << 1), xosdstop - xosdstart, xoffset + xfraction * xosdstart, xfraction, po, dd);
	ScaleYUVLine_P8F2(spm, dpm + (xosdstop << 1), width - xosdstop, xoffset + xfraction * xosdstop, xfraction);
	}

///////////////////////////////////////////////////////////////////////////////
//
// eight bit scaling routines
//


static inline void ConvertLineToYUV_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w, int s)
	{
	int x;
	int xs;

	xs = 1 << s;

	for(x = 0; x < w; x += 2 * xs)
		{
		*pm++ = py[x];
		*pm++ = puv[x];
		*pm++ = py[x+xs];
		*pm++ = puv[x+xs+1];
		}
	}

static inline void ConvertLineToYUVThroughOSD_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int w,
																	BYTE * po, DDWORD * dd, int s)
	{
	ConvertLineToYUV_P8DS(py, puv, pm, w, s);
	}

static inline void ConvertLineToYUVWithOSD_P8DS(BYTE * py, BYTE * puv, BYTE * pm, int width, int xscale,
																int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8DS(py, puv, pm, xosdstart, xscale);
	ConvertLineToYUVThroughOSD_P8DS(py + xosdstart, puv + xosdstart, pm + (xosdstart >> (xscale - 1)), xosdstop - xosdstart, po, dd, xscale);
	ConvertLineToYUV_P8DS(py + xosdstop, puv + xosdstop, pm + (xosdstop >> (xscale - 1)), width - xosdstop, xscale);
	}







static inline void ConvertLineToYUVWithOSD_P8D1I4(BYTE * py, BYTE * puv, BYTE * ppy, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
//	ConvertLineToYUV_P8D1I4(py, puv, ppy, bpr, pm, width);

	ConvertLineToYUV_P8D1I4(py, puv, ppy, bpr, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1I4(py + xosdstart, puv + xosdstart, ppy + xosdstart, bpr, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1I4(py + xosdstop, puv + xosdstop, ppy + xosdstop, bpr, pm + (xosdstop << 1), width - xosdstop);
	}



static inline void ConvertLineToYUVWithOSD_P8D1I4MUV(BYTE * py, BYTE * puv, BYTE * ppy, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
//	ConvertLineToYUV_P8D1I4MUV(py, puv, ppy, bpr, pm, width);

	ConvertLineToYUV_P8D1I4MUV(py, puv, ppy, bpr, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1I4MUV(py + xosdstart, puv + xosdstart, ppy + xosdstart, bpr, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1I4MUV(py + xosdstop, puv + xosdstop, ppy + xosdstop, bpr, pm + (xosdstop << 1), width - xosdstop);
	}



static inline void ConvertLineToYUVWithOSD_P8D2I4(BYTE * py, BYTE * puv, BYTE * ppy, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2I4(py, puv, ppy, bpr, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2I4(py + xosdstart, puv + xosdstart, ppy + xosdstart, bpr, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2I4(py + xosdstop, puv + xosdstop, ppy + xosdstop, bpr, pm + xosdstop, width - xosdstop);
	}





static inline void ConvertLineToYUVWithOSD_P8D1M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1M(py, puv, ppy, ppuv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1M(py + xosdstart, puv + xosdstart, ppy + xosdstart, ppuv + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1M(py + xosdstop, puv + xosdstop, ppy + xosdstop, ppuv + xosdstop, pm + (xosdstop << 1), width - xosdstop);
	}





static inline void ConvertLineToYUVWithOSD_P8D2M(BYTE * py, BYTE * puv, BYTE * ppy, BYTE * ppuv, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2M(py, puv, ppy, ppuv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2M(py + xosdstart, puv + xosdstart, ppy + xosdstart, ppuv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2M(py + xosdstop, puv + xosdstop, ppy + xosdstop, ppuv + xosdstop, pm + xosdstop, width - xosdstop);
	}





static inline void ConvertLineToYUVWithOSD_P8D1MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1MV(py, puv, bpr, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1MV(py + xosdstart, puv + xosdstart, bpr, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1MV(py + xosdstop, puv + xosdstop, bpr, pm + (xosdstop << 1), width - xosdstop);
	}


// new -> 01/26/2000
static inline void ConvertLineToYUVWithOSD_P8D1MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D1MV(py0, puv0, py1, puv1, py2, puv2, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D1MV(py0 + xosdstart, puv0 + xosdstart, py1 + xosdstart, puv1 + xosdstart, py2 + xosdstart, puv2 + xosdstart, pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D1MV(py0 + xosdstop, puv0 + xosdstop, py1 + xosdstop, puv1 + xosdstop, py2 + xosdstop, puv2 + xosdstop, pm + (xosdstop << 1), width - xosdstop);
	}
// <- new 01/26/2000


static inline void ConvertLineToYUVWithOSD_P8D2MV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2MV(py, puv, bpr, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2MV(py + xosdstart, puv + xosdstart, bpr, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2MV(py + xosdstop, puv + xosdstop, bpr, pm + xosdstop, width - xosdstop);
	}

// new -> 01/26/2000
static inline void ConvertLineToYUVWithOSD_P8D2MV(BYTE * py0, BYTE * puv0, BYTE * py1, BYTE * puv1, BYTE * py2, BYTE * puv2, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2MV(py0, puv0, py1, puv1, py2, puv2 , pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2MV(py0 + xosdstart, puv0 + xosdstart, py1 + xosdstart, puv1 + xosdstart, py2 + xosdstart, puv2 + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2MV(py0 + xosdstop, puv0 + xosdstop, py1 + xosdstop, puv1 + xosdstop, py2 + xosdstop, puv2 + xosdstop, pm + xosdstop, width - xosdstop);
	}
// <- new 01/26/2000

static inline void ConvertLineToYUVWithOSD_P8D2MUV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2(py + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop);
	}

static inline void ConvertLineToYUVWithOSD_P8D2MY(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2(py + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop);
	}

static inline void ConvertLineToYUVWithOSD_P8D2MYUV(BYTE * py, BYTE * puv, int bpr, BYTE * pm, int width, int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	ConvertLineToYUV_P8D2(py, puv, pm, xosdstart);
	ConvertLineToYUVThroughOSD_P8D2(py + xosdstart, puv + xosdstart, pm + xosdstart, xosdstop - xosdstart, po, dd);
	ConvertLineToYUV_P8D2(py + xosdstop, puv + xosdstop, pm + xosdstop, width - xosdstop);
	}

static inline void SharpenYUVLine(BYTE * spyuv, BYTE * dpyuv, int w)
	{
	static __int64 masklow  = 0x00ff00ff00ff00ff;
	static __int64 maskhigh	= 0xff00ff00ff00ff00;

	__asm
		{
		mov			esi, [spyuv]
		mov			edi, [dpyuv]
		mov			ecx, [w]
		shr			ecx, 1
loop1:
		movq			mm0, [esi-2]
		movq			mm1, [esi]
		movq			mm2, [esi+2]
		movq			mm3, mm1
		movq			mm4, mm1

		pand			mm0, [masklow]
		pand			mm1, [masklow]
		pand			mm2, [masklow]
		psrlw			mm3, 8
		packuswb		mm3, mm3
		paddw			mm0, mm2
		paddw			mm1, mm1
		psubw			mm1, mm0



		punpcklbw	mm1, mm3
		movq			[edi], mm1
		add			esi, 8
		add			edi, 8
		sub			ecx, 4
		jne			loop1

		mov			ecx, [w]
		shr			ecx, 1
loop2:
		movq			mm1, [esi]
		movq			[edi], mm1
		add			esi, 8
		add			edi, 8
		sub			ecx, 4
		jne			loop2
		}
	}



// new -> 01/25/2000
static inline void BlackOutLine(BYTE * pm, int width)
	{
	static MMXQUShort fill = {0x8000, 0x8000, 0x8000, 0x8000};

	if (width)
		{
		__asm
			{
			mov		eax, [pm]
			mov		ecx, [width]
			movq		mm0, [fill]
loop1:
			movq		[eax], mm0
			add		eax, 8
			sub		ecx, 4
			jne		loop1
			}
		}
	}
// <- new 01/25/2000


// new -> 01/25/2000
static inline void BlackOutLineThroughOSD(BYTE * pm, int width, BYTE * po, DDWORD * dd)
	{
	static MMXQShort fill = {0x0000, 0x0080, 0x0000, 0x0080};

	if (width)
		{
		__asm
			{
			mov			esi, [pm]
			mov			ecx, [width]
			mov			edi, [po]
			mov			ebx, [dd]
	loop2:
			movzx			eax, BYTE PTR [edi]
			movq			mm0, [fill]
			pmullw		mm0, [ebx + eax * 8 + 2048]
			paddw			mm0, [ebx + eax * 8]
			psrlw			mm0, 4
			movzx			eax, BYTE PTR [edi+1]
			movq			mm1, [fill]
			pmullw		mm1, [ebx + eax * 8 + 2048]
			paddw			mm1, [ebx + eax * 8]
			psrlw			mm1, 4
			packuswb		mm0, mm1
			movq			[esi], mm0
			add			edi, 2
			add			esi, 8
			sub			ecx, 4
			jne			loop2
			}
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
static inline void BlackOutLineWithOSD(BYTE * pm, int width,
													int xosdstart, int xosdstop, BYTE * po, DDWORD * dd)
	{
	BlackOutLine(pm, xosdstart);
	BlackOutLineThroughOSD(pm + (xosdstart << 1), xosdstop - xosdstart, po, dd);
	BlackOutLine(pm + (xosdstop << 1), width - xosdstop);
	}
// <- new 01/25/2000

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		case 1:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = (ycount >> 16) & p.stripeMask;

				lpy = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		case 1:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy = p.py + p.bpr * (hi & p.stripeMask);
				lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8DS(lpy, lpuv, pm, p.width, p.xscale, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUV_P8DS(p);
		}
	else
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = (ycount >> 16) & p.stripeMask;

			lpy  = p.py  + p.bpr * hi;
			lpuv = p.puv + p.bpr * (hi >> 1);

			ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVThroughOSD_P8DS(p);
		}
	else
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py +  p.bpr * (hi & p.stripeMask);
			lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
				}
			else
				{
				ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
				}

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}


void XMMXYUVVideoConverter::ConvertStripeToYUV_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;

	if (p.yfraction == 0x10000 || p.xscale > 1)
		{
		ConvertStripeToYUV_P8DS(p);
		}
	else
		{
		switch (p.xscale)
			{
			case 0:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = (ycount >> 16) & p.stripeMask;
					lo = (ycount & 0xffff) >> 8;

					lpy0 = p.py + p.bpr * hi;
					lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * (hi >> 1);

					if (lo)
						ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, lo);
					else
						ConvertLineToYUV_P8D1(lpy0, lpuv, pm, p.width);

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			case 1:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = (ycount >> 16) & p.stripeMask;
					lo = (ycount & 0xffff) >> 8;

					lpy0 = p.py + p.bpr * hi;
					lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * (hi >> 1);

					if (lo)
						ConvertLineToYUV_P8D2Y2(lpy0, lpy1, lpuv, pm, p.width, lo);
					else
						ConvertLineToYUV_P8D2(lpy0, lpuv, pm, p.width);

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv, * lpo;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.yfraction == 0x10000 || p.xscale > 1)
		ConvertStripeToYUVThroughOSD_P8DS(p);
	else
		{
		switch (p.xscale)
			{
			case 0:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount & 0xffff) >> 8;

					lpy0 = p.py + p.bpr * (hi & p.stripeMask);
					lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);


					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (lo)
							ConvertLineToYUVWithOSD_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, lo);
						else
							ConvertLineToYUVWithOSD_P8D1(lpy0, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						if (lo)
							ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, lo);
						else
							ConvertLineToYUV_P8D1(lpy0, lpuv, pm, p.width);
						}

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			case 1:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount & 0xffff) >> 8;

					lpy0 = p.py + p.bpr * (hi & p.stripeMask);
					lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);


					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (lo)
							ConvertLineToYUVWithOSD_P8D2Y2(lpy0, lpy1, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, lo);
						else
							ConvertLineToYUVWithOSD_P8D2(lpy0, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						if (lo)
							ConvertLineToYUV_P8D2Y2(lpy0, lpy1, lpuv, pm, p.width, lo);
						else
							ConvertLineToYUV_P8D2(lpy0, lpuv, pm, p.width);
						}

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			}
		}
	}


void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUV_P8F2(p);
		}
	else if (p.xscale == 0x10000)
		{
		ConvertStripeToYUV_P8DSY2(p);
		}
	else
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = (ycount >> 16) & p.stripeMask;
			lo = (ycount & 0xffff) >> 8;

			lpy0 = p.py + p.bpr * hi;
			lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

			lpuv = p.puv + p.bpr * (hi >> 1);

			if (lo)
				ConvertLineToYUV_P8F2Y2(lpy0, lpy1, lpuv, pm, p.mwidth, 0, p.xfraction, lo);
			else
				ConvertLineToYUV_P8F2(lpy0, lpuv, pm, p.mwidth, 0, p.xfraction);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv, * lpo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVThroughOSD_P8F2(p);
		}
	else if (p.xscale == 0x10000)
		{
		ConvertStripeToYUVThroughOSD_P8DSY2(p);
		}
	else
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount & 0xffff) >> 8;

			lpy0 = p.py + p.bpr * (hi & p.stripeMask);
			lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

			lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (lo)
					ConvertLineToYUVWithOSD_P8D1Y2(lpy0, lpy1, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, lo);
				else
					ConvertLineToYUVWithOSD_P8D1(lpy0, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);

				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
				}
			else
				{
				if (lo)
					ConvertLineToYUV_P8F2Y2(lpy0, lpy1, lpuv, pm, p.mwidth, 0, p.xfraction, lo);
				else
					ConvertLineToYUV_P8F2(lpy0, lpuv, pm, p.mwidth, 0, p.xfraction);
				}

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8DSY4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3, * lpuv;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;
	MMXQShort * sinctabyt;

	if (p.yfraction == 0x10000 || p.xscale > 1)
		ConvertStripeToYUV_P8DS(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);

		switch (p.xscale)
			{
			case 0:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount >> 9) & 0x7f;

					lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
					lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi > 0)
						lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
					else
						lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi < p.height - 2)
						{
						lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
						}
					else if (hi < p.height - 1)
						{
						lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						}
					else
						{
						lpy2 = lpy3 = lpy1;
						}

					ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, sinctabyt[lo].a);

					ycount += p.yfraction;
					pm  += p.mbpr;
					}
				break;
			case 1:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount >> 9) & 0x7f;

					lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
					lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi > 0)
						lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
					else
						lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi < p.height - 2)
						{
						lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
						}
					else if (hi < p.height - 1)
						{
						lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						}
					else
						{
						lpy2 = lpy3 = lpy1;
						}

					ConvertLineToYUV_P8D2Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, sinctabyt[lo].a);

					ycount += p.yfraction;
					pm  += p.mbpr;
					}
				break;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8DSY4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3, * lpuv, * lpo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;
	MMXQShort * sinctabyt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUV_P8F2(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);

		switch (p.xscale)
			{
			case 0:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount >> 9) & 0x7f;

					lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
					lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi > 0)
						lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
					else
						lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi < p.height - 2)
						{
						lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
						}
					else if (hi < p.height - 1)
						{
						lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						}
					else
						{
						lpy2 = lpy3 = lpy1;
						}

					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						ConvertLineToYUVWithOSD_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, sinctabyt[lo].a);
						}
					else
						ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, sinctabyt[lo].a);

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			case 1:
				for(y = p.ystart; y < p.ystop; y++)
					{
					hi = ycount >> 16;
					lo = (ycount >> 9) & 0x7f;

					lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
					lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi > 0)
						lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
					else
						lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

					if (hi < p.height - 2)
						{
						lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
						}
					else if (hi < p.height - 1)
						{
						lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
						}
					else
						{
						lpy2 = lpy3 = lpy1;
						}

					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						ConvertLineToYUVWithOSD_P8D2Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, sinctabyt[lo].a);
						}
					else
						ConvertLineToYUV_P8D2Y4(lpy0, lpy1, lpy2, lpy3, lpuv, pm, p.width, sinctabyt[lo].a);

					ycount += p.yfraction;
					pm += p.mbpr;
					}
				break;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F2Y4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3, * lpuv;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;
	MMXQShort * sinctabyt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUV_P8F2(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUV_P8DSY4(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount >> 9) & 0x7f;

			lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
			lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi > 0)
				lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
			else
				lpy0 = lpy1;

			if (hi < p.height - 2)
				{
				lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
				}
			else if (hi < p.height - 1)
				{
				lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				}
			else
				{
				lpy2 = lpy3 = lpy1;
				}

			ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, sinctabyt[lo].a);
			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			ycount += p.yfraction;
			pm  += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F2Y4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3, * lpuv, * lpo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int hi, lo;
	MMXQShort * sinctabyt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8F2(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8DSY4(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount >> 9) & 0x7f;

			lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
			lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi > 0)
				lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
			else
				lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi < p.height - 2)
				{
				lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
				}
			else if (hi < p.height - 1)
				{
				lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				}
			else
				{
				lpy2 = lpy3 = lpy1;
				}

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, sinctabyt[lo].a);
				}
			else
				ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, sinctabyt[lo].a);

			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabxt;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUV_P8DS(p);
		}
	else
		{
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = (ycount >> 16) & p.stripeMask;

			lpy = p.py + p.bpr * hi;
			lpuv = p.puv + p.bpr * (hi >> 1);

			ConvertLineToYUV_P8F4(lpy, lpuv, pm, p.mwidth, 0, p.xfraction, sinctabxt);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}


void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabxt;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVThroughOSD_P8DS(p);
		}
	else
		{
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy = p.py + p.bpr * (hi & p.stripeMask);
			lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);

				ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);
				}
			else
				{
				ConvertLineToYUV_P8F4(lpy, lpuv, pm, p.mwidth, 0, p.xfraction, sinctabxt);
				}

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F4Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabxt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUV_P8F4(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUV_P8DSY2(p);
	else
		{
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = (ycount >> 16) & p.stripeMask;
			lo = (ycount & 0xffff) >> 8;

			lpy0 = p.py + p.bpr * hi;
			lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

			lpuv = p.puv + p.bpr * (hi >> 1);

			if (lo)
				{
				ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, yuvcache, p.width, lo);
				ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);
				}
			else
				ConvertLineToYUV_P8F4(lpy0, lpuv, pm, p.mwidth, 0, p.xfraction, sinctabxt);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F4Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv, * lpo;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabxt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8F4(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8DSY2(p);
	else
		{
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount & 0xffff) >> 8;

			lpy0 = p.py + p.bpr * (hi & p.stripeMask);
			lpy1 = p.py + p.bpr * ((hi + 1) & p.stripeMask);

			lpuv = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (lo)
					ConvertLineToYUVWithOSD_P8D1Y2(lpy0, lpy1, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, lo);
				else
					ConvertLineToYUVWithOSD_P8D1(lpy0, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);

				ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);
				}
			else
				{
				if (lo)
					{
					ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, yuvcache, p.width, lo);
					ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);
					}
				else
					ConvertLineToYUV_P8F4(lpy0, lpuv, pm, p.mwidth, 0, p.xfraction, sinctabxt);
				}

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV_P8F4Y4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabyt;
	MMXQShort * sinctabxt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUV_P8F4(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUV_P8DSY4(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount >> 9) & 0x7f;

			lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
			lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi > 0)
				lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
			else
				lpy0 = lpy1;

			if (hi < p.height - 2)
				{
				lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
				}
			else if (hi < p.height - 1)
				{
				lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				}
			else
				{
				lpy2 = lpy3 = lpy1;
				}

			ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, sinctabyt[lo].a);
			ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);

			ycount += p.yfraction;
			pm  += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVThroughOSD_P8F4Y4(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpuv, * lpo, * lpy0, * lpy1, * lpy2, * lpy3;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	MMXQShort * sinctabyt;
	MMXQShort * sinctabxt;

	if (p.yfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8F4(p);
	else if (p.xfraction == 0x10000)
		ConvertStripeToYUVThroughOSD_P8DSY4(p);
	else
		{
		sinctabyt = &(sinctabySet[0x100000 / p.yfraction][0]);
		sinctabxt = &(sinctabxSet[0x100000 / p.xfraction][0]);

		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = (ycount >> 9) & 0x7f;

			lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
			lpy1 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi > 0)
				lpy0 = p.py + ((hi - 1) & p.stripeMask) * p.bpr;
			else
				lpy0 = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi < p.height - 2)
				{
				lpy2 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				lpy3 = p.py + ((hi + 2) & p.stripeMask) * p.bpr;
				}
			else if (hi <p. height - 1)
				{
				lpy2 = lpy3 = p.py + ((hi + 1) & p.stripeMask) * p.bpr;
				}
			else
				{
				lpy2 = lpy3 = lpy1;
				}

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, sinctabyt[lo].a);
				}
			else
				ConvertLineToYUV_P8D1Y4(lpy0, lpy1, lpy2, lpy3, lpuv, yuvcache, p.width, sinctabyt[lo].a);

			ScaleYUVLine_P8F4(yuvcache, pm, p.mwidth, 0, p.xfraction, sinctabxt);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

#define PANSCAN_FRACTION		0xc000

void XMMXYUVVideoConverter::ConvertStripeToYUVPS_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	BYTE * pm = p.pm;
	int ycount = p.ycount;

	if (p.xfraction == 0x10000)
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = (ycount >> 16) & p.stripeMask;

			lpy = p.py + p.bpr * hi;
			lpuv = p.puv + p.bpr * (hi >> 1);

			ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}


void XMMXYUVVideoConverter::ConvertStripeToYUVPSThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	BYTE * pm = p.pm;
	int ycount = p.ycount;

	if (p.xfraction == 0x10000)
		{
		for(y = p.ystart; y < p.ystop; y++)
			{
			hi = ycount >> 16;

			lpuv = p.puv + ((hi & p.stripeMask) >> 1) * p.bpr;
			lpy = p.py + ((hi + 0) & p.stripeMask) * p.bpr;

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUV_P8F2WithOSD(lpy, lpuv, pm, p.width, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);

			ycount += p.yfraction;
			pm += p.mbpr;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVPSAI_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * hi;
			lpuv = p.puv + p.bpr * (hi >> 1);

			if (hi >= 2 && hi < p.height - 1)
				{
				ConvertLineToYUV_P8D1MV(lpy - p.bpr, lpuv - p.bpr, p.bpr, yuvcache, p.width);
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
				}
			else
				ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVPSAIThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * hi;
			lpuv = p.puv + p.bpr * (hi >> 1);

			yo = hi - p.yosdoffset;

			if (hi >= 2 && hi < p.height - 1)
				ConvertLineToYUV_P8D1MV(lpy - p.bpr, lpuv - p.bpr, p.bpr, yuvcache, p.width);
			else
				ConvertLineToYUV_P8D1(lpy, lpuv, yuvcache, p.width);

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ScaleYUVLine_P8F2WithOSD(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				{
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
				}


			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}



// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSDI2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lppy = p.ppy + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
			lpytb = p.pytb + p.bpr * (hi & ~1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

			if (hi < p.height - 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					}
				}
			else
				{
				if ((hi & 1) != 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
						ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				}

			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * (hi | 1);
			lppy  = p.ppy  + p.bpr * (hi | 1);
			lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
			lpytb = p.pytb + p.bpr * (hi | 1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

			if (hi >= 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						break;
					}
				}
			else
				{
				if ((hi & 1) == 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
						ConvertLineToYUV_P8F2(lppy, lppuv, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
						ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				}

			pm += p.mbpr;
			ppm += p.mbpr;
			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lppy = p.ppy + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
			lpytb = p.pytb + p.bpr * (hi & ~1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1(lpy, lpuv, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				}
			else
				{
				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					}
				}

			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * (hi | 1);
			lppy  = p.ppy  + p.bpr * (hi | 1);
			lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
			lpytb = p.pytb + p.bpr * (hi | 1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4 && hi >= 4)
//				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1(lppy, lppuv, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1(lpytb, lpuvtb, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				}
			else
				{
				if (hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							break;
						}
					}
				else
					{
					if ((hi & 1) == 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8F2(lppy, lppuv, ppm, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					}
				}

			pm += p.mbpr;
			ppm += p.mbpr;
			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSA2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * hi;
			lppy  = p.ppy  + p.bpr * hi;
			lpuv  = p.puv  + p.bpr * (hi >> 1);
			lppuv = p.ppuv + p.bpr * (hi >> 1);

			ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, yuvcache, p.width);
			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * hi;
			lppy  = p.ppy  + p.bpr * hi;
			lpuv  = p.puv  + p.bpr * (hi >> 1);
			lppuv = p.ppuv + p.bpr * (hi >> 1);

			yo = hi - p.yosdoffset;

			ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, yuvcache, p.width);

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ScaleYUVLine_P8F2WithOSD(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, p.xoffset, PANSCAN_FRACTION);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSSI2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

			if (hi < p.height - 4 && hi >= 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MY(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MYUV(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						break;
					}
				}
			else
				{
				if ((hi & 1) != 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
						ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
						ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
					}
				}

			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVPSSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if	(p.xfraction == 0x10000)
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1(lpy, lpuv, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, yuvcache, p.bpr, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.bpr, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1(lpy, lpuv, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1(lpy, lpuv, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2WithOSD(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				}
			else
				{
				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, p.xoffset, PANSCAN_FRACTION);
						}
					}
				}

			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <-new 01/25/2000



void XMMXYUVVideoConverter::ConvertStripeToYUVLB_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpuv;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = (lycount >> 16) & p.stripeMask;
				llo = (lycount & 0xffff) >> 8;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy0 = p.py + p.bpr * lhi;
					lpy1 = p.py + p.bpr * ((lhi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * (lhi >> 1);

					if (llo)
						ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, llo);
					else
						ConvertLineToYUV_P8D1(lpy0, lpuv, pm, p.width);
					}
				else
					{
					BlackOutLine(pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVLBThroughOSD_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpuv, * lpo, * lpy0, * lpy1;
	int hi, lo;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = (lycount >> 16) & p.stripeMask;
				llo = (lycount & 0xffff) >> 8;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy0 = p.py + p.bpr * (lhi & p.stripeMask);
					lpy1 = p.py + p.bpr * ((lhi + 1) & p.stripeMask);

					lpuv = p.puv + p.bpr * ((lhi & p.stripeMask) >> 1);


					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (llo)
							ConvertLineToYUVWithOSD_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, llo);
						else
							ConvertLineToYUVWithOSD_P8D1(lpy0, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						if (llo)
							ConvertLineToYUV_P8D1Y2(lpy0, lpy1, lpuv, pm, p.width, llo);
						else
							ConvertLineToYUV_P8D1(lpy0, lpuv, pm, p.width);
						}
					}
				else
					{
					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						BlackOutLine(pm, p.xosdstart);
						BlackOutLineThroughOSD(pm + (p.xosdstart << 1), p.xosdstop - p.xosdstart, lpo, p.dd);
						BlackOutLine(pm + (p.xosdstop << 1), p.width - p.xosdstop);
						}
					else
						{
						BlackOutLine(pm, p.width);
						}

					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVLBAI_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = lycount >> 16;
				llo = lycount & 0xffff;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy  = p.py  + p.bpr * lhi;
					lpuv = p.puv + p.bpr * (lhi >> 1);

					if (lhi >= 2 && lhi < p.height - 2)
						{
						filter[0] = (0x10000 - llo) >> 12;
						filter[1] = (0x10000 + llo) >> 12;
						filter[2] = (0x20000 - llo) >> 12;
						filter[3] = (0x00000 + llo) >> 12;

						ConvertLineToYUV_P8D1Y4MUV(lpy - p.bpr, lpy + p.bpr, lpy, lpy + 2 * p.bpr, lpuv - p.bpr, lpuv, pm, p.width, filter);
						}
					else
						ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}
				else
					{
					BlackOutLine(pm, p.width);
					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVLBAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpuv, * lpo, * lpy;
	int hi, lo;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	switch (p.xscale)
		{
		case 0:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = lycount >> 16;
				llo = lycount & 0xffff;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy  = p.py  + p.bpr *  lhi;
					lpuv = p.puv + p.bpr * (lhi  >> 1);

					filter[0] = (0x10000 - llo) >> 12;
					filter[1] = (0x10000 + llo) >> 12;
					filter[2] = (0x20000 - llo) >> 12;
					filter[3] = (0x00000 + llo) >> 12;

					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (lhi >= 2 && lhi < p.height - 2)
							ConvertLineToYUVWithOSD_P8D1Y4MUV(lpy - p.bpr, lpy + p.bpr, lpy, lpy + 2 * p.bpr, lpuv - p.bpr, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, filter);
						else
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						if (lhi >= 2 && lhi < p.height - 2)
							ConvertLineToYUV_P8D1Y4MUV(lpy - p.bpr, lpy + p.bpr, lpy, lpy + 2 * p.bpr, lpuv - p.bpr, lpuv, pm, p.width, filter);
						else
							ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
						}
					}
				else
					{
					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						BlackOutLine(pm, p.xosdstart);
						BlackOutLineThroughOSD(pm + (p.xosdstart << 1), p.xosdstop - p.xosdstart, lpo, p.dd);
						BlackOutLine(pm + (p.xosdstop << 1), p.width - p.xosdstop);
						}
					else
						{
						BlackOutLine(pm, p.width);
						}

					}

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}


// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBDI2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = lycount >> 16;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy  = p.py  + p.bpr * (lhi & ~1);
					lppy = p.ppy + p.bpr * (lhi & ~1);
					lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);
					lpytb = p.pytb + p.bpr * (lhi & ~1);
					lpuvtb = p.puvtb + p.bpr * ((lhi >> 1) & ~1);

					if (lhi < p.height - 4)
						{
						switch (lhi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
							}
						}

					lpy   = p.py   + p.bpr * (lhi | 1);
					lppy  = p.ppy  + p.bpr * (lhi | 1);
					lppuv = p.ppuv + p.bpr * ((lhi >> 1) | 1);
					lpytb = p.pytb + p.bpr * (lhi | 1);
					lpuvtb = p.puvtb + p.bpr * ((lhi >> 1) | 1);

					if (lhi >= 4)
						{
						switch (lhi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
								break;
							}
						}
					else
						{
						if ((lhi & 1) == 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1(lppy, lppuv, ppm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
							}
						}
					}
				else
					{
					if ((lhi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							BlackOutLine(pm, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							BlackOutLine(pmb, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							BlackOutLine(pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							BlackOutLine(ppm, p.width);
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;

				lhi = lycount >> 16;

				lpy  = p.py  + p.bpr * (lhi & ~1);
				lppy = p.ppy + p.bpr * (lhi & ~1);
				lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);
				lpytb = p.pytb + p.bpr * (lhi & ~1);
				lpuvtb = p.puvtb + p.bpr * ((lhi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (lhi >= 0 && lhi < p.height - 1)
						{
						if (lhi < p.height - 4)
							{
							switch (lhi & 3)
								{
								case 0:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
										ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 1:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
										ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 2:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
										ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 3:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
										ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								}
							}
						else
							{
							if ((lhi & 1) != 0)
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							else
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								BlackOutLineWithOSD(pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								BlackOutLineWithOSD(pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (lhi >= 0 && lhi < p.height - 1)
						{
						if (lhi < p.height - 4)
							{
							switch (lhi & 3)
								{
								case 0:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
										ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
									break;
								case 1:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
										ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
									break;
								case 2:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
										ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
									break;
								case 3:
									if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
										ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
									break;
								}
							}
						else
							{
							if ((lhi & 1) != 0)
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
								}
							else
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
								}
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								BlackOutLine(pm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								BlackOutLine(pmt, p.width);
							}
						}
					}

				lpy   = p.py   + p.bpr * (lhi | 1);
				lppy  = p.ppy  + p.bpr * (lhi | 1);
				lppuv = p.ppuv + p.bpr * ((lhi >> 1) | 1);
				lpytb = p.pytb + p.bpr * (lhi | 1);
				lpuvtb = p.puvtb + p.bpr * ((lhi >> 1) | 1);

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (lhi >= 0 && lhi < p.height - 1)
						{
						if (lhi < p.height - 4 && lhi >= 4)
//						if (lhi < p.height - 4)
							{
							switch (lhi & 3)
								{
								case 0:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
										ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 1:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
										ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 2:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
										ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								case 3:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
										ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
									break;
								}
							}
						else
							{
							if ((lhi & 1) != 0)
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1(lppy, lppuv, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							else
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								}
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								BlackOutLineWithOSD(ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								BlackOutLineWithOSD(pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (lhi >= 0 && lhi < p.height - 1)
						{
						if (lhi >= 4)
							{
							switch (lhi & 3)
								{
								case 0:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
										ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
									break;
								case 1:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
										ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
									break;
								case 2:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
										ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
									break;
								case 3:
									if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
										ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
									break;
								}
							}
						else
							{
							if ((lhi & 1) == 0)
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1(lppy, lppuv, ppm, p.width);
								}
							else
								{
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
								}
							}
						}
					else
						{
						if ((lhi & 1) == 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								BlackOutLine(ppm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								BlackOutLine(pmb, p.width);
							}
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBA2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				llo = lycount & 0xffff;

				if (lhi >= 0 && lhi < p.height - 1)
					{
					lpy   = p.py   + p.bpr * lhi;
					lppy  = p.ppy  + p.bpr * lhi;
					lpuv  = p.puv  + p.bpr * (lhi >> 1);
					lppuv = p.ppuv + p.bpr * (lhi >> 1);

					filter[0] = (0x10000 >> 11) - (llo >> 11);
					filter[1] = (llo) >> 11;
					filter[2] = (0x10000 >> 11) - (llo >> 11);
					filter[3] = (llo) >> 11;

					ConvertLineToYUV_P8D1Y4MUV(lpy, lpy + p.bpr, lppy, lppy + p.bpr, lpuv, lppuv, pm, p.width, filter);
					}
				else
					BlackOutLine(pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	int lhi, llo;
	int sstart = p.lboffset << 16;
	int lycount;
	BYTE * pm = p.pm;
	short filter[4];

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				llo = lycount & 0xffff;
				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * lhi;
				lppy  = p.ppy  + p.bpr * lhi;
				lpuv  = p.puv  + p.bpr * (lhi >> 1);
				lppuv = p.ppuv + p.bpr * (lhi >> 1);

				filter[0] = (0x10000 >> 11) - (llo >> 11);
				filter[1] = (llo) >> 11;
				filter[2] = (0x10000 >> 11) - (llo >> 11);
				filter[3] = (llo) >> 11;

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (lhi >= 0 && lhi < p.height - 1)
						{
						ConvertLineToYUVWithOSD_P8D1Y4MUV(lpy, lpy + p.bpr, lppy, lppy + p.bpr, lpuv, lppuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, filter);
//						ConvertLineToYUVWithOSD_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						BlackOutLineWithOSD(pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}

					}
				else
					{
					if (lhi >= 0 && lhi < p.height - 1)
						{
						ConvertLineToYUV_P8D1Y4MUV(lpy, lpy + p.bpr, lppy, lppy + p.bpr, lpuv, lppuv, pm, p.width, filter);
//						ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);
						}
					else
						{
						BlackOutLine(pm, p.width);
						}
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBSI2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;

				lpy  = p.py  + p.bpr * (lhi & ~1);
				lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);

				if (lhi < p.height - 4 && lhi >= 4)
					{
					switch (lhi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1MY(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1MYUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							break;
						}
					}
				else if (lhi >= 0 && lhi < p.height)
					{
					if ((lhi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					}
				else
					{
					if ((lhi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							BlackOutLine(pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							BlackOutLine(pmb, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							BlackOutLine(pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							BlackOutLine(pmb, p.width);
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLBSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi, yo;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (lhi & ~1);
				lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (lhi < p.height - 4 && lhi >= 4)
						{
						switch (lhi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MY(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MY(lpy - p.bpr, lpuv + p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MYUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else if (lhi >= 0 && lhi < p.height)
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								BlackOutLineWithOSD(pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								BlackOutLineWithOSD(pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								BlackOutLineWithOSD(pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								BlackOutLineWithOSD(pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (lhi < p.height - 4 && lhi >= 4)
						{
						switch (lhi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1MY(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1MUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1MYUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
								break;
							}
						}
					else if (lhi >= 0 && lhi < p.height)
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						}
					else
						{
						if ((lhi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								BlackOutLine(pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								BlackOutLine(pmb, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								BlackOutLine(pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								BlackOutLine(pmb, p.width);
							}
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLB32PDFieldField_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				hi = ycount >> 16;

				if (lhi >= 0 && lhi < p.height)
					{
					if ((lhi & 1) == 0)
						{
						lpy  = p.py + p.bpr * lhi;
						lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);
						}
					else
						{
						lpy  = p.ppy + p.bpr * lhi;
						lpuv = p.ppuv + p.bpr * ((lhi >> 1) | 1);
						}

					ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}
				else
					{
					BlackOutLine(pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLB32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				hi = ycount >> 16;

				if (lhi >= 0 && lhi < p.height)
					{
					if ((lhi & 1) == 0)
						{
						lpy  = p.py + p.bpr * lhi;
						lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);
						}
					else
						{
						lpy  = p.ppy + p.bpr * lhi;
						lpuv = p.ppuv + p.bpr * ((lhi >> 1) | 1);
						}

					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
						}
					}
				else
					{
					if (hi >= p.yosdstart && hi < p.yosdstop)
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						BlackOutLineWithOSD(pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
						}
					else
						{
						BlackOutLine(pm, p.width);
						}
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}
// <- new 01/25/2000

// new -> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLB32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	ConvertStripeToYUVLBThroughOSD_P8DSY2(p);
	}
// <- new 01/25/2000

// new-> 01/25/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVLB32PDFrameField_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int hi;
	int lhi;
	int sstart = p.lboffset << 16;
	int lycount;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xscale == 0)
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				hi = ycount >> 16;

				if (lhi >= 0 && lhi < p.height)
					{
					if ((lhi & 1) == 1)
						{
						lpy  = p.py + p.bpr * lhi;
						lpuv = p.puv + p.bpr * ((lhi >> 1) | 1);

						ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
						}
					else
						{
						lpy   = p.py   + p.bpr * lhi;
						lpuv  = p.puv  + p.bpr * ((lhi >> 1) & ~1);
						lppy  = p.ppy  + p.bpr * lhi;
						lppuv = p.ppuv + p.bpr * ((lhi >> 1) & ~1);

						ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);
						}
					}
				else
					{
					BlackOutLine(pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				lycount = (((ycount - sstart) >> 8) * 5461) >> 4;
				lhi = lycount >> 16;
				hi = ycount >> 16;

				if (lhi >= 0 && lhi < p.height)
					{
					if ((lhi & 1) == 0)
						{
						lpy  = p.py + p.bpr * lhi;
						lpuv = p.puv + p.bpr * ((lhi >> 1) & ~1);

						ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
						}
					else
						{
						lpy   = p.py   + p.bpr * lhi;
						lpuv  = p.puv  + p.bpr * ((lhi >> 1) | 1);
						lppy  = p.ppy  + p.bpr * lhi;
						lppuv = p.ppuv + p.bpr * ((lhi >> 1) | 1);

						ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);
						}
					}
				else
					{
					BlackOutLine(pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		}
	}
// <-new 01/25/2000

void XMMXYUVVideoConverter::ConvertStripeToYUVDI_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int dbpr;

	switch (p.xscale)
		{
		case 0:
			if (p.topField)
				{
				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					if (hi >= p.height - 4) hi = 0;
					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
							break;
						case 1:
							ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						case 2:
							ConvertLineToYUV_P8D1MUV(lpy, lpuv, pm, p.width, p.bpr);
							break;
						case 3:
							ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			else
				{
				dbpr = p.bpr * 2;

				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					if (hi < 4) hi = 3;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
							break;
						case 1:
							ConvertLineToYUV_P8D1MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
							break;
						case 2:
							ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
							break;
						case 3:
							ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
							break;
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			break;
		case 1:
			if (p.topField)
				{
				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					if (hi >= p.height - 4) hi = 0;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
							break;
						case 1:
							ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						case 2:
							ConvertLineToYUV_P8D2MUV(lpy, lpuv, pm, p.width, p.bpr);
							break;
						case 3:
							ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			else
				{
				dbpr = p.bpr * 2;

				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					if (hi < 4) hi = 3;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8D2I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
							break;
						case 1:
							ConvertLineToYUV_P8D2MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
							break;
						case 2:
							ConvertLineToYUV_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
							break;
						case 3:
							ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
							break;
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDIThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lpo;
	int hi, yo, dbpr;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			if (p.topField)
				{
				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					yo = hi - p.yosdoffset;

					if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (hi >= p.height - 4) hi = 0;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								ConvertLineToYUVWithOSD_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if (hi >= p.height - 4) hi = 0;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
								break;
							case 1:
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							case 2:
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, pm, p.width, p.bpr);
								break;
							case 3:
								ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							}
						}
					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			else
				{
				dbpr = p.bpr * 2;

				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					yo = hi - p.yosdoffset;

					if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (hi < 4) hi = 3;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUVWithOSD_P8D1I4MUV(lpy - dbpr, lppy - dbpr, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								ConvertLineToYUVWithOSD_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if (hi < 4) hi = 3;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
								break;
							case 1:
								ConvertLineToYUV_P8D1MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
								break;
							case 2:
								ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
								break;
							case 3:
								ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
								break;
							}
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			break;
		case 1:
			if (p.topField)
				{
				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					yo = hi - p.yosdoffset;

					if (hi >= p.yosdstart && hi < p.yosdstop && ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0))
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (hi >= p.height - 4) hi = 0;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if (hi >= p.height - 4) hi = 0;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
								break;
							case 1:
								ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							case 2:
								ConvertLineToYUV_P8D2MUV(lpy, lpuv, pm, p.width, p.bpr);
								break;
							case 3:
								ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							}
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			else
				{
				dbpr = p.bpr * 2;

				for(y=p.ystart; y<p.ystop; y++)
					{
					hi = ycount >> 16;

					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					yo = hi - p.yosdoffset;

					if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
						{
						lpo = p.po + p.obpr * (hi - p.yosdstart);

						if (hi < 4) hi = 3;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUVWithOSD_P8D2I4(lpy - dbpr, lppy - dbpr, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv - dbpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								ConvertLineToYUVWithOSD_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if (hi < 4) hi = 3;

						switch (hi & 3)
							{
							case 0:
								ConvertLineToYUV_P8D2I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
								break;
							case 1:
								ConvertLineToYUV_P8D2MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
								break;
							case 2:
								ConvertLineToYUV_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
								break;
							case 3:
								ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
								break;
							}
						}

					pm += p.mbpr;
					ycount += p.yfraction;
					}
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDI_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int dbpr;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVDI_P8DS(p);
		}
	else
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				if (hi >= p.height - 4) hi = 0;

				switch (hi & 3)
					{
					case 0:
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
						break;
					case 1:
						ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					case 2:
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
//						ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
//						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					case 3:
						ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					}


				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			dbpr = p.bpr * 2;

			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi | 1);
				lppy = p.ppy + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

				if (hi < 4) hi = 3;

				switch (hi & 3)
					{
					case 0:
						ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, yuvcache, p.width);
						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					case 1:
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
//						ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
//						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					case 2:
						ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, yuvcache, p.width);
						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
						break;
					case 3:
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
						break;
					}


				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDIThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int dbpr;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVDIThroughOSD_P8DS(p);
		}
	else
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi >= p.height - 4) hi = 0;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 1:
							ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 2:
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 3:
							ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						}

					ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
					}
				else
					{
					if (hi >= p.height - 4) hi = 0;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
							break;
						case 1:
							ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						case 2:
							ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						case 3:
							ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						}
					}


				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			dbpr = p.bpr * 2;

			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi | 1);
				lppy = p.ppy + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < 4) hi = 3;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUVWithOSD_P8D1I4(lpy - dbpr, lppy - dbpr, lpuv - dbpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 1:
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv - dbpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 2:
							ConvertLineToYUVWithOSD_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						case 3:
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							break;
						}

					ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
					}
				else
					{
					if (hi < 4) hi = 3;

					switch (hi & 3)
						{
						case 0:
							ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						case 1:
							ConvertLineToYUV_P8F2(lpy, lpuv - dbpr, pm, p.mwidth, 0, p.xfraction);
	//						ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
	//						ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						case 2:
							ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							break;
						case 3:
							ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
							break;
						}
					}


				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}

		}
	}

static void ConvertStripeToYUVDI_P8D1TopFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	if (hi >= p.height - 4) hi = 0;

	switch (hi & 3)
		{
		case 0:
			ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
			break;
		case 1:
			ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
			break;
		case 2:
			ConvertLineToYUV_P8D1MUV(lpy, lpuv, pm, p.width, p.bpr);
			break;
		case 3:
			ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
			break;
		}
	}

static void ConvertStripeToYUVDI_P8D1BottomFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int dbpr = p.bpr * 2;

	if (hi < 4) hi = 3;

	switch (hi & 3)
		{
		case 0:
			ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
			break;
		case 1:
			ConvertLineToYUV_P8D1MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
			break;
		case 2:
			ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
			break;
		case 3:
			ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
			break;
		}
	}

static void ConvertStripeToYUVDIThroughOSD_P8D1TopFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int yo = hi - p.yosdoffset;
	BYTE * lpo;

	if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
		{
		lpo = p.po + p.obpr * (hi - p.yosdstart);

		if (hi >= p.height - 4) hi = 0;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 1:
				ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 2:
				ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 3:
				ConvertLineToYUVWithOSD_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			}
		}
	else
		{
		if (hi >= p.height - 4) hi = 0;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
				break;
			case 1:
				ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
				break;
			case 2:
				ConvertLineToYUV_P8D1MUV(lpy, lpuv, pm, p.width, p.bpr);
				break;
			case 3:
				ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
				break;
			}
		}
	}

static void ConvertStripeToYUVDIThroughOSD_P8D1BottomFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int dbpr = p.bpr * 2;
	int yo = hi - p.yosdoffset;
	BYTE * lpo;

	if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
		{
		lpo = p.po + p.obpr * (hi - p.yosdstart);

		if (hi < 4) hi = 3;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUVWithOSD_P8D1I4MUV(lpy - dbpr, lppy - dbpr, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 1:
				ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 2:
				ConvertLineToYUVWithOSD_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 3:
				ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			}
		}
	else
		{
		if (hi < 4) hi = 3;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUV_P8D1I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
				break;
			case 1:
				ConvertLineToYUV_P8D1MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
				break;
			case 2:
				ConvertLineToYUV_P8D1I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
				break;
			case 3:
				ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
				break;
			}
		}
	}

static void ConvertStripeToYUVDI_P8D2TopFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	if (hi >= p.height - 4) hi = 0;

	switch (hi & 3)
		{
		case 0:
			ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
			break;
		case 1:
			ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
			break;
		case 2:
			ConvertLineToYUV_P8D2MUV(lpy, lpuv, pm, p.width, p.bpr);
			break;
		case 3:
			ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
			break;
		}
	}

static void ConvertStripeToYUVDI_P8D2BottomFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int dbpr = p.bpr * 2;

	if (hi < 4) hi = 3;

	switch (hi & 3)
		{
		case 0:
			ConvertLineToYUV_P8D2I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
			break;
		case 1:
			ConvertLineToYUV_P8D2MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
			break;
		case 2:
			ConvertLineToYUV_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
			break;
		case 3:
			ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
			break;
		}
	}

static void ConvertStripeToYUVDIThroughOSD_P8D2TopFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int yo = hi - p.yosdoffset;
	BYTE * lpo;

	if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
		{
		lpo = p.po + p.obpr * (hi - p.yosdstart);

		if (hi >= p.height - 4) hi = 0;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 1:
				ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 2:
				ConvertLineToYUVWithOSD_P8D2MUV(lpy, lpuv, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 3:
				ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			}
		}
	else
		{
		if (hi >= p.height - 4) hi = 0;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
				break;
			case 1:
				ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
				break;
			case 2:
				ConvertLineToYUV_P8D2MUV(lpy, lpuv, pm, p.width, p.bpr);
				break;
			case 3:
				ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
				break;
			}
		}
	}

static void ConvertStripeToYUVDIThroughOSD_P8D2BottomFieldLine(const YUVVideoConverterParameter & p, BYTE * lpy, BYTE * lppy, BYTE * lpuv, int hi, BYTE * pm)
	{
	int dbpr = p.bpr * 2;
	int yo = hi - p.yosdoffset;
	BYTE * lpo;

	if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
		{
		lpo = p.po + p.obpr * (hi - p.yosdstart);

		if (hi < 4) hi = 3;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUVWithOSD_P8D2I4(lpy - dbpr, lppy - dbpr, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 1:
				ConvertLineToYUVWithOSD_P8D2MUV(lpy, lpuv - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 2:
				ConvertLineToYUVWithOSD_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			case 3:
				ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				break;
			}
		}
	else
		{
		if (hi < 4) hi = 3;

		switch (hi & 3)
			{
			case 0:
				ConvertLineToYUV_P8D2I4MUV(lpy - dbpr, lpuv - dbpr, lppy - dbpr, p.bpr, pm, p.width);
				break;
			case 1:
				ConvertLineToYUV_P8D2MUV(lpy, lpuv - dbpr, pm, p.width, p.bpr);
				break;
			case 2:
				ConvertLineToYUV_P8D2I4(lpy - dbpr, lpuv, lppy - dbpr, p.bpr, pm, p.width);
				break;
			case 3:
				ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
				break;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDI_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int predpos, predhi;

	predhi = -1;
	predpos = 0;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVDI_P8DS(p);
		}
	else
		{
		switch (p.xscale)
			{
			case 0:
				if (p.topField)
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi & ~1);
							lppy = p.ppy + p.bpr * (hi & ~1);
							lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

							ConvertStripeToYUVDI_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi & ~1);
						lppy = p.ppy + p.bpr * (hi & ~1);
						lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

						ConvertStripeToYUVDI_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				else
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi | 1);
							lppy = p.ppy + p.bpr * (hi | 1);
							lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

							ConvertStripeToYUVDI_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi | 1);
						lppy = p.ppy + p.bpr * (hi | 1);
						lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

						ConvertStripeToYUVDI_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				break;
			case 1:
				if (p.topField)
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi & ~1);
							lppy = p.ppy + p.bpr * (hi & ~1);
							lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

							ConvertStripeToYUVDI_P8D2TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi & ~1);
						lppy = p.ppy + p.bpr * (hi & ~1);
						lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

						ConvertStripeToYUVDI_P8D2TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width >> 1, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				else
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi | 1);
							lppy = p.ppy + p.bpr * (hi | 1);
							lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

							ConvertStripeToYUVDI_P8D2BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi | 1);
						lppy = p.ppy + p.bpr * (hi | 1);
						lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

						ConvertStripeToYUVDI_P8D2BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width >> 1, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				break;
			default:
				ConvertStripeToYUVDI_P8DS(p);
				break;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int predpos, predhi;

	predhi = -1;
	predpos = 0;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVDIThroughOSD_P8DS(p);
		}
	else
		{
		switch (p.xscale)
			{
			case 0:
				if (p.topField)
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi & ~1);
							lppy = p.ppy + p.bpr * (hi & ~1);
							lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

							ConvertStripeToYUVDIThroughOSD_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi & ~1);
						lppy = p.ppy + p.bpr * (hi & ~1);
						lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

						ConvertStripeToYUVDIThroughOSD_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				else
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi | 1);
							lppy = p.ppy + p.bpr * (hi | 1);
							lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

							ConvertStripeToYUVDIThroughOSD_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi | 1);
						lppy = p.ppy + p.bpr * (hi | 1);
						lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

						ConvertStripeToYUVDIThroughOSD_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				break;
			case 1:
				if (p.topField)
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi & ~1);
							lppy = p.ppy + p.bpr * (hi & ~1);
							lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

							ConvertStripeToYUVDIThroughOSD_P8D2TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi & ~1);
						lppy = p.ppy + p.bpr * (hi & ~1);
						lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

						ConvertStripeToYUVDIThroughOSD_P8D2TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				else
					{
					for(y=p.ystart; y<p.ystop; y++)
						{
						hi = ycount >> 16;
						lo = (ycount & 0xffff) >> 8;

						if (hi != predhi)
							{
							lpy  = p.py  + p.bpr * (hi | 1);
							lppy = p.ppy + p.bpr * (hi | 1);
							lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

							ConvertStripeToYUVDIThroughOSD_P8D2BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
							}

						predpos = 1440 - predpos;

						hi += 1;

						lpy  = p.py  + p.bpr * (hi | 1);
						lppy = p.ppy + p.bpr * (hi | 1);
						lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

						ConvertStripeToYUVDIThroughOSD_P8D2BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

						predhi = hi;

						ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, pm, p.width, lo);

						pm += p.mbpr;
						ycount += p.yfraction;
						}
					}
				break;
			default:
				ConvertStripeToYUVDIThroughOSD_P8DS(p);
				break;
			}
		}
	}


void XMMXYUVVideoConverter::ConvertStripeToYUVDI_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int predpos, predhi;

	predhi = -1;
	predpos = 0;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVDI_P8F2(p);
		}
	else if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVDI_P8DS(p);
		}
	else
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				if (hi != predhi)
					{
					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					ConvertStripeToYUVDI_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
					}

				predpos = 1440 - predpos;

				hi += 1;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				ConvertStripeToYUVDI_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

				predhi = hi;

				ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, yuvcache + 1440 - predpos, p.width, lo);
				ScaleYUVLine_P8F2(yuvcache + 1440 - predpos, pm, p.mwidth, 0, p.xfraction);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				if (hi != predhi)
					{
					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					ConvertStripeToYUVDI_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
					}

				predpos = 1440 - predpos;

				hi += 1;

				lpy  = p.py  + p.bpr * (hi | 1);
				lppy = p.ppy + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

				ConvertStripeToYUVDI_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

				predhi = hi;

				ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, yuvcache + 1440 - predpos, p.width, lo);
				ScaleYUVLine_P8F2(yuvcache + 1440 - predpos, pm, p.mwidth, 0, p.xfraction);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	int predpos, predhi;

	predhi = -1;
	predpos = 0;

	if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVDIThroughOSD_P8F2(p);
		}
	else if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVDIThroughOSD_P8DS(p);
		}
	else
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				if (hi != predhi)
					{
					lpy  = p.py  + p.bpr * (hi & ~1);
					lppy = p.ppy + p.bpr * (hi & ~1);
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					ConvertStripeToYUVDIThroughOSD_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
					}

				predpos = 1440 - predpos;

				hi += 1;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				ConvertStripeToYUVDIThroughOSD_P8D1TopFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

				predhi = hi;

				ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, yuvcache + 1440 - predpos, p.width, lo);
				ScaleYUVLine_P8F2(yuvcache + 1440 - predpos, pm, p.mwidth, 0, p.xfraction);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;
				lo = (ycount & 0xffff) >> 8;

				if (hi != predhi)
					{
					lpy  = p.py  + p.bpr * (hi | 1);
					lppy = p.ppy + p.bpr * (hi | 1);
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					ConvertStripeToYUVDIThroughOSD_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);
					}

				predpos = 1440 - predpos;

				hi += 1;

				lpy  = p.py  + p.bpr * (hi | 1);
				lppy = p.ppy + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

				ConvertStripeToYUVDIThroughOSD_P8D1BottomFieldLine(p, lpy, lppy, lpuv, hi, yuvcache + predpos);

				predhi = hi;

				ScaleYUVLines_P8Y2(yuvcache + 1440 - predpos, yuvcache + predpos, yuvcache + 1440 - predpos, p.width, lo);
				ScaleYUVLine_P8F2(yuvcache + 1440 - predpos, pm, p.mwidth, 0, p.xfraction);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		}
	}

///////////////////////////////

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFieldField_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 0)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
					}
				else
					{
					lpy  = p.ppy + p.bpr * hi;
					lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
					}

				ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 0)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
					}
				else
					{
					lpy  = p.ppy + p.bpr * hi;
					lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
					}

				ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFieldFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 0)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
					}
				else
					{
					lpy  = p.ppy + p.bpr * hi;
					lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
					}

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 0)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
					}
				else
					{
					lpy  = p.ppy + p.bpr * hi;
					lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
					}

				if (hi >= p.yosdstart && hi < p.yosdstop)
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFrameFieldThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	ConvertStripeToYUVThroughOSD_P8DS(p);
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFrameField_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xscale == 0)
		{
		if (p.topField)
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 1)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) | 1);

					ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}
				else
					{
					lpy   = p.py   + p.bpr * hi;
					lpuv  = p.puv  + p.bpr * ((hi >> 1) & ~1);
					lppy  = p.ppy  + p.bpr * hi;
					lppuv = p.ppuv + p.bpr * ((hi >> 1) & ~1);

					ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		else
			{
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				if ((y & 1) == 0)
					{
					lpy  = p.py + p.bpr * hi;
					lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

					ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
					}
				else
					{
					lpy   = p.py   + p.bpr * hi;
					lpuv  = p.puv  + p.bpr * ((hi >> 1) | 1);
					lppy  = p.ppy  + p.bpr * hi;
					lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);

					ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);
					}


				pm += p.mbpr;
				ycount += p.yfraction;
				}
			}
		}
	else
		ConvertStripeToYUV_P8DS(p);
	}

///////////////////////////////

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFieldField_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUV32PDFieldField_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			if ((y & 1) == 0)
				{
				lpy  = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				}
			else
				{
				lpy  = p.ppy + p.bpr * hi;
				lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				}

			ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFieldFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUV32PDFieldFieldThroughOSD_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			if ((y & 1) == 0)
				{
				lpy  = p.py + p.bpr * hi;
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				}
			else
				{
				lpy  = p.ppy + p.bpr * hi;
				lpuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				}

			if (hi >= p.yosdstart && hi < p.yosdstop)
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);

				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
				}
			else
				{
				ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
				}

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFrameFieldThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	ConvertStripeToYUVThroughOSD_P8F2(p);
	}

void XMMXYUVVideoConverter::ConvertStripeToYUV32PDFrameField_P8F2(const YUVVideoConverterParameter & p)
	{
	if (p.xfraction == 0x10000)
		ConvertStripeToYUV32PDFrameField_P8DS(p);
	else
		ConvertStripeToYUV_P8F2(p);
	}

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAI_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2;
	BYTE * lpuv0, * lpuv1, * lpuv2;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
				lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
				lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
				lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
				lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
				lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

				if (hi >= 2 && hi < p.height - 2)
//				if (hi >= 2 && hi < p.height - 1)
					ConvertLineToYUV_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width);
				else
					ConvertLineToYUV_P8D1(lpy1, lpuv1, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
				lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
				lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
				lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
				lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
				lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

				if (hi >= 2 && hi < p.height - 2)
//				if (hi >= 2 && hi < p.height - 1)
					ConvertLineToYUV_P8D2MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width);
				else
					ConvertLineToYUV_P8D2(lpy1, lpuv1, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * ((hi & p.stripeMask) & ~1);
				lpuv0 = p.puv + p.bpr * (((hi & p.stripeMask) >> 1) & ~1);

				ConvertLineToYUV_P8DS(lpy0, lpuv0, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAIThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2;
	BYTE * lpuv0, * lpuv1, * lpuv2;
	BYTE * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
				lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
				lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
				lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
				lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
				lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi >= 2 && hi < p.height - 2)
//					if (hi >= 2 && hi < p.height - 1)
						ConvertLineToYUVWithOSD_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					else
						ConvertLineToYUVWithOSD_P8D1(lpy1, lpuv1, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					if (hi >= 2 && hi < p.height - 2)
//					if (hi >= 2 && hi < p.height - 1)
						ConvertLineToYUV_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width);
					else
						ConvertLineToYUV_P8D1(lpy1, lpuv1, pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
				lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
				lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
				lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
				lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
				lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi >= 2 && hi < p.height - 2)
//					if (hi >= 2 && hi < p.height - 1)
						ConvertLineToYUVWithOSD_P8D2MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					else
						ConvertLineToYUVWithOSD_P8D2(lpy1, lpuv1, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					{
					if (hi >= 2 && hi < p.height - 2)
//					if (hi >= 2 && hi < p.height - 1)
						ConvertLineToYUV_P8D2MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, pm, p.width);
					else
						ConvertLineToYUV_P8D2(lpy1, lpuv1, pm, p.width);
					}

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy0  = p.py  + p.bpr * ((hi & p.stripeMask) & ~1);
				lpuv0 = p.puv + p.bpr * (((hi & p.stripeMask) >> 1) & ~1);

				ConvertLineToYUV_P8DS(lpy0, lpuv0, pm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				}
			break;
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAI_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2;
	BYTE * lpuv0, * lpuv1, * lpuv2;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVAI_P8DS(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

			if (hi >= 2 && hi < p.height - 2)
//			if (hi >= 2 && hi < p.height - 1)
				{
				ConvertLineToYUV_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, yuvcache, p.width);
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
				}
			else
				ConvertLineToYUV_P8F2(lpy1, lpuv1, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAIThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2;
	BYTE * lpuv0, * lpuv1, * lpuv2;
	BYTE * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVAIThroughOSD_P8DS(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi >= 2 && hi < p.height - 2)
//				if (hi >= 2 && hi < p.height - 1)
					ConvertLineToYUVWithOSD_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				else
					ConvertLineToYUVWithOSD_P8D1(lpy1, lpuv1, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				{
				if (hi >= 2 && hi < p.height - 2)
//				if (hi >= 2 && hi < p.height - 1)
					ConvertLineToYUV_P8D1MV(lpy0, lpuv0, lpy1, lpuv1, lpy2, lpuv2, yuvcache, p.width);
				else
					ConvertLineToYUV_P8D1(lpy1, lpuv1, yuvcache, p.width);
				}

			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAI_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3;
	BYTE * lpuv0, * lpuv1, * lpuv2, * lpuv3;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	if (p.yfraction == 0x10000 || p.xscale != 0)
		{
		ConvertStripeToYUVAI_P8DS(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = ycount & 0xffff;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);
			lpy3  = p.py  + p.bpr * (hi + 2 & p.stripeMask);
			lpuv3 = p.puv + p.bpr * ((hi + 4 & p.stripeMask) >> 1);

			if (hi >= 2 && hi < p.height - 2)
				{
				filter[0] = (0x10000 - lo) >> 12;
				filter[1] = (0x10000 + lo) >> 12;
				filter[2] = (0x20000 - lo) >> 12;
				filter[3] = (0x00000 + lo) >> 12;

				ConvertLineToYUV_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, pm, p.width, filter);
				}
			else
				ConvertLineToYUV_P8D1(lpy1, lpuv1, pm, p.width);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAIThroughOSD_P8DSY2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3;
	BYTE * lpuv0, * lpuv1, * lpuv2, * lpuv3;
	BYTE * lpo;
	int hi, yo, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	if (p.yfraction == 0x10000 || p.xscale != 0)
		{
		ConvertStripeToYUVAIThroughOSD_P8DS(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = ycount & 0xffff;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);
			lpy3  = p.py  + p.bpr * (hi + 2 & p.stripeMask);
			lpuv3 = p.puv + p.bpr * ((hi + 4 & p.stripeMask) >> 1);

			yo = hi - p.yosdoffset;

			filter[0] = (0x10000 - lo) >> 12;
			filter[1] = (0x10000 + lo) >> 12;
			filter[2] = (0x20000 - lo) >> 12;
			filter[3] = (0x00000 + lo) >> 12;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi >= 2 && hi < p.height - 2)
					ConvertLineToYUVWithOSD_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, filter);
				else
					ConvertLineToYUVWithOSD_P8D1(lpy1, lpuv1, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				{
				if (hi >= 2 && hi < p.height - 2)
					ConvertLineToYUV_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, pm, p.width, filter);
				else
					ConvertLineToYUV_P8D1(lpy1, lpuv1, pm, p.width);
				}

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAI_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3;
	BYTE * lpuv0, * lpuv1, * lpuv2, * lpuv3;
	int hi, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVAI_P8DSY2(p);
		}
	else if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVAI_P8F2(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = ycount & 0xffff;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);
			lpy3  = p.py  + p.bpr * (hi + 2 & p.stripeMask);
			lpuv3 = p.puv + p.bpr * ((hi + 4 & p.stripeMask) >> 1);

			if (hi >= 2 && hi < p.height - 2)
				{
				filter[0] = (0x10000 - lo) >> 12;
				filter[1] = (0x10000 + lo) >> 12;
				filter[2] = (0x20000 - lo) >> 12;
				filter[3] = (0x00000 + lo) >> 12;

				ConvertLineToYUV_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, yuvcache, p.width, filter);
				ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
				}
			else
				ConvertLineToYUV_P8F2(lpy1, lpuv1, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

// new -> 01/26/2000
void XMMXYUVVideoConverter::ConvertStripeToYUVAIThroughOSD_P8F2Y2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy0, * lpy1, * lpy2, * lpy3;
	BYTE * lpuv0, * lpuv1, * lpuv2, * lpuv3;
	BYTE * lpo;
	int hi, yo, lo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	short filter[4];

	if (p.xfraction == 0x10000)
		{
		ConvertStripeToYUVAIThroughOSD_P8DSY2(p);
		}
	else if (p.yfraction == 0x10000)
		{
		ConvertStripeToYUVAIThroughOSD_P8F2(p);
		}
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;
			lo = ycount & 0xffff;

			lpy0  = p.py  + p.bpr * (hi - 1 & p.stripeMask);
			lpuv0 = p.puv + p.bpr * ((hi - 2 & p.stripeMask) >> 1);
			lpy1  = p.py  + p.bpr * (hi & p.stripeMask);
			lpuv1 = p.puv + p.bpr * ((hi & p.stripeMask) >> 1);
			lpy2  = p.py  + p.bpr * (hi + 1 & p.stripeMask);
			lpuv2 = p.puv + p.bpr * ((hi + 2 & p.stripeMask) >> 1);
			lpy3  = p.py  + p.bpr * (hi + 2 & p.stripeMask);
			lpuv3 = p.puv + p.bpr * ((hi + 4 & p.stripeMask) >> 1);

			filter[0] = (0x10000 - lo) >> 12;
			filter[1] = (0x10000 + lo) >> 12;
			filter[2] = (0x20000 - lo) >> 12;
			filter[3] = (0x00000 + lo) >> 12;

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi >= 2 && hi < p.height - 2)
					ConvertLineToYUVWithOSD_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd, filter);
				else
					ConvertLineToYUVWithOSD_P8D1(lpy1, lpuv1, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				{
				if (hi >= 2 && hi < p.height - 2)
					ConvertLineToYUV_P8D1Y4MUV(lpy0, lpy2, lpy1, lpy3, lpuv0, lpuv1, yuvcache, p.width, filter);
				else
					ConvertLineToYUV_P8D1(lpy1, lpuv1, yuvcache, p.width);
				}

			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}
// <- new 01/26/2000

void XMMXYUVVideoConverter::ConvertStripeToYUVDI2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lpytb = p.pytb + p.bpr * (hi & ~1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
						}
					}

				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * (hi | 1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				lpytb = p.pytb + p.bpr * (hi | 1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

				if (hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) == 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8D1(lppy, lppuv, ppm, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lpytb = p.pytb + p.bpr * (hi & ~1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmt, p.width);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmt, p.width);
						}
					}

				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * (hi | 1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				lpytb = p.pytb + p.bpr * (hi | 1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

				if (hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmb, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) == 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8D2(lppy, lppuv, ppm, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmb, p.width);
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lppuv = p.puv + p.bpr * ((hi >> 1) | 1);

				ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);
				ConvertLineToYUV_P8DS(lppy, lppuv, ppm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				ppm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lpytb = p.pytb + p.bpr * (hi & ~1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy, lpuv, pm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmt, p.width);
							}
						}
					}

				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * (hi | 1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				lpytb = p.pytb + p.bpr * (hi | 1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4 && hi >= 4)
//					if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1(lppy, lppuv, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) == 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1(lppy, lppuv, ppm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpytb, lpuvtb, pmb, p.width);
							}
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy = p.ppy + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lpytb = p.pytb + p.bpr * (hi & ~1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmt, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D2I4(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D2MUV(lpytb, lpuvtb, pmt, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D2I4MUV(lpy, lpuv, lppy, p.bpr, pm, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2(lpy, lpuv, pm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmt, p.width);
							}
						}
					}

				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * (hi | 1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
				lpytb = p.pytb + p.bpr * (hi | 1);
				lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4 && hi >= 4)
					//if (hi < p.height - 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2I4(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2(lppy, lppuv, ppm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUVWithOSD_P8D2(lpytb, lpuvtb, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D2I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D2MUV(lpytb, lpuvtb - dbpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D2I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, ppm, p.width);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmb, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) == 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2(lppy, lppuv, ppm, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2(lpytb, lpuvtb, pmb, p.width);
							}
						}
					}

				pm += p.mbpr;
				ppm += p.mbpr;
				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		default:
			for(y = p.ystart; y < p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lppy  = p.ppy  + p.bpr * (hi | 1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
				lppuv = p.puv + p.bpr * ((hi >> 1) | 1);

				ConvertLineToYUV_P8DS(lpy, lpuv, pm, p.width, p.xscale);
				ConvertLineToYUV_P8DS(lppy, lppuv, ppm, p.width, p.xscale);

				ycount += p.yfraction;
				pm += p.mbpr;
				ppm += p.mbpr;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDI2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUVDI2_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lppy = p.ppy + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
			lpytb = p.pytb + p.bpr * (hi & ~1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

			if (hi < p.height - 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, 0, p.xfraction);
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							}
						break;
					}
				}
			else
				{
				if ((hi & 1) != 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
						ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, 0, p.xfraction);
					}
				}

			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * (hi | 1);
			lppy  = p.ppy  + p.bpr * (hi | 1);
			lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
			lpytb = p.pytb + p.bpr * (hi | 1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

			if (hi >= 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
							}
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width);
							ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, 0, p.xfraction);
						break;
					}
				}
			else
				{
				if ((hi & 1) == 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
						ConvertLineToYUV_P8F2(lppy, lppuv, ppm, p.mwidth, 0, p.xfraction);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
						ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth,0, p.xfraction);
					}
				}

			pm += p.mbpr;
			ppm += p.mbpr;
			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVDI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpytb, * lpuvtb, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;
	BYTE * ppm = p.ppm;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUVDI2ThroughOSD_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lppy = p.ppy + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);
			lpytb = p.pytb + p.bpr * (hi & ~1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) & ~1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						}
					}
				}
			else
				{
				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, 0, p.xfraction);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4MUV(lpy, lpuv, lppy, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy, lpuv, pm, p.mwidth, 0, p.xfraction);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmt, p.mwidth, 0, p.xfraction);
						}
					}
				}

			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * (hi | 1);
			lppy  = p.ppy  + p.bpr * (hi | 1);
			lppuv = p.ppuv + p.bpr * ((hi >> 1) | 1);
			lpytb = p.pytb + p.bpr * (hi | 1);
			lpuvtb = p.puvtb + p.bpr * ((hi >> 1) | 1);

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4 && hi >= 4)
//				if (hi < p.height - 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUVWithOSD_P8D1(lppy, lppuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUVWithOSD_P8D1(lpytb, lpuvtb, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						}
					}
				}
			else
				{
				if (hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4MUV(lppy - dbpr, lppuv - dbpr, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpytb, lpuvtb - dbpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1I4(lppy - dbpr, lppuv, lpy - dbpr, p.bpr, yuvcache, p.width);
								ScaleYUVLine_P8F2(yuvcache, ppm, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, 0, p.xfraction);
							break;
						}
					}
				else
					{
					if ((hi & 1) == 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8F2(lppy, lppuv, ppm, p.mwidth, 0, p.xfraction);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpytb, lpuvtb, pmb, p.mwidth, 0, p.xfraction);
						}
					}
				}

			pm += p.mbpr;
			ppm += p.mbpr;
			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVSI2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1MY(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1MYUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2MY(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2MUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2MYUV(lpy, lpuv, pmt, p.width, p.bpr);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVSI2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4 && hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MY(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MY(lpy - p.bpr, lpuv + p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D1MYUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi < p.height - 4 && hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1MY(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D1MUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D1MYUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D1(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D1(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy  = p.py  + p.bpr * (hi & ~1);
				lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					if (hi < p.height - 4 && hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2MYUV(lpy - p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2MY(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D2MUV(lpy + p.bpr, lpuv - p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUVWithOSD_P8D2MUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2MY(lpy - p.bpr, lpuv + p.bpr, p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUVWithOSD_P8D2MYUV(lpy, lpuv, p.bpr, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUVWithOSD_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUVWithOSD_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUVWithOSD_P8D2(lpy, lpuv, pmt, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUVWithOSD_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							}
						}
					}
				else
					{
					if (hi < p.height - 4 && hi >= 4)
						{
						switch (hi & 3)
							{
							case 0:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D2MYUV(lpy - p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 1:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D2MY(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D2MUV(lpy + p.bpr, lpuv - p.bpr, pmb, p.width, p.bpr);
								break;
							case 2:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
									ConvertLineToYUV_P8D2MUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
									ConvertLineToYUV_P8D2MY(lpy - p.bpr, lpuv + p.bpr, pmb, p.width, p.bpr);
								break;
							case 3:
								if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
									ConvertLineToYUV_P8D2MYUV(lpy, lpuv, pmt, p.width, p.bpr);
								if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
									ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
								break;
							}
						}
					else
						{
						if ((hi & 1) != 0)
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						else
							{
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8D2(lpy, lpuv, pmt, p.width);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								ConvertLineToYUV_P8D2(lpy + p.bpr, lpuv + p.bpr, pmb, p.width);
							}
						}
					}

				pmb += p.mbpr;
				pmt += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVSI2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUVSI2_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

			if (hi < p.height - 4 && hi >= 4)
				{
				switch (hi & 3)
					{
					case 0:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						break;
					case 1:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MY(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						break;
					case 2:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						break;
					case 3:
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUV_P8D1MYUV(lpy, lpuv, yuvcache, p.width, p.bpr);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
						break;
					}
				}
			else
				{
				if ((hi & 1) != 0)
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
						ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
					}
				else
					{
					if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
						ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
					if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
						ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
					}
				}

			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVSI2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lpuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pmt = p.pmt;
	BYTE * pmb = p.pmb;
	int dbpr = p.bpr * 2;

	if	(p.xfraction == 0x10000)
		ConvertStripeToYUVSI2ThroughOSD_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy  = p.py  + p.bpr * (hi & ~1);
			lpuv = p.puv + p.bpr * ((hi >> 1) & ~1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1MY(lpy, lpuv, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1MUV(lpy, lpuv, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1MY(lpy - p.bpr, lpuv + p.bpr, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUVWithOSD_P8D1MYUV(lpy, lpuv, p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							{
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							{
							ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							{
							ConvertLineToYUVWithOSD_P8D1(lpy, lpuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
							}
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							{
							ConvertLineToYUVWithOSD_P8D1(lpy + p.bpr, lpuv + p.bpr, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
							ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
							}
						}
					}
				}
			else
				{
				if (hi < p.height - 4 && hi >= 4)
					{
					switch (hi & 3)
						{
						case 0:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy - p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 1:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy + p.bpr, lpuv - p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 2:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
								{
								ConvertLineToYUV_P8D1MUV(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MY(lpy - p.bpr, lpuv + p.bpr, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmb, p.mwidth, 0, p.xfraction);
								}
							break;
						case 3:
							if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
								{
								ConvertLineToYUV_P8D1MYUV(lpy, lpuv, yuvcache, p.width, p.bpr);
								ScaleYUVLine_P8F2(yuvcache, pmt, p.mwidth, 0, p.xfraction);
								}
							if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
								ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
							break;
						}
					}
				else
					{
					if ((hi & 1) != 0)
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_COPY)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
						}
					else
						{
						if (p.deinterlaceFlags & DIFEF_ENABLE_TOP_COPY)
							ConvertLineToYUV_P8F2(lpy, lpuv, pmt, p.mwidth, 0, p.xfraction);
						if (p.deinterlaceFlags & DIFEF_ENABLE_BOTTOM_DEINTERLACE)
							ConvertLineToYUV_P8F2(lpy + p.bpr, lpuv + p.bpr, pmb, p.mwidth, 0, p.xfraction);
						}
					}
				}

			pmb += p.mbpr;
			pmt += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVA2_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * hi;
				lppy  = p.ppy  + p.bpr * hi;
				lpuv  = p.puv  + p.bpr * (hi >> 1);
				lppuv = p.ppuv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * hi;
				lppy  = p.ppy  + p.bpr * hi;
				lpuv  = p.puv  + p.bpr * (hi >> 1);
				lppuv = p.ppuv + p.bpr * (hi >> 1);

				ConvertLineToYUV_P8D2M(lpy, lpuv, lppy, lppuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVA2ThroughOSD_P8DS(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	switch (p.xscale)
		{
		case 0:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * hi;
				lppy  = p.ppy  + p.bpr * hi;
				lpuv  = p.puv  + p.bpr * (hi >> 1);
				lppuv = p.ppuv + p.bpr * (hi >> 1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		case 1:
			for(y=p.ystart; y<p.ystop; y++)
				{
				hi = ycount >> 16;

				lpy   = p.py   + p.bpr * hi;
				lppy  = p.ppy  + p.bpr * hi;
				lpuv  = p.puv  + p.bpr * (hi >> 1);
				lppuv = p.ppuv + p.bpr * (hi >> 1);

				yo = hi - p.yosdoffset;

				if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
					{
					lpo = p.po + p.obpr * (hi - p.yosdstart);

					ConvertLineToYUVWithOSD_P8D2M(lpy, lpuv, lppy, lppuv, pm, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
					}
				else
					ConvertLineToYUV_P8D2M(lpy, lpuv, lppy, lppuv, pm, p.width);

				pm += p.mbpr;
				ycount += p.yfraction;
				}
			break;
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVA2_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv;
	int hi;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUVA2_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * hi;
			lppy  = p.ppy  + p.bpr * hi;
			lpuv  = p.puv  + p.bpr * (hi >> 1);
			lppuv = p.ppuv + p.bpr * (hi >> 1);

			ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, yuvcache, p.width);
			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}

void XMMXYUVVideoConverter::ConvertStripeToYUVA2ThroughOSD_P8F2(const YUVVideoConverterParameter & p)
	{
	int y;
	BYTE * lpy, * lppy, * lpuv, * lppuv, * lpo;
	int hi, yo;
	int ycount = p.ycount;
	BYTE * pm = p.pm;

	if (p.xfraction == 0x10000)
		ConvertStripeToYUVA2ThroughOSD_P8DS(p);
	else
		{
		for(y=p.ystart; y<p.ystop; y++)
			{
			hi = ycount >> 16;

			lpy   = p.py   + p.bpr * hi;
			lppy  = p.ppy  + p.bpr * hi;
			lpuv  = p.puv  + p.bpr * (hi >> 1);
			lppuv = p.ppuv + p.bpr * (hi >> 1);

			yo = hi - p.yosdoffset;

			if (hi >= p.yosdstart && hi < p.yosdstop && (!p.osdNonZeroMap || ((p.osdNonZeroMap[yo >> 5] & (1 << (yo & 0x1f))) != 0)))
				{
				lpo = p.po + p.obpr * (hi - p.yosdstart);

				ConvertLineToYUVWithOSD_P8D1M(lpy, lpuv, lppy, lppuv, yuvcache, p.width, p.xosdstart, p.xosdstop, lpo, p.dd);
				}
			else
				ConvertLineToYUV_P8D1M(lpy, lpuv, lppy, lppuv, yuvcache, p.width);

			ScaleYUVLine_P8F2(yuvcache, pm, p.mwidth, 0, p.xfraction);

			pm += p.mbpr;
			ycount += p.yfraction;
			}
		}
	}





#pragma warning (default : 4799)
