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


#include "GenericMMXMacroBlockIDCTDecoder.h"

#include "..\common\MMXExtensionMacros.h"
//#include "driver\SoftwareCinemaster\common\ATHLONMacros.h"

#define DUMP_COEFFICIENTS	0
#define MEASURE_PARSE_TIME	0

#if DUMP_COEFFICIENTS
#include <stdio.h>

FILE * dfile;
static int blockcount;

#endif

#pragma warning (disable : 4799)

static unsigned char hflut[2][16] =
	{
		{1, 1, 2, 3, 4, 5, 6, 7, 8, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 2, 3, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1}
	};

static unsigned char hfluti[16] =
	{1, 1, 2, 3, 4, 5, 6, 7, 8, 1, 1, 1, 1, 1, 1, 1};

#define HIGH_PRECISION_IDCT		0

#if HIGH_PRECISION_IDCT

static inline double R1(double x)
	{
	return floor(x * (1 << 4));
	}

static inline double R2(double x)
	{
	return floor(x / (1 << 0));
	}

static inline double R2a(double x)
	{
	return floor(x / (1 << 0));
	}

void GenericMMXMacroBlockIDCTDecoder::InverseDCT(short * mb, int stride, DCTMulTable * table, int numLines, int numColumns)
	{
	double fmmxf[8][8];
	int y, x;
	double y0, y1, y2, y3, a0, a1, a2, a3, y4, y6, y5, y7;

	static int maxco, minco;

	__asm emms

	for (y=0; y < 8; y++)
		{
		y0 = R1(tmmxf[y][0] * DC8) + R1(tmmxf[y][1] * DC8);
		y1 = R1(tmmxf[y][0] * DC8) - R1(tmmxf[y][1] * DC8);
		y2 = R1(tmmxf[y][2] * DC2) + R1(tmmxf[y][3] * DC6);
		y3 = R1(tmmxf[y][2] * DC6) - R1(tmmxf[y][3] * DC2);
		a0 = y0 + y2;
		a1 = y1 + y3;
		a2 = y1 - y3;
		a3 = y0 - y2;
		y4 = R1(tmmxf[y][4] * DC1) + R1(tmmxf[y][6] * DC3) + R1(tmmxf[y][5] * DC5) + R1(tmmxf[y][7] * DC7);
		y6 = R1(tmmxf[y][4] * DC3) - R1(tmmxf[y][6] * DC7) - R1(tmmxf[y][5] * DC1) - R1(tmmxf[y][7] * DC5);
		y5 = R1(tmmxf[y][4] * DC7) - R1(tmmxf[y][6] * DC5) + R1(tmmxf[y][5] * DC3) - R1(tmmxf[y][7] * DC1);
		y7 = R1(tmmxf[y][4] * DC5) - R1(tmmxf[y][6] * DC1) + R1(tmmxf[y][5] * DC7) + R1(tmmxf[y][7] * DC3);
		fmmxf[y][0] = a0 + y4;
		fmmxf[y][1] = a1 + y6;
		fmmxf[y][2] = a2 + y7;
		fmmxf[y][3] = a3 + y5;
		fmmxf[y][4] = a3 - y5;
		fmmxf[y][5] = a2 - y7;
		fmmxf[y][6] = a1 - y6;
		fmmxf[y][7] = a0 - y4;
		}

	for (y=0; y < 8; y++)
		{
		y0 = R2(fmmxf[0][y] * DC8) + R2(fmmxf[4][y] * DC8);
		y1 = R2(fmmxf[0][y] * DC8) - R2(fmmxf[4][y] * DC8);
		y2 = R2(fmmxf[2][y] * DC2) + R2(fmmxf[6][y] * DC6);
		y3 = R2(fmmxf[2][y] * DC6) - R2(fmmxf[6][y] * DC2);
		a0 = y0 + y2;
		a1 = y1 + y3;
		a2 = y1 - y3;
		a3 = y0 - y2;
		y4 = R2(fmmxf[1][y] * DC1) + R2(fmmxf[3][y] * DC3) + R2(fmmxf[5][y] * DC5) + R2(fmmxf[7][y] * DC7);
		y6 = R2(fmmxf[1][y] * DC3) - R2(fmmxf[3][y] * DC7) - R2(fmmxf[5][y] * DC1) - R2(fmmxf[7][y] * DC5);
		y7 = R2(fmmxf[1][y] * DC5) - R2(fmmxf[3][y] * DC1) + R2(fmmxf[5][y] * DC7) + R2(fmmxf[7][y] * DC3);
		y5 = R2(fmmxf[1][y] * DC7) - R2(fmmxf[3][y] * DC5) + R2(fmmxf[5][y] * DC3) - R2(fmmxf[7][y] * DC1);
		fmmxf[0][y] = a0 + y4;
		fmmxf[1][y] = a1 + y6;
		fmmxf[2][y] = a2 + y7;
		fmmxf[3][y] = a3 + y5;
		fmmxf[4][y] = a3 - y5;
		fmmxf[5][y] = a2 - y7;
		fmmxf[6][y] = a1 - y6;
		fmmxf[7][y] = a0 - y4;
		}

	for(y=0; y<8; y++)
		{
		for(x=0; x<8; x++)
			{
			int t = (int)(floor(fmmxf[y][x]) + 0);
			t = (t + 64) >> 7;

			mb[y*(stride/2)+x] = t;
			tmmxf[y][x] = 0;
			}
		}
	}
#else
void GenericMMXMacroBlockIDCTDecoder::InverseDCT(short * mb, int stride, DCTMulTable * table, int numLines, int numColumns)
	{
	static MMXQWORD RoundOne0    = {ROUND_ONE, ROUND_ONE, ROUND_ONE, ROUND_ONE};
	static MMXQWORD RoundSingle0 = {ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE};

	static MMXQWORD RoundOne1    = {ROUND_ONE, ROUND_ONE, ROUND_ONE, ROUND_ONE};
	static MMXQWORD RoundSingle1 = {ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE};

	static MMXQWORD RoundSecond0 = {ROUND_SECOND, ROUND_SECOND, ROUND_SECOND, ROUND_SECOND};
	static MMXQWORD RoundSecond1 = {ROUND_SECOND, ROUND_SECOND, ROUND_SECOND, ROUND_SECOND};

	static MMXQWORD RoundSecond0a = {ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4};
	static MMXQWORD RoundSecond1a = {ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4};

	switch (numColumns)
		{
		case 1:
			if (numLines == 1)
				{
				__asm
					{
					mov			esi, [table]
					mov			edx, [stride]

					movq			mm0, [tmmxf]
					pxor			mm2, mm2

					movq			mm1, [RoundOne1]
					punpcklwd	mm0, mm0

					mov	ebx, [mb]
					punpckldq	mm0, mm0

					pmulhw		mm0, [esi].tpC8pC8pC8pC8

					lea			eax, [ebx+2*edx]

					lea			ecx, [ebx+2*edx]

					lea			eax, [eax+edx]

					paddw			mm1, mm0
					paddw			mm0, [RoundOne0]

					movq			[tmmxf], mm2
					psraw			mm0, ONE_SHIFT

					movq			[ebx], mm0
					movq			[ebx+8], mm0

					psraw			mm1, ONE_SHIFT

					movq			[ebx+2*edx], mm0
  					movq			[ebx+2*edx+8], mm0

					movq			[ebx+4*edx], mm0
					movq			[ebx+4*edx+8], mm0

					movq			[ecx+4*edx], mm0
					movq			[ecx+4*edx+8], mm0

					movq			[ebx+edx], mm1
					movq			[ebx+edx+8], mm1

					movq			[eax], mm1
					movq			[eax+8], mm1

					movq			[eax+2*edx], mm1
					movq			[eax+2*edx+8], mm1

					movq			[eax+4*edx], mm1
					movq			[eax+4*edx+8], mm1
					}

				return;
				}
			else
				{
				__asm
					{
					lea			eax, tmmxf
					mov			esi, [table]

					mov			ecx, [numLines]

					movq			mm1, [eax+0]

					pmaddwd		mm1, [esi].tpC8000000000

					sub			ecx, 1
loop11:
					movq			mm0, [eax+16]
					psrad			mm1, FIRST_SHIFT

					pmaddwd		mm0, [esi].tpC8000000000
					punpckldq	mm1, mm1

					packssdw		mm1, mm1
					sub			ecx, 1

					movq			[eax], mm1

					movq			[eax + 8], mm1
					movq			mm1, mm0

					lea			eax, [eax + 16]
					jne			loop11

					psrad			mm1, FIRST_SHIFT

					punpckldq	mm1, mm1

					packssdw		mm1, mm1

					movq			[eax], mm1

					movq			[eax + 8], mm1
					}
				}
			break;
		case 2:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]

				movq			mm1, [eax+8]

				punpcklwd	mm1, [eax+0]

				movq			mm4, [esi].tpC8pC7pC8pC5
				punpckldq	mm1, mm1

				movq			mm2, [esi].tpC8pC3pC8pC1
				pmaddwd		mm4, mm1

				movq			mm3, [esi].tpC8mC1pC8mC3
				pmaddwd		mm2, mm1

				movq			mm5, mm1
				pmaddwd		mm3, mm1

				pmaddwd		mm5, [esi].tpC8mC5pC8mC7
				psrad			mm4, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				je				done12
loop12:
				movq			mm1, [eax+24]
				psrad			mm5, FIRST_SHIFT

				punpcklwd	mm1, [eax+16]
				packssdw		mm2, mm4

				movq			mm4, [esi].tpC8pC7pC8pC5
				punpckldq	mm1, mm1

				movq			[eax], mm2
				packssdw		mm5, mm3

				movq			mm2, [esi].tpC8pC3pC8pC1
				pmaddwd		mm4, mm1

				movq			mm3, [esi].tpC8mC1pC8mC3
				pmaddwd		mm2, mm1

				movq			[eax + 8], mm5
				movq			mm5, mm1

				pmaddwd		mm3, mm1
				psrad			mm4, FIRST_SHIFT

				pmaddwd		mm5, [esi].tpC8mC5pC8mC7
				psrad			mm2, FIRST_SHIFT

				lea			eax, [eax+16]
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				jne			loop12
done12:
				psrad			mm5, FIRST_SHIFT

				packssdw		mm2, mm4

				packssdw		mm5, mm3

				movq			[eax], mm2

				movq			[eax + 8], mm5
				}
			break;
		case 3:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]
				movq			mm0, [eax+0]

				movq			mm4, [eax+8]
				movq			mm2, mm0

				pmaddwd		mm0, [esi].tpC8000000000
				psrlq			mm2, 32

				punpcklwd	mm4, mm2

				movq			mm1, [esi].tmC2pC7mC6pC5
				punpckldq	mm4, mm4

				movq			mm2, [esi].tpC6pC3pC2pC1
				pmaddwd		mm1, mm4

				movq			mm3, [esi].tpC2mC1pC6mC3
				punpckldq	mm0, mm0

				pmaddwd		mm2, mm4
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm3, mm4
				psrad			mm1, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				packssdw		mm0, mm0

				psrad			mm2, FIRST_SHIFT

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				sub			ecx, 1
				je				done13
loop13:
				movq			mm5, [eax+16]
				packssdw		mm2, mm1

				movq			mm1, [eax+24]
				paddsw		mm2, mm0

				movq			[eax], mm2
				movq			mm2, mm5

				pmaddwd		mm5, [esi].tpC8000000000
				packssdw		mm4, mm3

				psrlq			mm2, 32
				paddsw		mm4, mm0

				movq			[eax + 8], mm4
				punpcklwd	mm1, mm2

				punpckldq	mm1, mm1
				movq			mm0, mm5

				movq			mm2, [esi].tpC6pC3pC2pC1
				movq			mm4, mm1

				pmaddwd		mm1, [esi].tmC2pC7mC6pC5

				movq			mm3, [esi].tpC2mC1pC6mC3
				punpckldq	mm0, mm0

				pmaddwd		mm2, mm4
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm3, mm4
				psrad			mm1, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				packssdw		mm0, mm0

				psrad			mm2, FIRST_SHIFT
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				lea			eax, [eax+16]

				psrad			mm4, FIRST_SHIFT
				jne			loop13
done13:
				packssdw		mm2, mm1

				packssdw		mm4, mm3
				paddsw		mm2, mm0

				paddsw		mm4, mm0

				movq	[eax], mm2

				movq	[eax + 8], mm4

				}
			break;
		case 4:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]

loop14:
				movq			mm0, [eax+0]	// 00 f2 00 f0

				movq			mm1, [eax+8]	// 00 f3 00 f1
				psrlq			mm0, 16

				por			mm0, [eax+0]
				psrlq			mm1, 16

				por			mm1, [eax+8]
				punpckldq	mm0, mm0			// f2 f0 f2 f0

				movq			mm2, [esi].tpC6pC8pC2pC8
				punpckldq	mm1, mm1			// f3 f1 f3 f1

				movq			mm3, [esi].tmC7pC3pC3pC1
				pmaddwd		mm2, mm0

				pmaddwd		mm0, [esi].tmC2pC8mC6pC8

				pmaddwd		mm3, mm1

				pmaddwd		mm1, [esi].tmC5pC7mC1pC5
				movq			mm4, mm2

				movq			mm5, mm0
				paddd			mm2, mm3

				psrad			mm2, FIRST_SHIFT
				paddd			mm0, mm1

				psrad			mm0, FIRST_SHIFT
				psubd			mm4, mm3

				psrad			mm4, FIRST_SHIFT
				psubd			mm5, mm1

				psrad			mm5, FIRST_SHIFT

				packssdw		mm5, mm4
				add			eax, 16

				movq			mm4, mm5
				packssdw		mm2, mm0

				pslld			mm5, 16

				psrld			mm4, 16

				por			mm4, mm5

				movq			[eax-16], mm2

				movq			[eax-8], mm4

				sub			ecx, 1
				jne			loop14
				}
			break;
		default:
			__asm
				{
				mov			esi, [table]

				lea			eax, [tmmxf]
				mov			ecx, [numLines]
loop18a:
				movq			mm0, [eax+0]
				movq			mm1, mm0
				dec			ecx

				movq			mm6, [eax+8]			// f7 f3 f5 f1
				punpckldq	mm0, mm0					// f4 f0 f4 f0
				movq			mm7, mm6

				pmaddwd		mm0, [esi].tmC8pC8pC8pC8
				punpckhdq	mm1, mm1					// f6 f2 f6 f2

				pmaddwd		mm1, [esi].tmC2pC6pC6pC2
				movq			mm2, mm0

				movq			mm4, [esi].tmC1pC3pC5pC1
				lea			eax, [eax + 16]
				punpckldq	mm6, mm6					// f5 f1 f5 f1

				punpckhdq	mm7, mm7					// f7 f3 f7 f3
				pmaddwd		mm4, mm6					// y6a y4a

				pmaddwd		mm6, [esi].tpC3pC7pC7pC5	// y5a y7a
				paddd			mm0, mm1					// a1 a0
				psubd			mm2, mm1					// a3 a2

				movq			mm5, [esi].tmC5mC7pC7pC3
				pmaddwd		mm5, mm7					// y6b y4b
				punpckldq	mm1, mm2

				pmaddwd		mm7, [esi].tmC1mC5pC3mC1	// y5b y7b
				punpckhdq	mm2, mm1					// a2 a3

				paddd			mm4, mm5
				paddd			mm6, mm7

				movq			mm7, mm0
				movq			mm5, mm2

				paddd			mm0, mm4					// t1 t0
				paddd			mm2, mm6					// t2 t3

				psrad			mm0, FIRST_SHIFT
				psrad			mm2, FIRST_SHIFT

				psubd			mm7, mm4					// t6 t7
				psubd			mm5, mm6					// t4 t5

				psrad			mm7, FIRST_SHIFT
				psrad			mm5, FIRST_SHIFT

				packssdw		mm0, mm2					// t3 t2 t1 t0
				packssdw		mm5, mm7					// t6 t7 t4 t5

				movq			mm7, mm5
				pslld			mm5, 16

				movq			[eax-16], mm0

				psrld			mm7, 16
				por			mm5, mm7					// t4 t5 t6 t7

				movq			[eax-8], mm5

				jne			loop18a
				}
			break;
		}

	switch (numLines)
		{
		case 1:
			__asm
				{
				movq		mm2, [RoundSingle0]
				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm2
				psraw		mm0, INTER_PREC

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm2
				psraw		mm1, INTER_PREC

				movq		[tmmxf + 0 * 16 + 0], mm7
				mov		ebx, [mb]
				mov		edx, [stride]

				movq		[tmmxf + 0 * 16 + 8], mm7
				lea		ecx, [ebx+4*edx]
				lea		esi, [ebx+2* edx]
				lea		edi, [ecx+2*edx]

				movq		[ebx+edx], mm0
				movq		[ebx+edx+8], mm1
				movq		[ecx+edx], mm0
				movq		[ecx+edx+8], mm1
				movq		[ebx], mm0
				movq		[ebx+8], mm1
				movq		[ecx], mm0
				movq		[ecx+8], mm1
				movq		[esi], mm0
				movq		[esi+8], mm1
				movq		[edi], mm0
				movq		[edi+8], mm1
				add		esi, edx
				movq		[esi], mm0
				add		edi, edx
				movq		[esi+8], mm1
				movq		[edi], mm0
				movq		[edi+8], mm1
				}
			break;
		case 2:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7


				//////////////////////

				movq		mm3, [pC1pC1pC1pC1a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm4
				movq		[ebx+8], mm6
				movq		[ecx+esi], mm5
				movq		[ecx+esi+8], mm7


				movq		mm3, [pC3pC3pC3pC3a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+edx], mm4
				movq		[ebx+edx+8], mm6
				movq		[ecx+2*edx], mm5
				movq		[ecx+2*edx+8], mm7


				movq		mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				paddsw	mm2, mm2
				paddsw	mm3, mm3

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+2*edx], mm4
				movq		[ebx+2*edx+8], mm6
				movq		[ecx+edx], mm5
				movq		[ecx+edx+8], mm7

				movq		mm3, [pC7pC7pC7pC7a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+esi], mm4
				movq		[ebx+esi+8], mm6
				movq		[ecx], mm5
				movq		[ecx+8], mm7

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}

			break;
		case 3:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]

				movq		mm3, [pC2pC2pC2pC2a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, [tmmxf + 2 * 16 + 0]

				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm3, [tmmxf + 2 * 16 + 8]

				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]

				movq		mm4, mm0
				movq		mm5, mm1

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC1pC1pC1pC1a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0
				movq		[ebx+8], mm1
				movq		[ecx+esi], mm6
				movq		[ecx+esi+8], mm7

				//////////

				movq		mm3, [pC7pC7pC7pC7a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+esi], mm4
				movq		[ebx+esi+8], mm5
				movq		[ecx], mm6
				movq		[ecx+8], mm7

				////////////////////////////

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm2, mm2
				paddsw	mm3, mm3

				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]
				movq		mm4, mm0
				movq		mm5, mm1

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 2 * 16 + 8], mm7

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC3pC3pC3pC3a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+edx], mm0
				movq		[ebx+edx+8], mm1
				movq		[ecx+2*edx], mm6
				movq		[ecx+2*edx+8], mm7

				//////////

				movq		mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, mm2
				paddsw	mm3, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+2*edx], mm4
				movq		[ebx+2*edx+8], mm5
				movq		[ecx+edx], mm6
				movq		[ecx+edx+8], mm7

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}
			break;
		case 4:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm7, [RoundSecond0a]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm7

				movq		mm2, [pC2pC2pC2pC2a]
				movq		mm3, mm2

				movq		mm4, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm4
				paddsw	mm2, mm4

				movq		mm4, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, mm4
				paddsw	mm3, mm4

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm3, [tmmxf + 1 * 16 + 0]
				movq		mm2, [pC1pC1pC1pC1a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 0]
				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm3, [tmmxf + 1 * 16 + 8]
				movq		mm2, [pC1pC1pC1pC1a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 8]
				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0			// r0
				movq		[ebx+8], mm1			// r0
				movq		[ecx + esi], mm6	// r7
				movq		[ecx + esi+8], mm7	// r7

				//////

				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7a]
				paddsw	mm3, mm3
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7a]
				paddsw	mm3, mm3
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + esi], mm4			// r0
				movq		[ebx + esi+8], mm5			// r0
				movq		[ecx], mm6	// r7
				movq		[ecx+8], mm7	// r7

				//////////////////////////////

				pxor		mm7, mm7
				movq		mm6, [RoundSecond0a]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm6

				movq		[tmmxf + 0 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm6

				movq		[tmmxf + 0 * 16 + 8], mm7

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		[tmmxf + 2 * 16 + 0], mm7

				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm3, mm3

				movq		[tmmxf + 2 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm3, [tmmxf + 1 * 16 + 0]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm3, [tmmxf + 1 * 16 + 8]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + edx], mm0			// r1
				movq		[ebx + edx+8], mm1			// r1
				movq		[ecx + 2 * edx], mm6	// r6
				movq		[ecx + 2 * edx+8], mm7	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1a]
				paddsw	mm3, [tmmxf + 3 * 16 + 0]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1a]
				paddsw	mm3, [tmmxf + 3 * 16 + 8]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1
				psraw		mm4, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx], mm4			// r2
				psraw		mm5, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx+8], mm5			// r2
				psraw		mm6, SECOND_SHIFT + 2

				movq		[ecx + edx], mm6	// r5
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ecx + edx+8], mm7	// r5
				}
			break;
		default:
			__asm
				{
				movq		mm6, [RoundSecond0a]
				lea		eax, [tmmxf]

				movq		mm0, [eax + 0 * 16 + 0]
				paddsw	mm0, [eax + 4 * 16 + 0]
				paddsw	mm0, mm6

				movq		mm1, [eax + 0 * 16 + 8]
				paddsw	mm1, [eax + 4 * 16 + 8]
				paddsw	mm1, mm6

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [eax + 6 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		mm5, [eax + 2 * 16 + 0]
				movq		mm4, [pC2pC2pC2pC2a]
				pmulhw	mm4, mm5

				paddsw	mm2, mm5
				paddsw	mm2, mm4 // y2 0

				pmulhw	mm3, [eax + 6 * 16 + 8]
				paddsw	mm3, mm3

				movq		mm5, [eax + 2 * 16 + 8]
				movq		mm4, [pC2pC2pC2pC2a]
				pmulhw	mm4, mm5

				paddsw	mm3, mm5
				paddsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]


				movq		mm2, [pC1pC1pC1pC1a]
				movq		mm6, [eax + 1 * 16 + 0]
				pmulhw	mm2, mm6
				paddsw	mm2, mm6

				movq		mm3, [eax + 3 * 16 + 0]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [pC1pC1pC1pC1a]
				movq		mm3, [eax + 1 * 16 + 8]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [eax + 3 * 16 + 8]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0			// r0
				movq		[ebx+8], mm1			// r0
				movq		[ecx + esi], mm6	// r7
				movq		[ecx + esi+8], mm7	// r7

				//////

				movq		mm2, [eax + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7a]

				movq		mm3, [eax + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [eax + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7a]

				movq		mm3, [eax + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]

				psubsw	mm2, mm3

				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + esi], mm4			// r0
				movq		[ebx + esi+8], mm5			// r
				movq		[ecx], mm6	// r7
				movq		[ecx+8], mm7	// r7



				//////////////////////////////

				pxor		mm7, mm7

				movq		mm6, [RoundSecond1a]

				movq		mm0, [eax + 0 * 16 + 0]
				psubsw	mm0, [eax + 4 * 16 + 0]
				paddsw	mm0, mm6

				movq		mm1, [eax + 0 * 16 + 8]
				psubsw	mm1, [eax + 4 * 16 + 8]
				paddsw	mm1, mm6

				movq		[eax + 0 * 16 + 0], mm7
				movq		[eax + 4 * 16 + 0], mm7
				movq		[eax + 0 * 16 + 8], mm7
				movq		[eax + 4 * 16 + 8], mm7

				movq		mm2, [eax + 2 * 16 + 0]
				movq		mm3, [pC6pC6pC6pC6a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		mm4, [eax + 6 * 16 + 0]
				movq		mm5, [pC2pC2pC2pC2a]
				psubsw	mm2, mm4
				pmulhw	mm4, mm5

				psubsw	mm2, mm4 // y2 0

				pmulhw	mm3, [eax + 2 * 16 + 8]
				paddsw	mm3, mm3

				movq		mm4, [eax + 6 * 16 + 8]
				psubsw	mm3, mm4
				pmulhw	mm4, mm5

				psubsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		[eax + 2 * 16 + 0], mm7
				movq		[eax + 6 * 16 + 0], mm7
				movq		[eax + 2 * 16 + 8], mm7
				movq		[eax + 6 * 16 + 8], mm7

				movq		mm6, [eax + 1 * 16 + 0]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm6
				paddsw	mm2, mm6

				movq		mm3, [eax + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm7, [eax + 1 * 16 + 8]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm7
				paddsw	mm2, mm7

				movq		mm3, [eax + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + edx], mm0			// r1
				movq		[ebx + edx+8], mm1			// r1
				movq		[ecx + 2 * edx], mm6	// r6
				movq		[ecx + 2 * edx+8], mm7	// r6
				//////
				pxor		mm1, mm1

				movq		mm2, [eax + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2

				movq		mm3, [eax + 3 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		[eax + 1 * 16 + 0], mm1
				movq		[eax + 3 * 16 + 0], mm1
				movq		[eax + 5 * 16 + 0], mm1
				movq		[eax + 7 * 16 + 0], mm1

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [eax + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2

				movq		mm3, [eax + 3 * 16 + 8]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		[eax + 1 * 16 + 8], mm1
				movq		[eax + 3 * 16 + 8], mm1
				movq		[eax + 5 * 16 + 8], mm1
				movq		[eax + 7 * 16 + 8], mm1


				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ebx + 2 * edx+8], mm5			// r2
				movq		[ecx + edx], mm6	// r5
				movq		[ecx + edx+8], mm7	// r5
				}
		}
	}
#endif



void GenericMMXMacroBlockIDCTDecoder::InverseBYTEIntraDCT(BYTE * mb, int stride, DCTMulTable * table, int numLines, int numColumns)
	{
	static MMXQWORD RoundOne0    = {ROUND_ONE, ROUND_ONE - 1, ROUND_ONE, ROUND_ONE - 1};
	static MMXQWORD RoundSingle0 = {ROUND_SINGLE, ROUND_SINGLE - 1, ROUND_SINGLE, ROUND_SINGLE - 1};
	static MMXQWORD RoundSecond0 = {ROUND_SECOND, ROUND_SECOND - 1, ROUND_SECOND, ROUND_SECOND - 1};

	static MMXQWORD RoundOne1    = {ROUND_ONE - 1, ROUND_ONE, ROUND_ONE - 1, ROUND_ONE};
	static MMXQWORD RoundSingle1 = {ROUND_SINGLE - 1, ROUND_SINGLE, ROUND_SINGLE - 1, ROUND_SINGLE};
	static MMXQWORD RoundSecond1 = {ROUND_SECOND - 1, ROUND_SECOND, ROUND_SECOND - 1, ROUND_SECOND};

	switch (numColumns)
		{
		case 1:
			if (numLines == 1)
				{
				__asm
					{
					mov	ebx, [mb]
					mov	edx, [stride]
					mov	esi, [table]

					movq			mm0, [tmmxf]

					punpcklwd	mm0, mm0

					punpckldq	mm0, mm0

					pmulhw		mm0, [esi].tpC8pC8pC8pC8


					pxor			mm2, mm2

					lea			eax, [ebx+2*edx]

					lea			ecx, [ebx+2*edx]

					movq			mm1, mm0
					lea			eax, [eax+edx]

					paddw			mm0, [RoundOne0]

					movq			[tmmxf], mm2
					psraw			mm0, ONE_SHIFT

					paddw			mm1, [RoundOne1]
					packuswb		mm0, mm0

					psraw			mm1, ONE_SHIFT

					movq			[ebx], mm0
					packuswb		mm1, mm1

					movq			[ebx+2*edx], mm0

					movq			[ebx+4*edx], mm0

					movq			[ecx+4*edx], mm0

					movq			[ebx+edx], mm1

					movq			[eax], mm1

					movq			[eax+2*edx], mm1

					movq			[eax+4*edx], mm1
					}

				return;
				}
			else
				{
				__asm
					{
					lea	eax, tmmxf
					mov	ecx, [numLines]
					mov	esi, [table]
loop11:
					movq	mm0, [eax+0]

					pmaddwd mm0, [esi].tpC8000000000

					add	eax, 16

					psrad			mm0, FIRST_SHIFT

					punpckldq	mm0, mm0

					packssdw		mm0, mm0

					movq	[eax - 16], mm0

					movq	[eax - 8], mm0

					sub	ecx, 1
					jne	loop11
					}
				}
			break;
		case 2:
			__asm
				{
				mov	esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]
loop12:
				movq			mm1, [eax+8]

				punpcklwd	mm1, [eax+0]

				punpckldq	mm1, mm1

				movq			mm4, mm1

				pmaddwd		mm1, [esi].tpC8pC7pC8pC5
				movq			mm2, mm4

				pmaddwd		mm2, [esi].tpC8pC3pC8pC1
				movq			mm3, mm4

				pmaddwd		mm3, [esi].tpC8mC1pC8mC3

				pmaddwd		mm4, [esi].tpC8mC5pC8mC7
				psrad			mm1, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT
				add			eax, 16

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				packssdw		mm2, mm1

				packssdw		mm4, mm3

				movq	[eax - 16], mm2

				movq	[eax - 8], mm4

				sub	ecx, 1
				jne	loop12
				}
			break;
		case 3:
			__asm
				{
				mov	esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]
loop13:
				movq			mm0, [eax+0]

				movq			mm4, [eax+8]
				movq			mm2, mm0

				pmaddwd		mm0, [esi].tpC8000000000
				psrlq			mm2, 32

				punpcklwd	mm4, mm2
				add			eax, 16

				punpckldq	mm4, mm4

				movq			mm1, mm4
				punpckldq	mm0, mm0

				pmaddwd		mm1, [esi].tmC2pC7mC6pC5
				movq			mm2, mm4

				pmaddwd		mm2, [esi].tpC6pC3pC2pC1
				movq			mm3, mm4

				pmaddwd		mm3, [esi].tpC2mC1pC6mC3
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				psrad			mm1, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				packssdw		mm0, mm0

				packssdw		mm2, mm1

				packssdw		mm4, mm3
				paddsw		mm2, mm0

				paddsw		mm4, mm0

				movq	[eax - 16], mm2

				movq	[eax - 8], mm4

				sub	ecx, 1
				jne	loop13
				}
			break;
		case 4:
			__asm
				{
				mov	esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]

loop14:
				movq			mm0, [eax+0]	// 00 f2 00 f0

				movq			mm1, [eax+8]	// 00 f3 00 f1
				psllq			mm0, 32

				punpckhwd	mm0, [eax+0]

				psllq			mm1, 32

				punpckhwd	mm1, [eax+8]

				punpckldq	mm0, mm0			// f2 f0 f2 f0

				punpckldq	mm1, mm1			// f3 f1 f3 f1
				movq			mm2, mm0

				pmaddwd		mm0, [esi].tpC6pC8pC2pC8
				movq			mm3, mm1

				pmaddwd		mm2, [esi].tmC2pC8mC6pC8

				pmaddwd		mm1, [esi].tmC7pC3pC3pC1

				pmaddwd		mm3, [esi].tmC5pC7mC1pC5
				movq			mm4, mm0

				movq			mm5, mm2
				paddd			mm0, mm1

				psrad			mm0, FIRST_SHIFT
				paddd			mm2, mm3

				psrad			mm2, FIRST_SHIFT
				psubd			mm4, mm1

				psrad			mm4, FIRST_SHIFT
				psubd			mm5, mm3

				psrad			mm5, FIRST_SHIFT

				packssdw		mm5, mm4
				add			eax, 16

				movq			mm4, mm5
				packssdw		mm0, mm2

				pslld			mm5, 16

				psrld			mm4, 16

				por			mm4, mm5

				movq			[eax-16], mm0

				movq			[eax-8], mm4

				sub			ecx, 1
				jne			loop14
				}
			break;
		default:
			__asm
				{
				mov	esi, [table]
				lea			eax, [tmmxf]
				mov			ecx, [numLines]
loop18a:
				movq			mm0, [eax+0]

				movq			mm1, [eax+0]

				punpckldq	mm0, mm0					// f4 f0 f4 f0

				pmaddwd		mm0, [esi].tmC8pC8pC8pC8
				punpckhdq	mm1, mm1					// f6 f2 f6 f2

				pmaddwd		mm1, [esi].tmC2pC6pC6pC2

				movq			mm2, mm0
				paddd			mm0, mm1					// a1 a0
				psubd			mm2, mm1					// a3 a2

				punpckldq	mm1, mm2
				punpckhdq	mm2, mm1					// a2 a3

				movq			mm1, [eax+8]			// f7 f3 f5 f1

				movq			mm3, [eax+8]

				punpckldq	mm1, mm1					// f5 f1 f5 f1
				punpckhdq	mm3, mm3					// f7 f3 f7 f3

				movq			mm4, [esi].tmC1pC3pC5pC1
				pmaddwd		mm4, mm1					// y6a y4a
				pmaddwd		mm1, [esi].tpC3pC7pC7pC5	// y5a y7a

				movq			mm5, [esi].tmC5mC7pC7pC3
				pmaddwd		mm5, mm3					// y6b y4b
				pmaddwd		mm3, [esi].tmC1mC5pC3mC1	// y5b y7b

				paddd			mm4, mm5
				paddd			mm1, mm3

				movq			mm3, mm0
				movq			mm5, mm2

				paddd			mm0, mm4					// t1 t0
				paddd			mm2, mm1					// t2 t3

				psrad			mm0, FIRST_SHIFT
				psrad			mm2, FIRST_SHIFT

				psubd			mm3, mm4					// t6 t7
				psubd			mm5, mm1					// t4 t5

				psrad			mm3, FIRST_SHIFT
				psrad			mm5, FIRST_SHIFT

				packssdw		mm0, mm2					// t3 t2 t1 t0
				packssdw		mm5, mm3					// t6 t7 t4 t5

				movq			mm3, mm5
				pslld			mm5, 16

				movq			[eax], mm0
				psrld			mm3, 16
				por			mm3, mm5					// t4 t5 t6 t7

				movq			[eax+8], mm3

				add			eax, 16
				sub			ecx, 1
				jne			loop18a
				}
			break;
		}

	switch (numLines)
		{
		case 1:
			__asm
				{
				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, [RoundSingle0]
				pxor		mm7, mm7
				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, [RoundSingle1]
				psraw		mm0, 7
				psraw		mm1, 7
				movq		[tmmxf + 0 * 16 + 0], mm7
				mov		ebx, [mb]
				mov		edx, [stride]
				movq		[tmmxf + 0 * 16 + 8], mm7
				lea		ecx, [ebx+4*edx]
				packuswb	mm0, mm1

				movq		[ebx], mm0
				lea		esi, [edx+2*edx]
				movq		[ebx+edx], mm0
				movq		[ebx+2*edx], mm0
				movq		[ebx+esi], mm0
				movq		[ecx], mm0
				movq		[ecx+edx], mm0
				movq		[ecx+2*edx], mm0
				movq		[ecx+esi], mm0
				}
			break;
		case 2:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7


				//////////////////////

				movq		mm3, [pC1pC1pC1pC1]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx], mm4
				movq		[ecx+esi], mm5


				movq		mm3, [pC3pC3pC3pC3]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+edx], mm4
				movq		[ecx+2*edx], mm5


				movq		mm3, [pC5pC5pC5pC5]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+2*edx], mm4
				movq		[ecx+edx], mm5


				movq		mm3, [pC7pC7pC7pC7]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+esi], mm4
				movq		[ecx], mm5

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}

			break;
		case 3:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				movq		mm3, [pC2pC2pC2pC2]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]
				movq		mm4, mm0
				movq		mm5, mm1

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC1pC1pC1pC1]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, 5
				psraw		mm1, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0
				movq		[ecx+esi], mm6

				//////////

				movq		mm3, [pC7pC7pC7pC7]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx+esi], mm4
				movq		[ecx], mm6

				////////////////////////////

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				movq		mm3, [pC6pC6pC6pC6]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]
				movq		mm4, mm0
				movq		mm5, mm1

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 2 * 16 + 8], mm7

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC3pC3pC3pC3]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, 5
				psraw		mm1, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx+edx], mm0
				movq		[ecx+2*edx], mm6

				//////////

				movq		mm3, [pC5pC5pC5pC5]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx+2*edx], mm4
				movq		[ecx+edx], mm6

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}
			break;
		case 4:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC2pC2pC2pC2]
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC2pC2pC2pC2]

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0			// r0
				movq		[ecx + esi], mm6	// r7

				//////

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + esi], mm4			// r0
				movq		[ecx], mm6	// r7

				//////////////////////////////

				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 8], mm7

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC6pC6pC6pC6]

				movq		[tmmxf + 2 * 16 + 0], mm7

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC6pC6pC6pC6]

				movq		[tmmxf + 2 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx + edx], mm0			// r1
				movq		[ecx + 2 * edx], mm6	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ecx + edx], mm6	// r5
				}
			break;
		default:

			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, [tmmxf + 4 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, [tmmxf + 4 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC2pC2pC2pC2]
				movq		mm3, [tmmxf + 6 * 16 + 0]
				pmulhw	mm3, [pC6pC6pC6pC6]
				paddsw	mm2, mm3 // y2 0

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC2pC2pC2pC2]
				movq		mm4, [tmmxf + 6 * 16 + 8]
				pmulhw	mm4, [pC6pC6pC6pC6]
				paddsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0			// r0
				movq		[ecx + esi], mm6	// r7

				//////

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + esi], mm4			// r0
				movq		[ecx], mm6	// r7

				//////////////////////////////

				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psubsw	mm0, [tmmxf + 4 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 4 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				psubsw	mm1, [tmmxf + 4 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 4 * 16 + 8], mm7

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC6pC6pC6pC6]
				movq		mm3, [tmmxf + 6 * 16 + 0]
				pmulhw	mm3, [pC2pC2pC2pC2]
				psubsw	mm2, mm3 // y2 0

				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 6 * 16 + 0], mm7

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC6pC6pC6pC6]
				movq		mm4, [tmmxf + 6 * 16 + 8]
				pmulhw	mm4, [pC2pC2pC2pC2]
				psubsw	mm3, mm4 // y2 1

				movq		[tmmxf + 2 * 16 + 8], mm7
				movq		[tmmxf + 6 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx + edx], mm0			// r1
				movq		[ecx + 2 * edx], mm6	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7
				movq		[tmmxf + 5 * 16 + 0], mm7
				movq		[tmmxf + 7 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7
				movq		[tmmxf + 5 * 16 + 8], mm7
				movq		[tmmxf + 7 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ecx + edx], mm6	// r5
				}
		}
	}



int GenericMMXMacroBlockIDCTDecoder::ParseIntraBlockMatrix(int yuvType, int & dcPredictor, int * matrix)
	{
	int dctPred;
	unsigned long * table;
	int l;
	static MMXInt<2> lmask = {0x0000fff0, 0x00000000};
	static MMXInt<2> l2    = {(7 << 25) | (1 << 17), 0x00000000};
	VideoCodingStandard	vcs = this->vcs;

	dctPred = dcPredictor + bitStream->GetDCCoefficient(splitIntraDCTable[yuvType]);
	dcPredictor = dctPred;

	tmmxf[0][0] = (dctPred << (7 - DCT_PREC)) >> intraDCPrecision;

	table = intraNonDCTable->extDCTTable;

#if MEASURE_PARSE_TIME
	static __int64 dctSumTime;
	static int dctCount;
	__int64 t3, t4;

	ReadPerformanceCounter(t3);
#endif

	__asm
		{
		mov	ecx, [BSBits]
		mov	edi, [matrix]

		movq	mm2, [lmask]

		mov	edx, VIDEO_STREAM_BUFFER_MASK

		movq	mm3, [l2]

		and	edx, ecx
		//

		mov	esi, [table]
		shr	edx, 5
		//

		//
		//
loop1:
		mov	ebx, [VideoStreamBuffer +     edx * 4]
		mov	eax, [VideoStreamBuffer + 4 + edx * 4]

		shld	ebx, eax, cl

		mov	eax, ebx
		shr	ebx, 23
		//

		mov	ebx, [esi + 4 * ebx]
		mov	edx, 0x0000000f

		and	edx, ebx
		je		notSingle
single:
		movd	mm0, ebx
		shr	ebx, 16
		add	ecx, ebx

		movd	mm1, [edi + 4 * edx]
		lea	edi, [edi + 4 * edx]
		mov	edx, VIDEO_STREAM_BUFFER_MASK

		movzx	ebx, [edi + 3]
		and	edx, ecx
		shr	edx, 5

		mov	eax, [VideoStreamBuffer + 4 + edx * 4]
		pand	mm0, mm2

		mov	edx, [VideoStreamBuffer +     edx * 4]
		pmaddwd	mm0, mm1

		shld	edx, eax, cl
		psubusb	mm3, mm1
		paddb	mm3, mm1

		movd	eax, mm0
		add	eax, 0x00000001 << (DCT_PREC + 3)
		sar	eax, DCT_PREC + 4

		mov	WORD PTR [tmmxf+ebx], ax

		mov	eax, edx
		shr	edx, 23

		mov	ebx, [esi + 4 * edx]
		mov	edx, 0x0000000f
		and	edx, ebx

		jne	single
notSingle:
		cmp	eax, 0x08000000
		jnc	done

		cmp	eax, 0x04000000
		jnc	escape

		mov	ebx, 0x0000000f

		test	eax, 0x03800000
		je		longmulti

		shr	eax, 18

		mov	edx, [esi + 4 * eax + 0x800]

		mov	eax, edx
		and	ebx, edx

		shr	edx, 16
		jmp	final

longmulti:
		shr	eax, 15

		mov	edx, [esi + 4 * eax + 0xc00]

		mov	eax, edx
		and	ebx, edx

		shr	edx, 16
		jmp	final
escape:
		cmp	[vcs], VCS_MPEG_1
		jne	mpeg2escape

		mov	edx, eax
		shr	edx, 20
		and	edx, 0x0000003f
		add	edx, 1

		test	eax, 0x0007f000
		jne	mpeg1single

		mov	ebx, eax
		shl	ebx, 12
		and	ebx, 0x80000000
		sar	ebx, 19
		or		eax, ebx
		and	eax, 0x0000fff0

		mov	ebx, 20
		jmp	final
mpeg1single:
		shl	eax, 12

		sar	eax, 20
		and	eax, 0x0000fff0

		mov	ebx, 12
		jmp	final

mpeg2escape:
		mov	edx, eax

		shr	edx, 20

		shr	eax, 4

		and	edx, 0x0000003f

		mov	ebx, 16
		add	edx, 1
final:
		movd	mm0, eax

		movd	mm1, [edi + 4 * edx]
		pand	mm0, mm2

		pmaddwd	mm0, mm1

		lea	ecx, [ecx + ebx + 8]
		movzx	ebx, BYTE PTR [edi + 4 * edx + 3]

		lea	edi, [edi + 4 * edx]
		mov	edx, VIDEO_STREAM_BUFFER_MASK

		and	edx, ecx

		movd	eax, mm0

		shr	edx, 5
		add	eax, 0x00000001 << (DCT_PREC + 3)

		sar	eax, DCT_PREC + 4
		psubusb	mm3, mm1

		mov	WORD PTR [tmmxf+ebx], ax
		paddb		mm3, mm1

		jmp	loop1
done:
		shr	ebx, 16

		add	ecx, ebx

		mov	[BSBits], ecx

		movd	[l], mm3
		}

#if MEASURE_PARSE_TIME
		ReadPerformanceCounter(t4);

		dctSumTime += t4 - t3;
		dctCount++;

		if (!(dctCount & 65535))
			{
			char buffer[100];
			wsprintf(buffer, "PARSE %8d : %d.%02d\n", dctCount, (int)(dctSumTime / dctCount), (int)(100 * dctSumTime / dctCount % 100));
			OutputDebugString(buffer);
			}
#endif

#if DUMP_COEFFICIENTS
		{
		if (!dfile)
			{
			dfile = fopen("c:\\coeffdump.txt", "w");
			}

		fprintf(dfile, "INTRA %d\n", blockcount);
		for(int i=0; i<8; i++)
			{
			for(int j=0; j<8; j++)
				{
				fprintf(dfile, "%5d ", tmmxf[i][j]);
				}
			fprintf(dfile, "\n");
			}
		fprintf(dfile, "\n");
		}

#endif

	return l;
	}

int GenericMMXMacroBlockIDCTDecoder::ParseNonIntraBlockMatrix(int * matrix)
	{
	int l;
	static MMXInt<2> lmask = {0x0000fff0, 0x00000000};
	static MMXInt<2> tzmask = {0x000000ff, 0x00000000};
	static MMXInt<2> tzmaskh = {0x000001ff, 0x00000000};
	static MMXInt<2> lround = {0x00000001 << (DCT_PREC + 3 + 1), 0x00000000};
	VideoCodingStandard	vcs = this->vcs;

	static int l2 = (7 << 25) | (1 << 17);

	__asm
		{
		mov	ecx, [BSBits]
		mov	edx, VIDEO_STREAM_BUFFER_MASK

		mov	edi, [matrix]
		and	edx, ecx

		shr	edx, 5
		sub	edi, 4

		movq	mm2, [lmask]
		movq	mm4, [tzmaskh]
		//

		movd	mm3, [l2]
		//

		mov	ebx, [VideoStreamBuffer +     edx * 4]
		mov	eax, [VideoStreamBuffer + 4 + edx * 4]

		shld	ebx, eax, cl

		test	ebx, ebx
		jns	notInitial

		mov	eax, ebx
		add	ebx, ebx

		sar	ebx, 31
		mov	edx, [edi+4]

		shl	eax, 2
		add	edx, edx

		add	edx, [edi+4]
		add	ecx, 2

		and	edx, 0x0000ffff

		shr	edx, 1 + DCT_PREC
		add	edi, 4

		xor	edx, ebx

		sub	edx, ebx
		mov	ebx, eax

		shr	ebx, 23
		and	edx, 0x0000ffff

		mov	DWORD PTR [tmmxf], edx
		mov	edx, ecx
		mov	ecx, 0x0000000f

		mov	esi, [dctCoefficientZero + 4 * ebx].extNIDCTTable
		xor	ebx, ebx

		and	ecx, esi
		movd	mm0, esi
		jne	single

		jmp	notSingle

		align	16
		nop
		nop
notInitial:
		mov	eax, ebx
		shr	ebx, 23
		mov	esi, [dctCoefficientZero + 4 * ebx].extNIDCTTable

		mov	edx, ecx
		mov	ecx, 0x0000000f
		and	ecx, esi

		movd	mm0, esi
		je		notSingle

single:
		shr		esi, 16
		movd		mm1, [edi + 4 * esi]
		//

		shl		eax, cl
		mov		ebx, eax
		shr		eax, 23


		movq		mm6, mm0
		pand		mm0, mm2
		pmaddwd	mm0, mm1
		//

		add		edx, ecx
		mov		ecx, [dctCoefficientZero + 4 * eax].extNIDCTTable
		psraw		mm6, 15

		lea		edi, [edi + 4 * esi]
		movzx		esi, [edi + 3]

		psrlw		mm6, 7
		paddd		mm0, mm6
		movd		eax, mm0

		psubusb	mm1, mm3
		movd		mm0, ecx

		sar		eax, DCT_PREC + 4 + 1

		and		ecx, 0x0000000f
		paddb		mm3, mm1

		mov		WORD PTR [tmmxf+esi], ax

		movd		esi, mm0
		je			multiSingle2

		shr		esi, 16
		movd		mm1, [edi + 4 * esi]
		//

		shl		ebx, cl
		mov		eax, ebx
		shr		ebx, 23

		movq		mm6, mm0
		pand		mm0, mm2
		pmaddwd	mm0, mm1

		add		edx, ecx
		mov		ecx, [dctCoefficientZero + 4 * ebx].extNIDCTTable
		psraw		mm6, 15

		//

		lea		edi, [edi + 4 * esi]
		movzx		esi, [edi + 3]
		psrlw		mm6, 7

		paddd		mm0, mm6
		movd		ebx, mm0

		sar		ebx, DCT_PREC + 4 + 1

		psubusb	mm1, mm3
		movd		mm0, ecx

		and		ecx, 0x0000000f
		paddb		mm3, mm1

		mov		WORD PTR [tmmxf+esi], bx

		movd		esi, mm0
		je			multiSingle

		shr		esi, 16
		movd		mm1, [edi + 4 * esi]

		add		edx, ecx
		mov		ecx, edx
		and		edx, VIDEO_STREAM_BUFFER_MASK

		shr		edx, 5
		lea		edi, [edi + 4 * esi]

		mov		ebx, [VideoStreamBuffer + 4 + edx * 4]

		movq		mm6, mm0

		pand		mm0, mm2

		mov		esi, [VideoStreamBuffer +     edx * 4]
		pmaddwd	mm0, mm1
		psubusb	mm1, mm3

		movzx		edx, [edi + 3]
		paddb		mm3, mm1

		shld		esi, ebx, cl

		psraw		mm6, 15
		pand		mm6, mm4
		paddd		mm0, mm6

		movd		ebx, mm0

		sar		ebx, DCT_PREC + 4 + 1
		mov		eax, esi

		mov		BYTE PTR [tmmxf+edx], bl
		shr		esi, 23

		mov		BYTE PTR [tmmxf+edx+1], bh
		mov		esi, [dctCoefficientZero + 4 * esi].extNIDCTTable
		mov		edx, ecx
		mov		ecx, 0x0000000f

		movd		mm0, esi
		and		ecx, esi

		jne		single
notSingle:
		mov	ecx, edx

		test	eax, eax
		js		done

		cmp	eax, 0x04000000
		jnc	escape
		test	eax, 0x03800000
		je		longmulti
notLongMulti:
		shr	eax, 18

		mov	esi, [dctCoefficientZero + 4 * eax + 0x800].extNIDCTTable
		mov	edx, 0x0000000f

		mov	eax, esi
		and	edx, esi

		shr	esi, 16
		jmp	final

		align	16
multiSingle:
		mov	ebx, eax
multiSingle2:
		mov	ecx, edx
		test	ebx, ebx
		js		done

		and	edx, VIDEO_STREAM_BUFFER_MASK
		shr	edx, 5
		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	eax, [VideoStreamBuffer +     edx * 4]
		shld	eax, ebx, cl

//llongmulti:
		cmp	eax, 0x04000000
		jnc	escape


		test	eax, 0x03800000
		jne	notLongMulti

longmulti:
		shr	eax, 15

		mov	esi, [dctCoefficientZero + 4 * eax + 0xc00].extNIDCTTable
		mov	edx, 0x0000000f

		mov	eax, esi
		and	edx, esi

		shr	esi, 16
		jmp	final

		align	16
mpeg1escape:

		shr	esi, 20
		and	esi, 0x0000003f
		add	esi, 1

		test	eax, 0x0007f000
		jne	mpeg1single

		add	eax, eax
		and	eax, 0x00001fe0
		shl	edx, 12
		and	edx, 0x80000000
		sar	edx, 18
		or		eax, edx

		mov	edx, eax
		sar	edx, 31
		and	edx, 0xffffffe0
		lea	eax, [eax + edx + 16]

		mov	edx, 20
		jmp	final

		align	16
mpeg1single:
		shl	eax, 12
		and	eax, 0xff000000
		sar	eax, 19

		mov	edx, eax
		sar	edx, 31
		and	edx, 0xffffffe0
		lea	eax, [eax + edx + 16]

		mov	edx, 12
		jmp	final

		align	16
escape:
		cmp	[vcs], VCS_MPEG_1
		mov	esi, eax
		mov	edx, eax
		je		mpeg1escape

		shr	esi, 20

		shr	eax, 4
		and	esi, 0x0000003f

		shr	edx, 15
		lea	edi, [edi + 4 * esi + 4]

		movd	mm1, [edi]
		and	eax, 0x0000fff0
		and	edx, 0x00000010

		movzx	ebx, BYTE PTR [edi + 3]
		add	eax, 0x00000008
		add	ecx, 24

		sub	eax, edx
		movd	mm0, eax
		pmaddwd	mm0, mm1

		mov	edx, VIDEO_STREAM_BUFFER_MASK

		and	edx, ecx

		movq	mm6, mm0
		psrad	mm6, 31
		pand	mm6, [tzmask]
		paddd	mm0, mm6

		movd	eax, mm0

		shr	edx, 5

		mov	esi, [VideoStreamBuffer + 4 + edx * 4]

		sar	eax, DCT_PREC + 3 + 1
		psubusb	mm3, mm1

		mov	BYTE PTR [tmmxf+ebx], al
		paddb		mm3, mm1

		mov	BYTE PTR [tmmxf+ebx+1], ah
		mov	ebx, [VideoStreamBuffer +     edx * 4]

		shld	ebx, esi, cl

		jmp	notInitial

		align	16
final:
		lea		edi, [edi + 4 * esi]
		movd		mm1, [edi]
		movzx		esi, [edi + 3]

		movd		mm0, eax
		movd		mm6, eax
		pand		mm0, mm2

		pmaddwd	mm0, mm1
		lea		ecx, [ecx + edx + 8]
		mov		edx, VIDEO_STREAM_BUFFER_MASK

		and		edx, ecx
		psraw		mm6, 15
		pand		mm6, mm4

		paddd		mm0, mm6
		shr		edx, 5
		mov		ebx, [VideoStreamBuffer +     edx * 4]

		movd		eax, mm0
		sar		eax, DCT_PREC + 4 + 1

		mov		edx, [VideoStreamBuffer + 4 + edx * 4]
		shld		ebx, edx, cl

		psubusb	mm3, mm1
		test		ebx, ebx

		mov		WORD PTR [tmmxf+esi], ax
		paddb		mm3, mm1
		jns		notInitial

		test		ebx, 0x40000000
		jne		notInitial
done:
		add		ecx, 2

		mov		[BSBits], ecx

		movd		[l], mm3
		}

#if DUMP_COEFFICIENTS
		{
		if (!dfile)
			{
			dfile = fopen("c:\\coeffdump.txt", "w");
			}

		fprintf(dfile, "NON INTRA %d\n", blockcount);
		for(int i=0; i<8; i++)
			{
			for(int j=0; j<8; j++)
				{
				fprintf(dfile, "%5d ", tmmxf[i][j]);
				}
			fprintf(dfile, "\n");
			}
		fprintf(dfile, "\n");
		}

#endif

	return l;
	}

GenericMMXMacroBlockIDCTDecoder::GenericMMXMacroBlockIDCTDecoder(void)
	{
	splitIntraDCTable[0] = &dctDCSizeLuminance;
	splitIntraDCTable[1] = splitIntraDCTable[2] = &dctDCSizeChrominance;
	}

GenericMMXMacroBlockIDCTDecoder::~GenericMMXMacroBlockIDCTDecoder(void)
	{
#if DUMP_COEFFICIENTS
	if (dfile)
		{
		fclose(dfile);
		dfile = NULL;
		}
#endif
	}

void GenericMMXMacroBlockIDCTDecoder::SetPictureParameters(bool hurryUp,
																			  VideoCodingStandard vcs,
																			  int intraDCPrecision,
																			  HuffmanTable * intraNonDCTable)
	{
	this->hurryUp = hurryUp;
	this->vcs = vcs;
	this->intraDCPrecision = intraDCPrecision;
	this->intraNonDCTable = intraNonDCTable;

	memclr128(tmmxf);
	}

void GenericMMXMacroBlockIDCTDecoder::ParseIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, short * mb, int stride)
	{
	int l;

	l = ParseIntraBlockMatrix(yuvType, dcPredictor, matrix);
	InverseDCT(mb, stride, dctMulTable[yuvType], hfluti[(((l >> 25) + 8) >> 3) & 0x0f], hfluti[(l >> 17) & 0x0f]);
	}

void GenericMMXMacroBlockIDCTDecoder::ParseNonIntraBlock(int num, int yuvType, int * matrix, short * mb, int stride)
	{
	int l;

	l = ParseNonIntraBlockMatrix(matrix);
	InverseDCT(mb, stride, dctMulTable[yuvType], hflut[hurryUp][(((l >> 25) + 8) >> 3) & 0x0f], hflut[hurryUp][(l >> 17) & 0x0f]);
	}

void GenericMMXMacroBlockIDCTDecoder::ParseNonIntraBlocks(int codedBlockPattern, short ** yuvp, int * yuvd, int * matrix, int * offset)
	{
	int l, i, t;
	static char tp[] = {0, 0, 0, 0, 1, 2};

	for(i=0; i<6; i++)
		{
		if ((codedBlockPattern << i) & 0x20)
			{
			t = tp[i];
			l = ParseNonIntraBlockMatrix(matrix);
			tmmxf[0][0] += (short)offset[t];
			InverseDCT(yuvp[i], yuvd[t], dctMulTable[t], hflut[hurryUp][(((l >> 25) + 8) >> 3) & 0x0f], hflut[hurryUp][(l >> 17) & 0x0f]);
			}
		}
	}

void GenericMMXMacroBlockIDCTDecoder::ParseBaseNonIntraBlocks(int codedBlockPattern, short * yuvbp, int * yuvbo, int * yuvd, int * matrix)
	{
	int l, i, t;
	static char tp[] = {0, 0, 0, 0, 1, 2};

	for(i=0; i<6; i++)
		{
		if ((codedBlockPattern << i) & 0x20)
			{
			t = tp[i];
			l = ParseNonIntraBlockMatrix(matrix);
			InverseDCT(yuvbp + yuvbo[i], 2 * yuvd[t], dctMulTable[t], hflut[hurryUp][(((l >> 25) + 8) >> 3) & 0x0f], hflut[hurryUp][(l >> 17) & 0x0f]);
			}
		}
	}

void GenericMMXMacroBlockIDCTDecoder::ParseBYTEIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, BYTE * mb, int stride)
	{
	int l;

	l = ParseIntraBlockMatrix(yuvType, dcPredictor, matrix);
	InverseBYTEIntraDCT(mb, stride, dctMulTable[yuvType], hfluti[(((l >> 25) + 8) >> 3) & 0x0f], hfluti[(l >> 17) & 0x0f]);
	}




////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// AMD-ATHLON INTEL-KATMAI SUPPORT /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

// optimized 9/3/1999
void GenericXMMXMacroBlockIDCTDecoder::InverseDCT(short * mb, int stride, DCTMulTable * table, int numLines, int numColumns)
	{
	static MMXQWORD RoundOne0    = {ROUND_ONE, ROUND_ONE, ROUND_ONE, ROUND_ONE};
	static MMXQWORD RoundSingle0 = {ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE};

	static MMXQWORD RoundOne1    = {ROUND_ONE, ROUND_ONE, ROUND_ONE, ROUND_ONE};
	static MMXQWORD RoundSingle1 = {ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE, ROUND_SINGLE};

	static MMXQWORD RoundSecond0 = {ROUND_SECOND, ROUND_SECOND, ROUND_SECOND, ROUND_SECOND};
	static MMXQWORD RoundSecond1 = {ROUND_SECOND, ROUND_SECOND, ROUND_SECOND, ROUND_SECOND};

	static MMXQWORD RoundSecond0a = {ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4};
	static MMXQWORD RoundSecond1a = {ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4, ROUND_SECOND * 4};

	switch (numColumns)
		{
		case 1:
			if (numLines == 1)
				{
				__asm
					{
					mov			esi, [table]
					mov			edx, [stride]

					movq			mm0, [tmmxf]				// tm3 tm2 tm1 tm0

					// XMMX
					pshufw		(mm0, mm0, 0x00)
				//	punpcklwd	mm0, mm0						// tm1 tm1 tm0 tm0
				//	punpckldq	mm0, mm0						// tm0 tm0 tm0 tm0

					pxor			mm2, mm2

					movq			mm1, [RoundOne1]

					mov	ebx, [mb]

					pmulhw		mm0, [esi].tpC8pC8pC8pC8

					lea			eax, [ebx+2*edx]

					lea			ecx, [ebx+2*edx]

					lea			eax, [eax+edx]

					paddw			mm1, mm0
					paddw			mm0, [RoundOne0]

					movq			[tmmxf], mm2
					psraw			mm0, ONE_SHIFT

					movq			[ebx], mm0
					movq			[ebx+8], mm0

					psraw			mm1, ONE_SHIFT

					movq			[ebx+2*edx], mm0
  					movq			[ebx+2*edx+8], mm0

					movq			[ebx+4*edx], mm0
					movq			[ebx+4*edx+8], mm0

					movq			[ecx+4*edx], mm0
					movq			[ecx+4*edx+8], mm0

					movq			[ebx+edx], mm1
					movq			[ebx+edx+8], mm1

					movq			[eax], mm1
					movq			[eax+8], mm1

					movq			[eax+2*edx], mm1
					movq			[eax+2*edx+8], mm1

					movq			[eax+4*edx], mm1
					movq			[eax+4*edx+8], mm1
					}

				return;
				}
			else
				{
				__asm
					{
					lea			eax, tmmxf
					mov			esi, [table]

					mov			ecx, [numLines]

					movq			mm1, [eax+0]

					pmaddwd		mm1, [esi].tpC8000000000

					sub			ecx, 1
loop11:
					movq			mm0, [eax+16]
					psrad			mm1, FIRST_SHIFT

					pmaddwd		mm0, [esi].tpC8000000000
					punpckldq	mm1, mm1

					packssdw		mm1, mm1
					sub			ecx, 1

					movq			[eax], mm1

					movq			[eax + 8], mm1
					movq			mm1, mm0

					lea			eax, [eax + 16]
					jne			loop11

					psrad			mm1, FIRST_SHIFT

					punpckldq	mm1, mm1

					packssdw		mm1, mm1

					movq			[eax], mm1

					movq			[eax + 8], mm1
					}
				}
			break;
		case 2:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]

				movq			mm1, [eax+8]

				punpcklwd	mm1, [eax+0]

				movq			mm4, [esi].tpC8pC7pC8pC5
				punpckldq	mm1, mm1

				movq			mm2, [esi].tpC8pC3pC8pC1
				pmaddwd		mm4, mm1

				movq			mm3, [esi].tpC8mC1pC8mC3
				pmaddwd		mm2, mm1

				movq			mm5, mm1
				pmaddwd		mm3, mm1

				pmaddwd		mm5, [esi].tpC8mC5pC8mC7
				psrad			mm4, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				je				done12
loop12:
				movq			mm1, [eax+24]
				psrad			mm5, FIRST_SHIFT

				punpcklwd	mm1, [eax+16]
				packssdw		mm2, mm4

				movq			mm4, [esi].tpC8pC7pC8pC5
				punpckldq	mm1, mm1

				movq			[eax], mm2
				packssdw		mm5, mm3

				movq			mm2, [esi].tpC8pC3pC8pC1
				pmaddwd		mm4, mm1

				movq			mm3, [esi].tpC8mC1pC8mC3
				pmaddwd		mm2, mm1

				movq			[eax + 8], mm5
				movq			mm5, mm1

				pmaddwd		mm3, mm1
				psrad			mm4, FIRST_SHIFT

				pmaddwd		mm5, [esi].tpC8mC5pC8mC7
				psrad			mm2, FIRST_SHIFT

				lea			eax, [eax+16]
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				jne			loop12
done12:
				psrad			mm5, FIRST_SHIFT

				packssdw		mm2, mm4

				packssdw		mm5, mm3

				movq			[eax], mm2

				movq			[eax + 8], mm5
				}
			break;
		case 3:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]

				movq			mm0, [eax+0]					// tm3 tm2 tm1 tm0

				// XMMX
				pshufw		(mm2, mm0, 0xEE)				// tm3 tm2 tm3 tm2
			//	movq			mm2, mm0							// tm3 tm2 tm1 tm0
			//	psrlq			mm2, 32							//   0   0 tm3 tm2

				movq			mm4, [eax+8]					// tm7 tm6 tm5 tm4
				punpcklwd	mm4, mm2							// tm3 tm5 tm2 tm4
				punpckldq	mm4, mm4						   // tm2 tm4 tm2 tm4

				pmaddwd		mm0, [esi].tpC8000000000


				movq			mm1, [esi].tmC2pC7mC6pC5

				movq			mm2, [esi].tpC6pC3pC2pC1
				pmaddwd		mm1, mm4

				movq			mm3, [esi].tpC2mC1pC6mC3
				punpckldq	mm0, mm0							//

				pmaddwd		mm2, mm4
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm3, mm4
				psrad			mm1, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				packssdw		mm0, mm0

				psrad			mm2, FIRST_SHIFT

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				sub			ecx, 1
				je				done13
loop13:
				movq			mm5, [eax+16]				   // tm3 tm2 tm1 tm0
				packssdw		mm2, mm1

				movq			mm1, [eax+24]					// tm7 tm6 tm5 tm4
				paddsw		mm2, mm0

				movq			[eax], mm2

				// XMMX
				pshufw		(mm2, mm5, 0xEE)				// tm3 tm2 tm3 tm2
			//	movq			mm2, mm5							// tm3 tm2 tm1 tm0
			//	psrlq			mm2, 32							//   0   0 tm3 tm2

				pmaddwd		mm5, [esi].tpC8000000000
				packssdw		mm4, mm3

				paddsw		mm4, mm0

				movq			[eax + 8], mm4

				punpcklwd	mm1, mm2							//	tm3 tm5 tm2 tm4
				punpckldq	mm1, mm1							//	tm2 tm4 tm2 tm4

				movq			mm0, mm5

				movq			mm2, [esi].tpC6pC3pC2pC1
				movq			mm4, mm1

				pmaddwd		mm1, [esi].tmC2pC7mC6pC5

				movq			mm3, [esi].tpC2mC1pC6mC3
				punpckldq	mm0, mm0

				pmaddwd		mm2, mm4
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm3, mm4
				psrad			mm1, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				packssdw		mm0, mm0

				psrad			mm2, FIRST_SHIFT
				sub			ecx, 1

				psrad			mm3, FIRST_SHIFT
				lea			eax, [eax+16]

				psrad			mm4, FIRST_SHIFT
				jne			loop13
done13:
				packssdw		mm2, mm1

				packssdw		mm4, mm3
				paddsw		mm2, mm0

				paddsw		mm4, mm0

				movq	[eax], mm2

				movq	[eax + 8], mm4

				}
			break;
		case 4:
			__asm
				{
				mov			esi, [table]

				lea			eax, tmmxf
				mov			ecx, [numLines]

loop14:

				//
				pshufwMEM	(mm0, eax, 0x99)					// 4
				pshufwMEM	(mm7, eax, 0x44)					// 4
				por			mm0, mm7								// 3

				//
				pshufwIMM8	(mm1, eax, 8, 0x99)				// 5

				//
				pshufwIMM8	(mm6, eax, 8, 0x44)				// 5
				por			mm1, mm6								// 3
				movq			mm2, [esi].tpC6pC8pC2pC8		// 4

				//
				movq			mm3, [esi].tmC7pC3pC3pC1		// 4

				//
				pmaddwd		mm2, mm0								// 3
				pmaddwd		mm0, [esi].tmC2pC8mC6pC8		// 4
				pmaddwd		mm3, mm1								// 3

				//
				pmaddwd		mm1, [esi].tmC5pC7mC1pC5		// 4
				movq			mm4, mm2								// 3

				movq			mm5, mm0								// 3
				paddd			mm2, mm3								// 3

				psrad			mm2, FIRST_SHIFT					// 4
				paddd			mm0, mm1								// 3

				psrad			mm0, FIRST_SHIFT					// 4
				psubd			mm4, mm3								// 3

				psrad			mm4, FIRST_SHIFT					// 4
				psubd			mm5, mm1								// 3

				psrad			mm5, FIRST_SHIFT					// 4

				packssdw		mm5, mm4								// 3
				lea			eax, [eax+16]
			//	add			eax, 16								// 3

				movq			mm4, mm5								// 3
				packssdw		mm2, mm0								// 3

				pslld			mm5, 16								// 4

				psrld			mm4, 16								// 4

				por			mm4, mm5								// 3

				movq			[eax-16], mm2

				movq			[eax-8], mm4

				sub			ecx, 1
				jne			loop14
				}
		break;
		default:
			__asm
				{
				mov			esi, [table]

				lea			eax, [tmmxf]
				mov			ecx, [numLines]
loop18a:

				// XMMX
				pshufwMEM	(mm0, eax, 0x44)		// tm1 tm0 tm1 tm0
			//	movq			mm0, [eax+0]
			//	punpckldq	mm0, mm0					// f4 f0 f4 f0

				// XMMX
				pshufwMEM	(mm1, eax, 0xEE)		// tm3 tm2 tm3 tm2
			//	movq			mm1, [eax+0]
			//	punpckhdq	mm1, mm1					// f6 f2 f6 f2

				// XMMX
				pshufwIMM8	(mm6, eax, 8, 0x44)
			//	movq			mm6, [eax+8]			// f7 f3 f5 f1
			//	punpckldq	mm6, mm6					// f5 f1 f5 f1

				// XMMX
				pshufwIMM8	(mm7, eax, 8, 0xEE)
			//	movq			mm7, [eax+8]
			//	punpckhdq	mm7, mm7					// f7 f3 f7 f3

				pmaddwd		mm0, [esi].tmC8pC8pC8pC8

				pmaddwd		mm1, [esi].tmC2pC6pC6pC2
				movq			mm2, mm0

				movq			mm4, [esi].tmC1pC3pC5pC1
				lea			eax, [eax + 16]

				pmaddwd		mm4, mm6					// y6a y4a

				pmaddwd		mm6, [esi].tpC3pC7pC7pC5	// y5a y7a
				paddd			mm0, mm1					// a1 a0
				psubd			mm2, mm1					// a3 a2

				movq			mm5, [esi].tmC5mC7pC7pC3
				pmaddwd		mm5, mm7					// y6b y4b

				// XMMX
				pshufw		(mm2, mm2, 0x4E)
			//	punpckldq	mm1, mm2
			//	punpckhdq	mm2, mm1					// a2 a3

				pmaddwd		mm7, [esi].tmC1mC5pC3mC1	// y5b y7b

				paddd			mm4, mm5
				paddd			mm6, mm7

				movq			mm7, mm0
				movq			mm5, mm2

				paddd			mm0, mm4					// t1 t0
				paddd			mm2, mm6					// t2 t3

				psrad			mm0, FIRST_SHIFT
				psrad			mm2, FIRST_SHIFT

				psubd			mm7, mm4					// t6 t7
				psubd			mm5, mm6					// t4 t5

				psrad			mm7, FIRST_SHIFT
				psrad			mm5, FIRST_SHIFT

				packssdw		mm0, mm2					// t3 t2 t1 t0
				packssdw		mm5, mm7					// t6 t7 t4 t5

				movq			mm7, mm5
				pslld			mm5, 16

				movq			[eax-16], mm0

				psrld			mm7, 16
				por			mm5, mm7					// t4 t5 t6 t7

				movq			[eax-8], mm5
				dec			ecx

				jne			loop18a
				}
			break;
		}

	switch (numLines)
		{
		case 1:
			__asm
				{
				movq		mm2, [RoundSingle0]
				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm2
				psraw		mm0, INTER_PREC

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm2
				psraw		mm1, INTER_PREC

				movq		[tmmxf + 0 * 16 + 0], mm7
				mov		ebx, [mb]
				mov		edx, [stride]

				movq		[tmmxf + 0 * 16 + 8], mm7
				lea		ecx, [ebx+4*edx]
				lea		esi, [ebx+2* edx]
				lea		edi, [ecx+2*edx]

				movq		[ebx+edx], mm0
				movq		[ebx+edx+8], mm1
				movq		[ecx+edx], mm0
				movq		[ecx+edx+8], mm1
				movq		[ebx], mm0
				movq		[ebx+8], mm1
				movq		[ecx], mm0
				movq		[ecx+8], mm1
				movq		[esi], mm0
				movq		[esi+8], mm1
				movq		[edi], mm0
				movq		[edi+8], mm1
				add		esi, edx
				movq		[esi], mm0
				add		edi, edx
				movq		[esi+8], mm1
				movq		[edi], mm0
				movq		[edi+8], mm1
				}
			break;
		case 2:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7


				//////////////////////

				movq		mm3, [pC1pC1pC1pC1a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm4
				movq		[ebx+8], mm6
				movq		[ecx+esi], mm5
				movq		[ecx+esi+8], mm7


				movq		mm3, [pC3pC3pC3pC3a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+edx], mm4
				movq		[ebx+edx+8], mm6
				movq		[ecx+2*edx], mm5
				movq		[ecx+2*edx+8], mm7


				movq		mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				paddsw	mm2, mm2
				paddsw	mm3, mm3

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+2*edx], mm4
				movq		[ebx+2*edx+8], mm6
				movq		[ecx+edx], mm5
				movq		[ecx+edx+8], mm7

				movq		mm3, [pC7pC7pC7pC7a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+esi], mm4
				movq		[ebx+esi+8], mm6
				movq		[ecx], mm5
				movq		[ecx+8], mm7

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}

			break;
		case 3:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]

				movq		mm3, [pC2pC2pC2pC2a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, [tmmxf + 2 * 16 + 0]

				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm3, [tmmxf + 2 * 16 + 8]

				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]

				movq		mm4, mm0
				movq		mm5, mm1

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC1pC1pC1pC1a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0
				movq		[ebx+8], mm1
				movq		[ecx+esi], mm6
				movq		[ecx+esi+8], mm7

				//////////

				movq		mm3, [pC7pC7pC7pC7a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+esi], mm4
				movq		[ebx+esi+8], mm5
				movq		[ecx], mm6
				movq		[ecx+8], mm7

				////////////////////////////

				movq		mm0, [tmmxf + 0 * 16 + 0]
				movq		mm1, [tmmxf + 0 * 16 + 8]

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm2, mm2
				paddsw	mm3, mm3

				paddsw	mm0, [RoundSecond0a]
				paddsw	mm1, [RoundSecond0a]
				movq		mm4, mm0
				movq		mm5, mm1

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 2 * 16 + 8], mm7

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC3pC3pC3pC3a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, [tmmxf + 1 * 16 + 0]
				paddsw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+edx], mm0
				movq		[ebx+edx+8], mm1
				movq		[ecx+2*edx], mm6
				movq		[ecx+2*edx+8], mm7

				//////////

				movq		mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]
				paddsw	mm2, mm2
				paddsw	mm3, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx+2*edx], mm4
				movq		[ebx+2*edx+8], mm5
				movq		[ecx+edx], mm6
				movq		[ecx+edx+8], mm7

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}
			break;
		case 4:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm7, [RoundSecond0a]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm7

				movq		mm2, [pC2pC2pC2pC2a]
				movq		mm3, mm2

				movq		mm4, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm4
				paddsw	mm2, mm4

				movq		mm4, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, mm4
				paddsw	mm3, mm4

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm3, [tmmxf + 1 * 16 + 0]
				movq		mm2, [pC1pC1pC1pC1a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 0]
				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm3, [tmmxf + 1 * 16 + 8]
				movq		mm2, [pC1pC1pC1pC1a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 8]
				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0			// r0
				movq		[ebx+8], mm1			// r0
				movq		[ecx + esi], mm6	// r7
				movq		[ecx + esi+8], mm7	// r7

				//////

				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7a]
				paddsw	mm3, mm3
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7a]
				paddsw	mm3, mm3
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + esi], mm4			// r0
				movq		[ebx + esi+8], mm5			// r0
				movq		[ecx], mm6	// r7
				movq		[ecx+8], mm7	// r7

				//////////////////////////////

				pxor		mm7, mm7
				movq		mm6, [RoundSecond0a]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, mm6

				movq		[tmmxf + 0 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, mm6

				movq		[tmmxf + 0 * 16 + 8], mm7

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		[tmmxf + 2 * 16 + 0], mm7

				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm3, mm3

				movq		[tmmxf + 2 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm3, [tmmxf + 1 * 16 + 0]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm3, [tmmxf + 1 * 16 + 8]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + edx], mm0			// r1
				movq		[ebx + edx+8], mm1			// r1
				movq		[ecx + 2 * edx], mm6	// r6
				movq		[ecx + 2 * edx+8], mm7	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1a]
				paddsw	mm3, [tmmxf + 3 * 16 + 0]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1a]
				paddsw	mm3, [tmmxf + 3 * 16 + 8]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1
				psraw		mm4, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx], mm4			// r2
				psraw		mm5, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx+8], mm5			// r2
				psraw		mm6, SECOND_SHIFT + 2

				movq		[ecx + edx], mm6	// r5
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ecx + edx+8], mm7	// r5
				}
			break;
		default:
			__asm
				{
				movq		mm6, [RoundSecond0a]
				lea		eax, [tmmxf]

				movq		mm0, [eax + 0 * 16 + 0]
				paddsw	mm0, [eax + 4 * 16 + 0]
				paddsw	mm0, mm6

				movq		mm1, [eax + 0 * 16 + 8]
				paddsw	mm1, [eax + 4 * 16 + 8]
				paddsw	mm1, mm6

				movq		mm3, [pC6pC6pC6pC6a]
				movq		mm2, [eax + 6 * 16 + 0]
				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		mm5, [eax + 2 * 16 + 0]
				movq		mm4, [pC2pC2pC2pC2a]
				pmulhw	mm4, mm5

				paddsw	mm2, mm5
				paddsw	mm2, mm4 // y2 0

				pmulhw	mm3, [eax + 6 * 16 + 8]
				paddsw	mm3, mm3

				movq		mm5, [eax + 2 * 16 + 8]
				movq		mm4, [pC2pC2pC2pC2a]
				pmulhw	mm4, mm5

				paddsw	mm3, mm5
				paddsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]


				movq		mm2, [pC1pC1pC1pC1a]
				movq		mm6, [eax + 1 * 16 + 0]
				pmulhw	mm2, mm6
				paddsw	mm2, mm6

				movq		mm3, [eax + 3 * 16 + 0]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [pC1pC1pC1pC1a]
				movq		mm3, [eax + 1 * 16 + 8]
				pmulhw	mm2, mm3
				paddsw	mm2, mm3

				movq		mm3, [eax + 3 * 16 + 8]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx], mm0			// r0
				movq		[ebx+8], mm1			// r0
				movq		[ecx + esi], mm6	// r7
				movq		[ecx + esi+8], mm7	// r7

				//////

				movq		mm2, [eax + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7a]

				movq		mm3, [eax + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [eax + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7a]

				movq		mm3, [eax + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]

				paddsw	mm2, mm3

				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]

				psubsw	mm2, mm3

				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + esi], mm4			// r0
				movq		[ebx + esi+8], mm5			// r
				movq		[ecx], mm6	// r7
				movq		[ecx+8], mm7	// r7



				//////////////////////////////

				pxor		mm7, mm7

				movq		mm6, [RoundSecond1a]

				movq		mm0, [eax + 0 * 16 + 0]
				psubsw	mm0, [eax + 4 * 16 + 0]
				paddsw	mm0, mm6

				movq		mm1, [eax + 0 * 16 + 8]
				psubsw	mm1, [eax + 4 * 16 + 8]
				paddsw	mm1, mm6

				movq		[eax + 0 * 16 + 0], mm7
				movq		[eax + 4 * 16 + 0], mm7
				movq		[eax + 0 * 16 + 8], mm7
				movq		[eax + 4 * 16 + 8], mm7

				movq		mm2, [eax + 2 * 16 + 0]
				movq		mm3, [pC6pC6pC6pC6a]

				pmulhw	mm2, mm3
				paddsw	mm2, mm2

				movq		mm4, [eax + 6 * 16 + 0]
				movq		mm5, [pC2pC2pC2pC2a]
				psubsw	mm2, mm4
				pmulhw	mm4, mm5

				psubsw	mm2, mm4 // y2 0

				pmulhw	mm3, [eax + 2 * 16 + 8]
				paddsw	mm3, mm3

				movq		mm4, [eax + 6 * 16 + 8]
				psubsw	mm3, mm4
				pmulhw	mm4, mm5

				psubsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		[eax + 2 * 16 + 0], mm7
				movq		[eax + 6 * 16 + 0], mm7
				movq		[eax + 2 * 16 + 8], mm7
				movq		[eax + 6 * 16 + 8], mm7

				movq		mm6, [eax + 1 * 16 + 0]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm6
				paddsw	mm2, mm6

				movq		mm3, [eax + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm7, [eax + 1 * 16 + 8]
				movq		mm2, [pC3pC3pC3pC3a]
				pmulhw	mm2, mm7
				paddsw	mm2, mm7

				movq		mm3, [eax + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5a]
				paddsw	mm3, mm3

				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, SECOND_SHIFT + 2
				psraw		mm1, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + edx], mm0			// r1
				movq		[ebx + edx+8], mm1			// r1
				movq		[ecx + 2 * edx], mm6	// r6
				movq		[ecx + 2 * edx+8], mm7	// r6
				//////
				pxor		mm1, mm1

				movq		mm2, [eax + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2

				movq		mm3, [eax + 3 * 16 + 0]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 0]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		[eax + 1 * 16 + 0], mm1
				movq		[eax + 3 * 16 + 0], mm1
				movq		[eax + 5 * 16 + 0], mm1
				movq		[eax + 7 * 16 + 0], mm1

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [eax + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5a]
				paddsw	mm2, mm2

				movq		mm3, [eax + 3 * 16 + 8]
				psubsw	mm2, mm3
				pmulhw	mm3, [pC1pC1pC1pC1a]

				psubsw	mm2, mm3

				movq		mm3, [eax + 5 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7a]

				paddsw	mm2, mm3

				movq		mm3, [eax + 7 * 16 + 8]
				paddsw	mm2, mm3
				pmulhw	mm3, [pC3pC3pC3pC3a]

				paddsw	mm2, mm3

				movq		[eax + 1 * 16 + 8], mm1
				movq		[eax + 3 * 16 + 8], mm1
				movq		[eax + 5 * 16 + 8], mm1
				movq		[eax + 7 * 16 + 8], mm1


				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, SECOND_SHIFT + 2
				psraw		mm5, SECOND_SHIFT + 2
				psraw		mm6, SECOND_SHIFT + 2
				psraw		mm7, SECOND_SHIFT + 2

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ebx + 2 * edx+8], mm5			// r2
				movq		[ecx + edx], mm6	// r5
				movq		[ecx + edx+8], mm7	// r5
				}
		}
	}


void GenericXMMXMacroBlockIDCTDecoder::InverseBYTEIntraDCT(BYTE * mb, int stride, DCTMulTable * table, int numLines, int numColumns)
	{
	static MMXQWORD RoundOne0    = {ROUND_ONE, ROUND_ONE - 1, ROUND_ONE, ROUND_ONE - 1};
	static MMXQWORD RoundSingle0 = {ROUND_SINGLE, ROUND_SINGLE - 1, ROUND_SINGLE, ROUND_SINGLE - 1};
	static MMXQWORD RoundSecond0 = {ROUND_SECOND, ROUND_SECOND - 1, ROUND_SECOND, ROUND_SECOND - 1};

	static MMXQWORD RoundOne1    = {ROUND_ONE - 1, ROUND_ONE, ROUND_ONE - 1, ROUND_ONE};
	static MMXQWORD RoundSingle1 = {ROUND_SINGLE - 1, ROUND_SINGLE, ROUND_SINGLE - 1, ROUND_SINGLE};
	static MMXQWORD RoundSecond1 = {ROUND_SECOND - 1, ROUND_SECOND, ROUND_SECOND - 1, ROUND_SECOND};

	switch (numColumns)
		{
		case 1:
			if (numLines == 1)
				{
				__asm
					{
					lea			edi, [tmmxf]
					mov			ebx, [mb]
					mov			edx, [stride]
					mov			esi, [table]

					// XMMX
					pshufwMEM	(mm0, edi, 0x00)		// tm0 tm0 tm0 tm0
				//	movq			mm0, [edi]				// tm3 tm2 tm1 tm0
				//	punpcklwd	mm0, mm0					// tm1 tm1 tm0 tm0
				//	punpckldq	mm0, mm0					// tm0 tm0 tm0 tm0

					pmulhw		mm0, [esi].tpC8pC8pC8pC8


					pxor			mm2, mm2

					lea			eax, [ebx+2*edx]

					lea			ecx, [ebx+2*edx]

					movq			mm1, mm0
					lea			eax, [eax+edx]

					paddw			mm0, [RoundOne0]

					movq			[edi], mm2
					psraw			mm0, ONE_SHIFT

					paddw			mm1, [RoundOne1]
					packuswb		mm0, mm0

					psraw			mm1, ONE_SHIFT

					movq			[ebx], mm0
					packuswb		mm1, mm1

					movq			[ebx+2*edx], mm0

					movq			[ebx+4*edx], mm0

					movq			[ecx+4*edx], mm0

					movq			[ebx+edx], mm1

					movq			[eax], mm1

					movq			[eax+2*edx], mm1

					movq			[eax+4*edx], mm1
					}

				return;
				}
			else
				{
				__asm
					{
					lea	eax, tmmxf
					mov	ecx, [numLines]
					mov	esi, [table]
loop11:
					movq	mm0, [eax+0]

					pmaddwd mm0, [esi].tpC8000000000

					add	eax, 16

					psrad			mm0, FIRST_SHIFT

					punpckldq	mm0, mm0

					packssdw		mm0, mm0

					movq	[eax - 16], mm0

					movq	[eax - 8], mm0

					sub	ecx, 1
					jne	loop11
					}
				}
			break;
		case 2:
			__asm
				{
				mov			esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]
loop12:
				movq			mm1, [eax+8]			// tm7 tm6 tm5 tm4
				punpcklwd	mm1, [eax+0]			// tm1 tm5 tm0 tm4
				punpckldq	mm1, mm1					// tm0 tm4 tm0 tm4
				movq			mm4, mm1					// tm0 tm4 tm0 tm4

				pmaddwd		mm1, [esi].tpC8pC7pC8pC5
				movq			mm2, mm4

				pmaddwd		mm2, [esi].tpC8pC3pC8pC1
				movq			mm3, mm4

				pmaddwd		mm3, [esi].tpC8mC1pC8mC3

				pmaddwd		mm4, [esi].tpC8mC5pC8mC7
				psrad			mm1, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT
				add			eax, 16

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				packssdw		mm2, mm1

				packssdw		mm4, mm3

				movq			[eax - 16], mm2

				movq			[eax - 8], mm4

				sub	ecx, 1
				jne	loop12
				}
			break;
		case 3:
			__asm
				{
				mov	esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]
loop13:

				// XMMX
				movq			mm0, [eax+0]				// tm3 tm2 tm1 tm0
				pshufw		(mm1, mm0, 0xAA)			// tm2 tm2 tm2 tm2
				pshufwIMM8	(mm4, eax, 8, 0x00)		// tm4 tm4 tm4 tm4
				punpcklwd	mm4, mm1						// tm2 tm4 tm2 tm4
				movq			mm1, mm4						// tm2 tm4 tm2 tm4

			//	movq			mm0, [eax+0]				// tm3 tm2 tm1 tm0
			//	movq			mm4, [eax+8]				// tm7 tm6 tm5 tm4
			//	movq			mm2, [eax+0]				// tm3 tm2 tm1 tm0
			//	psrlq			mm2, 32						//   0   0 tm3 tm2
			//	punpcklwd	mm4, mm2						// tm3 tm5 tm2 tm4
			//	punpckldq	mm4, mm4						// tm2 tm4 tm2 tm4
			//	movq			mm1, mm4						// tm2 tm4 tm2 tm4

				pmaddwd		mm0, [esi].tpC8000000000

				add			eax, 16


				punpckldq	mm0, mm0

				pmaddwd		mm1, [esi].tmC2pC7mC6pC5
				movq			mm2, mm4

				pmaddwd		mm2, [esi].tpC6pC3pC2pC1
				movq			mm3, mm4

				pmaddwd		mm3, [esi].tpC2mC1pC6mC3
				psrad			mm0, FIRST_SHIFT

				pmaddwd		mm4, [esi].tmC6mC5mC2mC7
				psrad			mm1, FIRST_SHIFT

				psrad			mm2, FIRST_SHIFT

				psrad			mm3, FIRST_SHIFT

				psrad			mm4, FIRST_SHIFT

				packssdw		mm0, mm0

				packssdw		mm2, mm1

				packssdw		mm4, mm3
				paddsw		mm2, mm0

				paddsw		mm4, mm0

				movq	[eax - 16], mm2

				movq	[eax - 8], mm4

				sub	ecx, 1
				jne	loop13
				}
			break;
		case 4:
			__asm
				{
				mov	esi, [table]
				lea			eax, tmmxf
				mov			ecx, [numLines]

loop14:

				// XMMX
				pshufwMEM	(mm0, eax, 0x88)
			//	movq			mm0, [eax+0]			// tm3 tm2 tm1 tm0
			//	psllq			mm0, 32					//	tm1 tm0	 0	  0
			//	punpckhwd	mm0, [eax+0]			//	tm3 tm1 tm2 tm0
			//	punpckldq	mm0, mm0					// tm2 tm0 tm2 tm0

				// XMMX
				pshufwIMM8	(mm1, eax, 8, 0x88)
			//	movq			mm1, [eax+8]			// tm7 tm6 tm5 tm4
			//	psllq			mm1, 32					//	tm5 tm4   0   0
			//	punpckhwd	mm1, [eax+8]			//	tm7 tm5 tm6 tm4
			//	punpckldq	mm1, mm1					// tm6 tm4 tm6 tm4

			//	movq			mm2, mm0					//	tm2 tm0 tm2 tm0
			//	movq			mm3, mm1					//	tm6 tm4 tm6 tm4

				movq			mm2, [esi].tmC2pC8mC6pC8
			//	pmaddwd		mm0, [esi].tpC6pC8pC2pC8
				pmaddwd		mm2, mm0

				pmaddwd		mm0, [esi].tpC6pC8pC2pC8

				movq			mm3, [esi].tmC5pC7mC1pC5
			//	pmaddwd		mm1, [esi].tmC7pC3pC3pC1
				pmaddwd		mm3, mm1

				pmaddwd		mm1, [esi].tmC7pC3pC3pC1

				movq			mm4, mm0

				movq			mm5, mm2
				paddd			mm0, mm1

				psrad			mm0, FIRST_SHIFT
				paddd			mm2, mm3

				psrad			mm2, FIRST_SHIFT
				psubd			mm4, mm1

				psrad			mm4, FIRST_SHIFT
				psubd			mm5, mm3

				psrad			mm5, FIRST_SHIFT

				packssdw		mm5, mm4
				add			eax, 16

				movq			mm4, mm5
				packssdw		mm0, mm2

				pslld			mm5, 16

				psrld			mm4, 16

				por			mm4, mm5

				movq			[eax-16], mm0

				movq			[eax-8], mm4

				sub			ecx, 1
				jne			loop14
				}
			break;
		default:
			__asm
				{
				mov	esi, [table]
				lea			eax, [tmmxf]
				mov			ecx, [numLines]
loop18a:

				// XMMX
				pshufwMEM	(mm0, eax, 0x44)		// tm1 tm0 tm1 tm0
			//	movq			mm0, [eax+0]
			//	punpckldq	mm0, mm0					// f4 f0 f4 f0

				// XMMX
				pshufwMEM	(mm1, eax, 0xEE)		// tm3 tm2 tm3 tm2
			//	movq			mm1, [eax+0]
			//	punpckhdq	mm1, mm1					// f6 f2 f6 f2


				pmaddwd		mm0, [esi].tmC8pC8pC8pC8

				pmaddwd		mm1, [esi].tmC2pC6pC6pC2

				movq			mm2, mm0
				paddd			mm0, mm1					// a1 a0
				psubd			mm2, mm1					// a3 a2

				// XMMX
				pshufw		(mm2, mm2, 0x4E)		// fpli DWORDS
			//	punpckldq	mm1, mm2
			//	punpckhdq	mm2, mm1					// a2 a3

				// XMMX
				pshufwIMM8	(mm1, eax, 8, 0x44)
			//	movq			mm1, [eax+8]			// f7 f3 f5 f1
			//	punpckldq	mm1, mm1					// f5 f1 f5 f1

				// XMMX
				pshufwIMM8	(mm3, eax, 8, 0xEE)
			//	movq			mm3, [eax+8]
			//	punpckhdq	mm3, mm3					// f7 f3 f7 f3

				movq			mm4, [esi].tmC1pC3pC5pC1
				pmaddwd		mm4, mm1					// y6a y4a
				pmaddwd		mm1, [esi].tpC3pC7pC7pC5	// y5a y7a

				movq			mm5, [esi].tmC5mC7pC7pC3
				pmaddwd		mm5, mm3					// y6b y4b
				pmaddwd		mm3, [esi].tmC1mC5pC3mC1	// y5b y7b

				paddd			mm4, mm5
				paddd			mm1, mm3

				movq			mm3, mm0
				movq			mm5, mm2

				paddd			mm0, mm4					// t1 t0
				paddd			mm2, mm1					// t2 t3

				psrad			mm0, FIRST_SHIFT
				psrad			mm2, FIRST_SHIFT

				psubd			mm3, mm4					// t6 t7
				psubd			mm5, mm1					// t4 t5

				psrad			mm3, FIRST_SHIFT
				psrad			mm5, FIRST_SHIFT

				packssdw		mm0, mm2					// t3 t2 t1 t0
				packssdw		mm5, mm3					// t6 t7 t4 t5

				movq			mm3, mm5
				pslld			mm5, 16

				movq			[eax], mm0
				psrld			mm3, 16
				por			mm3, mm5					// t4 t5 t6 t7

				movq			[eax+8], mm3

				add			eax, 16
				sub			ecx, 1
				jne			loop18a
				}
			break;
		}

	switch (numLines)
		{
		case 1:
			__asm
				{
				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, [RoundSingle0]
				pxor		mm7, mm7
				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, [RoundSingle1]
				psraw		mm0, 7
				psraw		mm1, 7
				movq		[tmmxf + 0 * 16 + 0], mm7
				mov		ebx, [mb]
				mov		edx, [stride]
				movq		[tmmxf + 0 * 16 + 8], mm7
				lea		ecx, [ebx+4*edx]
				packuswb	mm0, mm1

				movq		[ebx], mm0
				lea		esi, [edx+2*edx]
				movq		[ebx+edx], mm0
				movq		[ebx+2*edx], mm0
				movq		[ebx+esi], mm0
				movq		[ecx], mm0
				movq		[ecx+edx], mm0
				movq		[ecx+2*edx], mm0
				movq		[ecx+esi], mm0
				}
			break;
		case 2:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7


				//////////////////////

				movq		mm3, [pC1pC1pC1pC1]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx], mm4
				movq		[ecx+esi], mm5


				movq		mm3, [pC3pC3pC3pC3]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+edx], mm4
				movq		[ecx+2*edx], mm5


				movq		mm3, [pC5pC5pC5pC5]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+2*edx], mm4
				movq		[ecx+edx], mm5


				movq		mm3, [pC7pC7pC7pC7]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm4, mm0
				movq		mm5, mm0
				paddsw	mm4, mm2
				psubsw	mm5, mm2

				movq		mm6, mm1
				movq		mm7, mm1
				paddsw	mm6, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm6
				packuswb	mm5, mm7

				movq		[ebx+esi], mm4
				movq		[ecx], mm5

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}

			break;
		case 3:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				movq		mm3, [pC2pC2pC2pC2]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]
				movq		mm4, mm0
				movq		mm5, mm1

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC1pC1pC1pC1]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, 5
				psraw		mm1, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0
				movq		[ecx+esi], mm6

				//////////

				movq		mm3, [pC7pC7pC7pC7]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx+esi], mm4
				movq		[ecx], mm6

				////////////////////////////

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				movq		mm3, [pC6pC6pC6pC6]
				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 2 * 16 + 8]
				paddsw	mm0, [RoundSecond0]
				paddsw	mm1, [RoundSecond0]
				movq		mm4, mm0
				movq		mm5, mm1

				pxor		mm7, mm7
				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 2 * 16 + 8], mm7

				paddsw	mm0, mm2
				paddsw	mm1, mm3
				psubsw	mm4, mm2
				psubsw	mm5, mm3

				/////////////

				movq		mm3, [pC3pC3pC3pC3]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm0
				paddsw	mm0, mm2
				psubsw	mm6, mm2

				movq		mm7, mm1
				paddsw	mm1, mm3
				psubsw	mm7, mm3

				psraw		mm0, 5
				psraw		mm1, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx+edx], mm0
				movq		[ecx+2*edx], mm6

				//////////

				movq		mm3, [pC5pC5pC5pC5]
				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, mm3
				pmulhw	mm3, [tmmxf + 1 * 16 + 8]

				movq		mm6, mm4
				paddsw	mm4, mm2
				psubsw	mm6, mm2

				movq		mm7, mm5
				paddsw	mm5, mm3
				psubsw	mm7, mm3

				psraw		mm4, 5
				psraw		mm5, 5
				psraw		mm6, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx+2*edx], mm4
				movq		[ecx+edx], mm6

				pxor		mm7, mm7
				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 1 * 16 + 8], mm7
				}
			break;
		case 4:
			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC2pC2pC2pC2]
				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC2pC2pC2pC2]

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0			// r0
				movq		[ecx + esi], mm6	// r7

				//////

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + esi], mm4			// r0
				movq		[ecx], mm6	// r7

				//////////////////////////////

				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 8], mm7

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC6pC6pC6pC6]

				movq		[tmmxf + 2 * 16 + 0], mm7

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC6pC6pC6pC6]

				movq		[tmmxf + 2 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx + edx], mm0			// r1
				movq		[ecx + 2 * edx], mm6	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ecx + edx], mm6	// r5
				}
			break;
		default:

			__asm
				{
				mov		ebx, [mb]
				mov		edx, [stride]
				lea		ecx, [ebx+4*edx]
				lea		esi, [edx+2*edx]

				movq		mm0, [tmmxf + 0 * 16 + 0]
				paddsw	mm0, [tmmxf + 4 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		mm1, [tmmxf + 0 * 16 + 8]
				paddsw	mm1, [tmmxf + 4 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC2pC2pC2pC2]
				movq		mm3, [tmmxf + 6 * 16 + 0]
				pmulhw	mm3, [pC6pC6pC6pC6]
				paddsw	mm2, mm3 // y2 0

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC2pC2pC2pC2]
				movq		mm4, [tmmxf + 6 * 16 + 8]
				pmulhw	mm4, [pC6pC6pC6pC6]
				paddsw	mm3, mm4 // y2 1

				movq		mm4, mm0
				paddsw	mm0, mm2	// a0 0
				psubsw	mm4, mm2 // a3 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a0 1
				psubsw	mm5, mm3 // a3 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r0	0
				psubsw	mm6, mm2		// r7	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC1pC1pC1pC1]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r0 1
				psubsw	mm7, mm2		// r7 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx], mm0			// r0
				movq		[ecx + esi], mm6	// r7

				//////

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		mm6, mm4
				paddsw	mm4, mm2		// r3	0
				psubsw	mm6, mm2		// r4	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC7pC7pC7pC7]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3

				movq		mm7, mm5
				paddsw	mm5, mm2		// r3 1
				psubsw	mm7, mm2		// r4 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + esi], mm4			// r0
				movq		[ecx], mm6	// r7

				//////////////////////////////

				pxor		mm7, mm7

				movq		mm0, [tmmxf + 0 * 16 + 0]
				psubsw	mm0, [tmmxf + 4 * 16 + 0]
				psraw		mm0, 2
				paddsw	mm0, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 0], mm7
				movq		[tmmxf + 4 * 16 + 0], mm7

				movq		mm1, [tmmxf + 0 * 16 + 8]
				psubsw	mm1, [tmmxf + 4 * 16 + 8]
				psraw		mm1, 2
				paddsw	mm1, [RoundSecond0]

				movq		[tmmxf + 0 * 16 + 8], mm7
				movq		[tmmxf + 4 * 16 + 8], mm7

				movq		mm2, [tmmxf + 2 * 16 + 0]
				pmulhw	mm2, [pC6pC6pC6pC6]
				movq		mm3, [tmmxf + 6 * 16 + 0]
				pmulhw	mm3, [pC2pC2pC2pC2]
				psubsw	mm2, mm3 // y2 0

				movq		[tmmxf + 2 * 16 + 0], mm7
				movq		[tmmxf + 6 * 16 + 0], mm7

				movq		mm3, [tmmxf + 2 * 16 + 8]
				pmulhw	mm3, [pC6pC6pC6pC6]
				movq		mm4, [tmmxf + 6 * 16 + 8]
				pmulhw	mm4, [pC2pC2pC2pC2]
				psubsw	mm3, mm4 // y2 1

				movq		[tmmxf + 2 * 16 + 8], mm7
				movq		[tmmxf + 6 * 16 + 8], mm7

				movq		mm4, mm0
				paddsw	mm0, mm2	// a1 0
				psubsw	mm4, mm2 // a2 0

				movq		mm5, mm1
				paddsw	mm1, mm3 // a1 1
				psubsw	mm5, mm3 // a2 1

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm6, mm0
				paddsw	mm0, mm2		// r1	0
				psubsw	mm6, mm2		// r6	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC3pC3pC3pC3]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC5pC5pC5pC5]
				psubsw	mm2, mm3

				movq		mm7, mm1
				paddsw	mm1, mm2		// r1 1
				psubsw	mm7, mm2		// r6 1

				psraw		mm0, 5
				psraw		mm6, 5
				psraw		mm1, 5
				psraw		mm7, 5

				packuswb	mm0, mm1
				packuswb	mm6, mm7

				movq		[ebx + edx], mm0			// r1
				movq		[ecx + 2 * edx], mm6	// r6

				//////
				pxor		mm7, mm7

				movq		mm2, [tmmxf + 1 * 16 + 0]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 0]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 0]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 0]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 0], mm7
				movq		[tmmxf + 3 * 16 + 0], mm7
				movq		[tmmxf + 5 * 16 + 0], mm7
				movq		[tmmxf + 7 * 16 + 0], mm7

				movq		mm6, mm4
				paddsw	mm4, mm2		// r2	0
				psubsw	mm6, mm2		// r5	0

				movq		mm2, [tmmxf + 1 * 16 + 8]
				pmulhw	mm2, [pC5pC5pC5pC5]
				movq		mm3, [tmmxf + 3 * 16 + 8]
				pmulhw	mm3, [pC1pC1pC1pC1]
				psubsw	mm2, mm3
				movq		mm3, [tmmxf + 5 * 16 + 8]
				pmulhw	mm3, [pC7pC7pC7pC7]
				paddsw	mm2, mm3
				movq		mm3, [tmmxf + 7 * 16 + 8]
				pmulhw	mm3, [pC3pC3pC3pC3]
				paddsw	mm2, mm3

				movq		[tmmxf + 1 * 16 + 8], mm7
				movq		[tmmxf + 3 * 16 + 8], mm7
				movq		[tmmxf + 5 * 16 + 8], mm7
				movq		[tmmxf + 7 * 16 + 8], mm7

				movq		mm7, mm5
				paddsw	mm5, mm2		// r2 1
				psubsw	mm7, mm2		// r5 1

				psraw		mm4, 5
				psraw		mm6, 5
				psraw		mm5, 5
				psraw		mm7, 5

				packuswb	mm4, mm5
				packuswb	mm6, mm7

				movq		[ebx + 2 * edx], mm4			// r2
				movq		[ecx + edx], mm6	// r5
				}
		}
	}





GenericXMMXMacroBlockIDCTDecoder::GenericXMMXMacroBlockIDCTDecoder(void)
	: GenericMMXMacroBlockIDCTDecoder()
	{
	}

void GenericXMMXMacroBlockIDCTDecoder::ParseIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, short * mb, int stride)
	{
	int l;

	l = ParseIntraBlockMatrix(yuvType, dcPredictor, matrix);
	InverseDCT(mb, stride, dctMulTable[yuvType], hfluti[(((l >> 25) + 8) >> 3) & 0x0f], hfluti[(l >> 17) & 0x0f]);
	}

void GenericXMMXMacroBlockIDCTDecoder::ParseNonIntraBlock(int num, int yuvType, int * matrix, short * mb, int stride)
	{
	int l;

	l = ParseNonIntraBlockMatrix(matrix);
	InverseDCT(mb, stride, dctMulTable[yuvType], hflut[hurryUp][(((l >> 25) + 8) >> 3) & 0x0f], hflut[hurryUp][(l >> 17) & 0x0f]);
	}

void GenericXMMXMacroBlockIDCTDecoder::ParseNonIntraBlocks(int codedBlockPattern, short ** yuvp, int * yuvd, int * matrix, int * offset)
	{
	int l, i, t;
	static char tp[] = {0, 0, 0, 0, 1, 2};

	for(i=0; i<6; i++)
		{
		if ((codedBlockPattern << i) & 0x20)
			{
			t = tp[i];
			l = ParseNonIntraBlockMatrix(matrix);
			tmmxf[0][0] += (short)offset[t];
			InverseDCT(yuvp[i], yuvd[t], dctMulTable[t], hflut[hurryUp][(((l >> 25) + 8) >> 3) & 0x0f], hflut[hurryUp][(l >> 17) & 0x0f]);
			}
		}
	}

