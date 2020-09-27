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


#include <stdio.h>
#include <stdlib.h>
#include "SliceParser.h"
#include "MP2HuffTables.h"
#include <memory.h>
#include "MPEGStatistics.h"
#include "library\common\vddebug.h"

#define DUMP_MOTION_VECTORS	0

#if DUMP_MOTION_VECTORS
#include <stdio.h>
static FILE * mfile;
static int frameCount;
int vectorSumX[256], vectorSumY[256];
int vectorTotal;
#endif

static const int ScanTable[2][8][8] =
	{{{  0,  1,  5,  6, 14, 15, 27, 28},
	  {  2,  4,  7, 13, 16, 26, 29, 42},
	  {  3,  8, 12, 17, 25, 30, 41, 43},
	  {  9, 11, 18, 24, 31, 40, 44, 53},
	  { 10, 19, 23, 32, 39, 45, 52, 54},
	  { 20, 22, 33, 38, 46, 51, 55, 60},
	  { 21, 34, 37, 47, 50, 56, 59, 61},
	  { 35, 36, 48, 49, 57, 58, 62, 63}},
	 {{  0,  4,  6, 20, 22, 36, 38, 52},
	  {  1,  5,  7, 21, 23, 37, 39, 53},
	  {  2,  8, 19, 24, 34, 40, 50, 54},
	  {  3,  9, 18, 25, 35, 41, 51, 55},
	  { 10, 17, 26, 30, 42, 46, 56, 60},
	  { 11, 16, 27, 31, 43, 47, 57, 61},
	  { 12, 15, 28, 32, 44, 48, 58, 62},
	  { 13, 14, 29, 33, 45, 49, 59, 63}}};

unsigned char SliceParser::InverseScanTable[2][64];

const int SliceParser::QuantizerScale[2][32] =
	{{  0,  2,  4,  6,  8, 10, 12, 14,
	   16, 18, 20, 22, 24, 26, 28, 30,
		32, 34, 36, 38, 40, 42, 44, 46,
		48, 50, 52, 54, 56, 58, 60, 62},
	 {  0,  1,  2,  3,  4,  5,  6,  7,
	    8, 10, 12, 14, 16, 18, 20, 22,
		24, 28, 32, 36, 40, 44, 48, 52,
		56, 64, 72, 80, 88, 96,104,112}};

void SliceParser::InitInverseScanTable(void)
	{
	int i, x, y;

	for(i=0; i<2; i++)
		{
		for(x=0; x<8; x++)
			{
			for(y=0; y<8; y++)
				{
				InverseScanTable[i][ScanTable[i][y][x]] = 8 * x + y;
				}
			}
		}
	}

__forceinline void SliceParser::ParseMacroblockModes(void)
	{
	macroblockType = bitStream->GetSymbol(macroblockTypeTable);
	dctType = IS_MACROBLOCK_ALTERNATE_DCT(macroblockType);
	motionVectorCount = MACROBLOCK_MOTION_COUNT(macroblockType);
	predictionType = MACROBLOCK_PREDICT_TYPE(macroblockType);
	dmv = 0;
	}

static __forceinline int CalcMotionComponentField(int f, int & pmv, int delta)
	{
	int high, low, range, v;

	high = f - 1;
	low = -f;
	range = 2 * f;

	v = (pmv >> 1) + delta;
	if      (v < low)  v += range;
	else if (v > high) v -= range;
	pmv = v * 2;

	return v;
	}

static __forceinline int CalcMotionComponentFrame(int f, int & pmv, int delta)
	{
	int high, low, range, v;

	high = f - 1;
	low = -f;
	range = 2 * f;

	v = pmv + delta;
	if      (v < low)  v += range;
	else if (v > high) v -= range;
	pmv = v;

	return v;
	}

#if DUMP_MOTION_VECTORS
static void ProcessMVStatistic(int mx, int my)
	{
	if (mx < 0) mx = -mx;
	if (my < 0) my = -my;
	vectorSumX[mx]++;
	vectorSumY[my]++;
	vectorTotal++;
	}

static void DumpMVStatistic(void)
	{
	int i;

	if (mfile)
		{
		if (vectorTotal)
			{
			for(i=0; i<256; i++)
				{
				fprintf(mfile, "Size %3d X : %6d %3d%%  Y : %6d %3d%%\n",
					i,
					vectorSumX[i], 100 * vectorSumX[i] / vectorTotal,
					vectorSumY[i], 100 * vectorSumY[i] / vectorTotal);
				}
			}
		}
	}
#endif

__forceinline int CalcDualPrime(int vector, int m, int e, int dmv)
	{
	vector *= m;
	if (vector > 0)
		vector = (vector + 1) >> 1;
	else
		vector = vector >> 1;
	vector += e + dmv;

	return vector;
	}

__forceinline void SliceParser::CalcMotionComponents(void)
	{
	if (motionVectorCount == 1)
		{
		if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
			{
			vectors[0][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[0][0][0], motionValue[0][0][0]);
			vectors[0][0][1] = CalcMotionComponentFrame(ffcodes[0][1], pmv[0][0][1], motionValue[0][0][1]);
			pmv[1][0][0] = pmv[0][0][0];
			pmv[1][0][1] = pmv[0][0][1];

#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "FW %d, %d ", vectors[0][0][0], vectors[0][0][1]);
		ProcessMVStatistic(vectors[0][0][0], vectors[0][0][1]);
#endif
			}
		if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
			{
			vectors[0][1][0] = CalcMotionComponentFrame(ffcodes[1][0], pmv[0][1][0], motionValue[0][1][0]);
			vectors[0][1][1] = CalcMotionComponentFrame(ffcodes[1][1], pmv[0][1][1], motionValue[0][1][1]);
			pmv[1][1][0] = pmv[0][1][0];
			pmv[1][1][1] = pmv[0][1][1];

#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "BW %d, %d ", vectors[0][1][0], vectors[0][1][1]);
		ProcessMVStatistic(vectors[0][1][0], vectors[0][1][1]);
#endif
			}
		}
	else if (motionVectorCount == 2)
		{
		if (predictionType == PT_16x8MC)
			{
			if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
				{
				vectors[0][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[0][0][0], motionValue[0][0][0]);
				vectors[0][0][1] = CalcMotionComponentFrame(ffcodes[0][1], pmv[0][0][1], motionValue[0][0][1]);
				vectors[1][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[1][0][0], motionValue[1][0][0]);
				vectors[1][0][1] = CalcMotionComponentFrame(ffcodes[0][1], pmv[1][0][1], motionValue[1][0][1]);
#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "FW16x8 %d, %d | %d, %d", vectors[0][0][0], vectors[0][0][1], vectors[1][0][0], vectors[1][0][1]);
		ProcessMVStatistic(vectors[0][0][0], vectors[0][0][1]);
		ProcessMVStatistic(vectors[1][0][0], vectors[1][0][1]);
#endif
				}
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				vectors[0][1][0] = CalcMotionComponentFrame(ffcodes[1][0], pmv[0][1][0], motionValue[0][1][0]);
				vectors[0][1][1] = CalcMotionComponentFrame(ffcodes[1][1], pmv[0][1][1], motionValue[0][1][1]);
				vectors[1][1][0] = CalcMotionComponentFrame(ffcodes[1][0], pmv[1][1][0], motionValue[1][1][0]);
				vectors[1][1][1] = CalcMotionComponentFrame(ffcodes[1][1], pmv[1][1][1], motionValue[1][1][1]);
#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "BW16x8 %d, %d | %d, %d", vectors[0][1][0], vectors[0][1][1], vectors[1][1][0], vectors[1][1][1]);
		ProcessMVStatistic(vectors[0][1][0], vectors[0][1][1]);
		ProcessMVStatistic(vectors[1][1][0], vectors[1][1][1]);
#endif
				}
			}
		else
			{
			if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
				{
				vectors[0][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[0][0][0], motionValue[0][0][0]);
				vectors[0][0][1] = CalcMotionComponentField(ffcodes[0][1], pmv[0][0][1], motionValue[0][0][1]);
				vectors[1][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[1][0][0], motionValue[1][0][0]);
				vectors[1][0][1] = CalcMotionComponentField(ffcodes[0][1], pmv[1][0][1], motionValue[1][0][1]);
#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "FW %d, %d | %d, %d", vectors[0][0][0], vectors[0][0][1], vectors[1][0][0], vectors[1][0][1]);
		ProcessMVStatistic(vectors[0][0][0], vectors[0][0][1]);
		ProcessMVStatistic(vectors[1][0][0], vectors[1][0][1]);
#endif
				}
			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				{
				vectors[0][1][0] = CalcMotionComponentFrame(ffcodes[1][0], pmv[0][1][0], motionValue[0][1][0]);
				vectors[0][1][1] = CalcMotionComponentField(ffcodes[1][1], pmv[0][1][1], motionValue[0][1][1]);
				vectors[1][1][0] = CalcMotionComponentFrame(ffcodes[1][0], pmv[1][1][0], motionValue[1][1][0]);
				vectors[1][1][1] = CalcMotionComponentField(ffcodes[1][1], pmv[1][1][1], motionValue[1][1][1]);
#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "BW %d, %d | %d, %d", vectors[0][1][0], vectors[0][1][1], vectors[1][1][0], vectors[1][1][1]);
		ProcessMVStatistic(vectors[0][1][0], vectors[0][1][1]);
		ProcessMVStatistic(vectors[1][1][0], vectors[1][1][1]);
#endif
				}
			}
		}
	else if (motionVectorCount == 4)
		{
		if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
			{
			vectors[1][0][0] = vectors[0][0][0] = CalcMotionComponentFrame(ffcodes[0][0], pmv[0][0][0], motionValue[0][0][0]);
			if (pictureStructure == PS_FRAME_PICTURE)
				{
				vectors[1][0][1] = vectors[0][0][1] = CalcMotionComponentField(ffcodes[0][1], pmv[0][0][1], motionValue[0][0][1]);

				if (PF_TOP_FIELD_FIRST(pictureFlags))
					{
					vectors[2][0][0] = CalcDualPrime(vectors[0][0][0], 1,  0, dmvector[0]);
					vectors[2][0][1] = CalcDualPrime(vectors[0][0][1], 1, -1, dmvector[1]);

					vectors[3][0][0] = CalcDualPrime(vectors[0][0][0], 3,  0, dmvector[0]);
					vectors[3][0][1] = CalcDualPrime(vectors[0][0][1], 3, +1, dmvector[1]);
					}
				else
					{
					vectors[2][0][0] = CalcDualPrime(vectors[0][0][0], 3,  0, dmvector[0]);
					vectors[2][0][1] = CalcDualPrime(vectors[0][0][1], 3, -1, dmvector[1]);

					vectors[3][0][0] = CalcDualPrime(vectors[0][0][0], 1,  0, dmvector[0]);
					vectors[3][0][1] = CalcDualPrime(vectors[0][0][1], 1, +1, dmvector[1]);
					}
				}
			else
				{
				vectors[1][0][1] = vectors[0][0][1] = CalcMotionComponentFrame(ffcodes[0][1], pmv[0][0][1], motionValue[0][0][1]);
				if (pictureStructure == PS_TOP_FIELD)
					{
					vectors[2][0][0] = CalcDualPrime(vectors[1][0][0], 1,  0, dmvector[0]);
					vectors[2][0][1] = CalcDualPrime(vectors[1][0][1], 1, -1, dmvector[1]);
					}
				else
					{
					vectors[2][0][0] = CalcDualPrime(vectors[1][0][0], 1,  0, dmvector[0]);
					vectors[2][0][1] = CalcDualPrime(vectors[1][0][1], 1, +1, dmvector[1]);
					}
				}

			pmv[1][0][0] = pmv[0][0][0];
			pmv[1][0][1] = pmv[0][0][1];

#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "FW %d, %d ", vectors[0][0][0], vectors[0][0][1]);
#endif
			}
		}
	else if (pictureCodingType == PREDICTIVE_CODED)
		{
		ResetPMV();
		vectors[0][0][0] = 0; vectors[0][0][1] = 0;
		vectors[1][0][0] = 0; vectors[1][0][1] = 0;
		motionVerticalFieldSelect[0][0] = currentFieldID;
		}
	else
		{
		ResetPMV();
		motionVerticalFieldSelect[0][0] = currentFieldID;
		motionVerticalFieldSelect[0][1] = currentFieldID;
		}
	}

__forceinline void SliceParser::ResetPMV(void)
	{
	memset(pmv, 0, sizeof(pmv));
	}

__forceinline void SliceParser::ResetPredictors(void)
	{
	dcPredictor[0] = dcLumaPredictorInitializer;
	dcPredictor[1] = dcChromaUPredictorInitializer;
	dcPredictor[2] = dcChromaVPredictorInitializer;
	}

__forceinline void SliceParser::ParseCodedBlockPattern(void)
	{
#if _M_IX86
	__asm
		{
		mov	ecx, [BSBits]
		mov	edx, VIDEO_STREAM_BUFFER_MASK

		mov	edi, [this]
		and	edx, ecx

		shr	edx, 5

		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	eax, [VideoStreamBuffer +     edx * 4]

		shld	eax, ebx, cl
		mov	ebx, 0x3f

		test	eax, 0xc0000000
		je		longCode

		shr	eax, 27
		movzx	eax, [codedBlockPattern420 + eax].extPatternTable;

		and	ebx, eax
		shr	eax, 6

		lea	ecx, [ecx + eax + 3]
		jmp	doneCode
longCode:
		shr	eax, 23
		movzx	eax, [codedBlockPattern420 + 32 + eax].extPatternTable;

		and	ebx, eax
		shr	eax, 6

		lea	ecx, [ecx + eax + 6]
doneCode:
		mov	[BSBits], ecx

		mov	[edi].codedBlockPattern, ebx
		}
#else
	codedBlockPattern = bitStream->GetSymbol(&codedBlockPattern420);
#endif
	}


#pragma warning (disable : 4035)

__forceinline int SliceParser::GetMotionVector(int fcode, HuffmanTable * table)
	{
#if _M_IX86
	__asm
		{
		mov	esi, [BSBits]
		mov	edx, VIDEO_STREAM_BUFFER_MASK

		mov	ecx, esi
		and	edx, esi

		shr	edx, 5
		//

		//
		//

		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	eax, [VideoStreamBuffer +     edx * 4]

		shld	eax, ebx, cl

		test	eax, 0xc0000000
		js		zero

		mov	edx, eax
		jne	one

		shr	eax, 22
		mov	ebx, [table]

		add	esi, 9

		mov	eax, [ebx + 4 * eax]
		mov	ecx, 0x000000ff

		and	ecx, eax
		and	eax, 0x00000f00

		shl	edx, cl

		add	esi, ecx
		mov	ebx, edx

		sar	edx, 31
		and	ebx, 0x7fffffff

		shr	ebx, 23
		mov	ecx, [fcode]

		or		eax, ebx
		sub	esi, ecx

		shr	eax, cl

		add	eax, 1
		//

		xor	eax, edx
		and	edx, 0x00000001

		add	eax, edx
		jmp	done
one:
		and	eax, 0x1fffffff
		mov	ecx, [fcode]

		shl	edx, 2
		sub	esi, ecx

		sar	edx, 31
		add	ecx, 21

		shr	eax, cl

		add	esi, 11
		add	eax, 1

		xor	eax, edx

		sub	eax, edx
		jmp	done
zero:
		add	esi, 1
		xor	eax, eax
done:
		mov	[BSBits], esi
		}
#else
	DWORD res, sym, num;
	int vec;

	res = bitStream->Peek();

	if (res & 0x80000000)
		{
		bitStream->Advance(1);

		return 0;
		}
	else if (res & 0xc0000000)
		{
		vec = ((res & 0x1fffffff) >> (fcode + 21)) + 1;
		if (res & 0x20000000) vec = -vec;

		bitStream->Advance(11 - fcode);

		return vec;
		}
	else
		{
		sym = table->FindFlatSymbol(res << 2);
		num = sym & 0xff;
		res <<= num;

		vec = (((sym & 0x00000f00) + ((res & 0x7fffffff) >> 23)) >> fcode) + 1;
		if (res & 0x80000000) vec = - vec;

		bitStream->Advance(9 + num - fcode);

		return vec;
		}

#endif
	}

#pragma warning (default : 4035)

__forceinline void SliceParser::ParseMotionVector(int r, int s)
	{
	motionValue[r][s][0] = GetMotionVector(ifcodes[s][0], &motionCodes);
	motionValue[r][s][1] = GetMotionVector(ifcodes[s][1], &motionCodes);
#if DUMP_MOTION_VECTORS
	if (mfile) fprintf(mfile, "(%d, %d) ", motionValue[r][s][0], motionValue[r][s][1]);
#endif
	}

__forceinline void SliceParser::ParseDMVMotionVector(int s)
	{
	motionValue[0][s][0] = GetMotionVector(ifcodes[s][0], &motionCodes);
	if (bitStream->GetBit())
		dmvector[0] = 1 - 2 * bitStream->GetBit();
	else
		dmvector[0] = 0;

	motionValue[0][s][1] = GetMotionVector(ifcodes[s][1], &motionCodes);
	if (bitStream->GetBit())
		dmvector[1] = 1 - 2 * bitStream->GetBit();
	else
		dmvector[1] = 0;
	motionVectorCount = 4;
	}

void SliceParser::ParseMotionVectors(int s)
	{
	if (motionVectorCount == 1)
		{
		if (IS_MACROBLOCK_DUAL_PRIME_PREDICTION(macroblockType))
			{
			motionVerticalFieldSelect[0][s] = 0;
			motionVerticalFieldSelect[0][s] = 1;
			ParseDMVMotionVector(s);
			}
		else
			{
			if (predictionType != PT_FRAME_BASED)
				{
				motionVerticalFieldSelect[0][s] = bitStream->GetBit();
				}
			ParseMotionVector(0, s);
			}
		}
	else
		{
		motionVerticalFieldSelect[0][s] = bitStream->GetBit();
		ParseMotionVector(0, s);
		motionVerticalFieldSelect[1][s] = bitStream->GetBit();
		ParseMotionVector(1, s);
		}
	}

void SliceParser::ExecuteSkippedMacroblock(int mx, int my)
	{
	codedBlockPattern = 0x00;
	MotionCompensation(mx, my, TRUE, TRUE);
	}

__forceinline void SliceParser::SkipMacroblocks(int num)
	{
	int m, mx, my;

	if (num)
		{
		if (pictureCodingType != INTRA_CODED)
			{
			macroblockType &= ~MF_INTRA;

			ResetPredictors();
			switch (pictureStructure)
				{
				case PS_FRAME_PICTURE:
					if (pictureCodingType == PREDICTIVE_CODED)
						{
						ResetPMV();
						vectors[0][0][0] = 0; vectors[0][0][1] = 0;
						vectors[0][1][0] = 0; vectors[0][1][1] = 0;
						}
					else
						{
						vectors[0][0][0] = pmv[0][0][0];
						vectors[0][0][1] = pmv[0][0][1];
						vectors[0][1][0] = pmv[0][1][0];
						vectors[0][1][1] = pmv[0][1][1];
						}

					predictionType = PT_FRAME_BASED;
					break;
				case PS_TOP_FIELD:
					if (pictureCodingType == PREDICTIVE_CODED)
						{
						ResetPMV();
						vectors[0][0][0] = 0; vectors[0][0][1] = 0;
						vectors[0][1][0] = 0; vectors[0][1][1] = 0;
						motionVerticalFieldSelect[0][0] = 0;
						}
					else
						{
						vectors[0][0][0] = pmv[0][0][0];
						vectors[0][0][1] = pmv[0][0][1];
						vectors[0][1][0] = pmv[0][1][0];
						vectors[0][1][1] = pmv[0][1][1];
						motionVerticalFieldSelect[0][0] = 0;
						motionVerticalFieldSelect[0][1] = 0;
						}

					predictionType = PT_FIELD_BASED;
					break;
				case PS_BOTTOM_FIELD:
					if (pictureCodingType == PREDICTIVE_CODED)
						{
						ResetPMV();
						vectors[0][0][0] = 0; vectors[0][0][1] = 0;
						vectors[0][1][0] = 0; vectors[0][1][1] = 0;
						motionVerticalFieldSelect[0][0] = 1;
						}
					else
						{
						vectors[0][0][0] = pmv[0][0][0];
						vectors[0][0][1] = pmv[0][0][1];
						vectors[0][1][0] = pmv[0][1][0];
						vectors[0][1][1] = pmv[0][1][1];
						motionVerticalFieldSelect[0][0] = 1;
						motionVerticalFieldSelect[0][1] = 1;
						}

					predictionType = PT_FIELD_BASED;
					break;
				}


			for(m=0; m<num; m++)
				{
				mx = macroblockX * 16;
				my = macroblockY * 16;
				if (my >= pictureHeight) return;

				ExecuteSkippedMacroblock(mx, my);

				macroblockAddress++;
				macroblockX++;
				if (macroblockX == macroblocksPerRow)
					{
					macroblockX = 0;
					macroblockY ++;
					}
				}
			}
		else
			{
			macroblockAddress += num;
			macroblockX = macroblockAddress % macroblocksPerRow;
			macroblockY = macroblockAddress / macroblocksPerRow;
			errorInSlice = true;
			}
		}
	}

#pragma warning (disable : 4035)

__forceinline int SliceParser::GetMacroblockAddressIncrement(HuffmanTable * table)
	{
#if _M_IX86
	__asm
		{
		mov	ecx, [BSBits]
		mov	eax, 1

		mov	edx, VIDEO_STREAM_BUFFER_MASK
		mov	esi, [table]

loop1:
		and	edx, ecx

		shr	edx, 5

		mov	ebx, [VideoStreamBuffer + 4 + edx * 4]
		mov	edx, [VideoStreamBuffer +     edx * 4]

		shld	edx, ebx, cl

		test	edx, 0xfe000000
		js		one

		jne	code

		// check for macroblock stuffing, only for MPEG 1 needed,
		// but does not matter here
		test	edx, 0x00e00000
		jne	stuff

		add	eax, 33
stuff:
		add	ecx, 11

		mov	edx, VIDEO_STREAM_BUFFER_MASK
		jmp	loop1

code:
		mov	ebx, edx
		mov	edi, 0x000000ff

		shr	ebx, 24
		and	edx, 0x00ff0000

		shr	edx, 16
		mov	esi, [esi + 4 * ebx]

		test	esi, 1
		jne	hit1

		mov	esi, [esi + 4 * edx]
hit1:
		and	edi, esi
		dec	eax

		shr	edi, 1
		dec	ecx

		sar	esi, 8
		add	ecx, edi

		add	eax, esi
one:
		add	ecx, 1

		mov	[BSBits], ecx
		}
#else
	DWORD res;
	DWORD sym;
	DWORD escape;

	escape = 0;

	for(;;)
		{
		res = bitStream->Peek();

		if (res & 0x80000000)
			{
			bitStream->Advance(1);

			return escape + 1;
			}
		else if (res & 0xfe000000)
			{
			sym = table->FindSymbol(res);

			bitStream->Advance((sym >> 1) & 0x7f);

			return (((int)sym) >> 8) + escape;
			}
		else
			{
			if (!(res & 0x00e0000))
				{
				escape += 33;
				}

			bitStream->Advance(11);
			}
		}
#endif
	}

#pragma warning (default : 4035)

//
//  Parse Intra Macroblock
//

void SliceParser::ParseIntraMacroblock(int mx, int my)
	{
//	DP("Intra MB");
	codedBlockPattern = 0x3f;

	intraDCTable = &dctDCSizeLuminance;
	ParseIntraBlock(0, 0);
	ParseIntraBlock(1, 0);
	ParseIntraBlock(2, 0);
	ParseIntraBlock(3, 0);
	intraDCTable = &dctDCSizeChrominance;
	ParseIntraBlock(4, 1);
	ParseIntraBlock(5, 2);

	if (my < pictureHeight) MotionCompensation(mx, my, FALSE, FALSE);
	}

//
//  Parse Pattern Macroblock
//

void SliceParser::ParsePatternMacroblock(int mx, int my)
	{
//	DP("Pattern MB");
	bool hasYDCT, hasUVDCT;

	ClearBlocks();

	hasYDCT = FALSE;
	hasUVDCT = FALSE;

	if ((codedBlockPattern & 0x3c) != 0)
		{
		intraDCTable = &dctDCSizeLuminance;
		if (codedBlockPattern & 0x20) ParseNonIntraBlock(0, 0);
		if (codedBlockPattern & 0x10) ParseNonIntraBlock(1, 0);
		if (codedBlockPattern & 0x08) ParseNonIntraBlock(2, 0);
		if (codedBlockPattern & 0x04) ParseNonIntraBlock(3, 0);
		hasYDCT = TRUE;
		}
	if ((codedBlockPattern & 0x03) != 0)
		{
		intraDCTable = &dctDCSizeChrominance;
		if (codedBlockPattern & 0x02) ParseNonIntraBlock(4, 1);
		if (codedBlockPattern & 0x01) ParseNonIntraBlock(5, 2);
		hasUVDCT = TRUE;
		}

	CalcMotionComponents();


	if (my < pictureHeight) MotionCompensation(mx, my, !hasYDCT, !hasUVDCT);
	}

//
//  Parse Skipped Macroblock
//

void SliceParser::ParseSkippedMacroblock(int mx, int my)
	{
//	DP("Skipped MB");
	codedBlockPattern = 0x0;

	CalcMotionComponents();

	if (my < pictureHeight) MotionCompensation(mx, my, TRUE, TRUE);
	}

//
//  Parse Macroblock
//

__forceinline bool SliceParser::ParseMacroblock(void)
	{
	int macroblockAddressIncrement;

	macroblockAddressIncrement = GetMacroblockAddressIncrement(&macroBlockAddressIncrement) - macroblockAddressDecrement;
	if (macroblockAddressIncrement < 0)
		{
		macroblockAddress += macroblockAddressIncrement;
		macroblockX = macroblockAddress % macroblocksPerRow;
		macroblockY = macroblockAddress / macroblocksPerRow;
		macroblockAddressIncrement = 0;
		errorInPicture = true;
		DP("Error in macroblock address calculation");
		}
	macroblockAddressDecrement = 1;

	SkipMacroblocks(macroblockAddressIncrement);

#if DUMP_MOTION_VECTORS
	if (mfile) fprintf(mfile, "MX %3d MY %3d : ", macroblockX, macroblockY);
#endif

	if (macroblockAddress < finalMacroblockAddress)
		{
		ParseMacroblockModes();

		if (IS_MACROBLOCK_QUANT(macroblockType))
			{
			SetQuantScaleCode(bitStream->GetBits(5));
			}

#if DUMP_MOTION_VECTORS
		if (mfile) fprintf(mfile, "MD %02x Q %2d ", macroblockType, quantizerScaleCode);
#endif


		if (IS_MACROBLOCK_INTRA(macroblockType))
			{
			//
			//  Intra Macroblock
			//

#if DUMP_MOTION_VECTORS
			if (mfile) fprintf(mfile, "INTRA ");
#endif

			if (PF_CONCEALMENT_MOTION_VECTORS(pictureFlags))
				{
#if DUMP_MOTION_VECTORS
				if (mfile) fprintf(mfile, "CMV ");
#endif

				macroblockType |= MF_MOTION_FORWARD;

				if (pictureStructure == PS_FRAME_PICTURE)
					predictionType = PT_FRAME_BASED;
				else
					predictionType = PT_FIELD_BASED;

				motionVectorCount = 1;
				ParseMotionVectors(0);
				bitStream->Advance(1);
				CalcMotionComponents();
				}
			else
				{
				ResetPMV();
				}

			ParseIntraMacroblock(macroblockX * 16, macroblockY * 16);
			}
		else if (pictureCodingType != INTRA_CODED)
			{
			//
			//  Non Intra Macroblock
			//

#if DUMP_MOTION_VECTORS
			if (mfile) fprintf(mfile, "NON INTRA ");
#endif

			if (IS_MACROBLOCK_MOTION_FORWARD(macroblockType))
				ParseMotionVectors(0);

			if (IS_MACROBLOCK_MOTION_BACKWARD(macroblockType))
				ParseMotionVectors(1);

			ResetPredictors();

			if (IS_MACROBLOCK_PATTERN(macroblockType))
				{
				ParseCodedBlockPattern();

#if DUMP_MOTION_VECTORS
			if (mfile) fprintf(mfile, "CBP %02x ", codedBlockPattern);
#endif

				ParsePatternMacroblock(macroblockX * 16, macroblockY * 16);
				}
			else
				{
				ParseSkippedMacroblock(macroblockX * 16, macroblockY * 16);
				}

			}
		else
			return false;
		}
	else
		return false;

#if DUMP_MOTION_VECTORS
	if (mfile) fprintf(mfile, "\n");
#endif

	macroblockAddress++;
	macroblockX++;
	if (macroblockX == macroblocksPerRow)
		{
		macroblockX = 0;
		macroblockY ++;
		}

	return true;
	}

//
//  Parse MPEG-2 Slice
//

void SliceParser::ParseMPEG2Slice(void)
	{
	DWORD startCode;
   int newSliceVerticalPosition;
	bool errorInSlice;

	frameIsComplete = false;
	errorInSlice = false;

	startCode = bitStream->GetBits(32);
	newSliceVerticalPosition = (startCode & 0xff) - 1;
#if DUMP_MOTION_VECTORS
	if (mfile) fprintf(mfile, "%08lx : ", (DWORD)(bitStream->CurrentLocation()));
	if (mfile) fprintf(mfile, "Slice %d\n", newSliceVerticalPosition);
#endif
	if (newSliceVerticalPosition > sliceVerticalPosition)
		{
		sliceVerticalPosition = newSliceVerticalPosition;

		macroblockAddress = macroblocksPerRow * sliceVerticalPosition;
		macroblockX = 0;
		macroblockY = sliceVerticalPosition;

		directAccess = FALSE;
		if (macroblockY * 16 < pictureHeight)
			{
			if (display)
				{
				directAccess = AttemptDirectAccess();
				}

			macroblockAddressDecrement = 1;

			for(;;)
				{
				numFLines = 8;

				ResetPredictors();
				ResetPMV();

				SetQuantScaleCode(bitStream->GetBits(5));

				if (bitStream->PeekBits(1))
					{
					bitStream->GetBits(9);
					while (bitStream->PeekBits(1))
						{
						bitStream->GetBits(9);
						}
					}
				bitStream->GetBit();

				do	{
	#if DUMP_MOTION_VECTORS
					if (mfile) fprintf(mfile, "%08lx : ", (DWORD)(bitStream->CurrentLocation()));
	#endif
					if (!ParseMacroblock()) errorInSlice = true;
					} while (!errorInSlice && macroblockAddress < finalMacroblockAddress && bitStream->PeekBits(23) != 0);

				bitStream->NextStartCode();

				if (errorInSlice)
					{
					errorInPicture = true;
					}

				if (!errorInSlice && bitStream->PeekBits(32) == startCode)
					{
					bitStream->Advance(32);
					macroblockAddressDecrement = macroblockX + 1;
					}
				else
					break;
				}
			}
		else
			errorInPicture = true;

		if (display)
			{
			if (directAccess)
				DoneDirectAccess();
			else if (stripeAccess && !errorInPicture)
				display->CopyStripe(sliceVerticalPosition * 16, pictureStructure);
			}
		}
	else
		{
		errorInPicture = true;
		if (startCode >= 0x00000101 && startCode <= 0x000001a0)
			{
			bitStream->Advance(8);
			bitStream->NextStartCode();
			}
		}

	if (!errorInPicture && macroblockAddress == finalMacroblockAddress)
		{
		if (!bitStream->IsBufferUnderflow())
			{
			MarkFrameComplete();
			frameIsComplete = true;
			}
		}
	}

void SliceParser::MarkFrameComplete(void)
	{
	if (display) display->MarkFrameComplete();
	}

void SliceParser::ParseMPEG1Slice(void)
	{
	DWORD startCode;

	frameIsComplete = false;
	errorInSlice = false;

	startCode = bitStream->GetBits(32);
	sliceVerticalPosition = (startCode & 0xff) - 1;

	if (sliceVerticalPosition < stopMacroblockY)
		{
		macroblockAddress = macroblocksPerRow * macroblockY + macroblockX;
		macroblockAddressDecrement = macroblockAddress + 1 - macroblocksPerRow * sliceVerticalPosition;

		directAccess = FALSE;

		numFLines = 8;

		ResetPredictors();
		ResetPMV();

		SetQuantScaleCode(bitStream->GetBits(5));

		if (bitStream->PeekBits(1))
			{
			bitStream->GetBits(9);
			while (bitStream->PeekBits(1))
				{
				bitStream->GetBits(9);
				}
			}
		bitStream->GetBit();

		do	{
			if (!ParseMacroblock()) errorInSlice = true;
			} while (!errorInSlice && macroblockAddress < finalMacroblockAddress && bitStream->PeekBits(23) != 0);

		bitStream->NextStartCode();

		if (errorInSlice)
			{
			errorInPicture = true;
			}

		if (!errorInPicture && macroblockAddress == finalMacroblockAddress)
			{
			if (!bitStream->IsBufferUnderflow())
				{
				MarkFrameComplete();
				frameIsComplete = true;
				}
			}
		}
	else
		{
		bitStream->Advance(8);
		bitStream->NextStartCode();

		errorInPicture = true;
		}
	}

void SliceParser::SetFCodes(BYTE fc00, BYTE fc01, BYTE fc10, BYTE fc11)
	{
	fcodes[0][0] = fc00;
	fcodes[0][1] = fc01;
	fcodes[1][0] = fc10;
	fcodes[1][1] = fc11;
	ifcodes[0][0] = 9-fc00;
	ifcodes[0][1] = 9-fc01;
	ifcodes[1][0] = 9-fc10;
	ifcodes[1][1] = 9-fc11;
	ffcodes[0][0] = 1 << (fc00 + 3);
	ffcodes[0][1] = 1 << (fc01 + 3);
	ffcodes[1][0] = 1 << (fc10 + 3);
	ffcodes[1][1] = 1 << (fc11 + 3);
	}

void SliceParser::SetPictureParameters(BYTE intraDCPrecision,
		                    PictureStructure pictureStructure,
								  BYTE pictureFlags)
	{
	this->intraDCPrecision = intraDCPrecision;
	this->pictureStructure = pictureStructure;
	this->pictureFlags = pictureFlags;

	if (PF_INTRA_VLC_FORMAT(pictureFlags))
		intraNonDCTable = &dctCoefficientOne;
	else
		intraNonDCTable = &dctCoefficientZero;

	if (PF_ALTERNATE_SCAN(pictureFlags))
		scanTable = inverseScanTable[1];
	else
		scanTable = inverseScanTable[0];

	if (pictureStructure != PS_FRAME_PICTURE)
		{
		switch (pictureCodingType)
			{
			case INTRA_CODED:
				macroblockTypeTable = &iFieldMacroBlockType;
				break;
			case PREDICTIVE_CODED:
				macroblockTypeTable = &pFieldMacroBlockType;
				break;
			case BIDIRECTIONAL_PREDICTIVE_CODED:
				macroblockTypeTable = &bFieldMacroBlockType;
				break;
			}
		}
	else if (PF_FRAME_PRED_FRAME_DCT(pictureFlags))
		{
		switch (pictureCodingType)
			{
			case INTRA_CODED:
				macroblockTypeTable = &iFrameMacroBlockTypeFPFD;
				break;
			case PREDICTIVE_CODED:
				macroblockTypeTable = &pFrameMacroBlockTypeFPFD;
				break;
			case BIDIRECTIONAL_PREDICTIVE_CODED:
				macroblockTypeTable = &bFrameMacroBlockTypeFPFD;
				break;
			}
		}
	else
		{
		switch (pictureCodingType)
			{
			case INTRA_CODED:
				macroblockTypeTable = &iFrameMacroBlockType;
				break;
			case PREDICTIVE_CODED:
				macroblockTypeTable = &pFrameMacroBlockType;
				break;
			case BIDIRECTIONAL_PREDICTIVE_CODED:
				macroblockTypeTable = &bFrameMacroBlockType;
				break;
			}
		}

	dcLumaPredictorInitializer = brightness << intraDCPrecision;
	dcChromaUPredictorInitializer = uBrightness << intraDCPrecision;
	dcChromaVPredictorInitializer = vBrightness << intraDCPrecision;

	sliceVerticalPosition = -1;
	}

void SliceParser::SetCodingType(PictureCodingType pictureCodingType)
	{
	this->pictureCodingType = pictureCodingType;
	}

void SliceParser::SetForwardVector(bool fullPel, BYTE fcode)
	{
	this->forwardFullPel = fullPel;
	this->forwardFCode = fcode;
	fcodes [0][0] = fcodes [0][1] = fcode;
	ifcodes[0][0] = ifcodes[0][1] = 9-fcode;
	ffcodes[0][0] = ffcodes[0][1] = 1 << (fcode + 3);
	}

void SliceParser::SetBackwardVector(bool fullPel, BYTE fcode)
	{
	this->backwardFullPel = fullPel;
	this->backwardFCode = fcode;
	fcodes [1][0] = fcodes [1][1] = fcode;
	ifcodes[1][0] = ifcodes[1][1] = 9-fcode;
	ffcodes[1][0] = ffcodes[1][1] = 1 << (fcode + 3);
	}

void SliceParser::SetImageSize(int width, int height)
	{
	this->width = width;
	this->height = height;
	this->height16 = height - 16;

	macroblocksPerRow = width / 16;
	}

void SliceParser::SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix)
	{
	int i;

	for(i=0; i<64; i++)
		{
		quantMatrix[type][InverseScanTable[0][i]] = matrix[i];
		}
	}

bool SliceParser::BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp)
	{
#if DUMP_MOTION_VECTORS
	if (mfile)
		{
		DumpMVStatistic();
		}
	if (mfile) fclose(mfile);
	mfile = NULL;
	char fname[100];
	wsprintf(fname, "c:\\mvectors\\frame%03d.txt", frameCount);
	frameCount++;
	mfile = fopen(fname, "w");
	fprintf(mfile, "Coding Type %d Pred %08lx Succ %08lx Curr %08lx PF %08lx\n",
		pictureCodingType, pred, succ, curr, pictureFlags);
#endif

	frameIsComplete = false;
	errorInPicture = false;

	if (display) display->MarkFrameIncomplete();

	predFrame = pred;
	succFrame = succ;
	curFrame = curr;
	this->display = display;

	predFields[0] = predFrame;
	predFields[1] = predFrame;

	motionVerticalFieldSelect[0][0] = 0;
	motionVerticalFieldSelect[0][1] = 0;
	motionVerticalFieldSelect[1][0] = 0;
	motionVerticalFieldSelect[1][1] = 0;

	currentFieldID = (pictureStructure == PS_BOTTOM_FIELD) ? 1 : 0;

	if (display && vcs == VCS_MPEG_2)
		stripeAccess = display->BeginStripeAccess(curr, 16, pictureStructure);
	else
		stripeAccess = FALSE;

	pictureHeight = (pictureStructure == PS_FRAME_PICTURE) ? height : height >> 1;
	macroblockX = 0;
	macroblockY = 0;
	stopMacroblockY = pictureHeight >> 4;
	finalMacroblockAddress = macroblocksPerRow * stopMacroblockY;

	return TRUE;
	}

void SliceParser::DoneFrameDecoding(void)
	{
	if (stripeAccess)
		{
		display->DoneStripeAccess();
		}
	else if (display)
		{
		display->CopyFrame(curFrame);
		}

#if DUMP_MOTION_VECTORS
	DumpMVStatistic();
	if (mfile) fclose(mfile);
	mfile = NULL;
#endif
	}

void SliceParser::DoneFirstFieldDecoding(void)
	{
	}

void SliceParser::BeginSecondFieldDecoding(void)
	{
	currentFieldID = (pictureStructure == PS_BOTTOM_FIELD) ? 1 : 0;

	if (pictureCodingType == PREDICTIVE_CODED)
		{
		predFields[currentFieldID] = predFrame;
		predFields[1-currentFieldID] = curFrame;
		}

#if DUMP_MOTION_VECTORS
	if (mfile) fprintf(mfile, "Coding Type %d PF %08lx\n",
		pictureCodingType, pictureFlags);
#endif
	}

bool SliceParser::SuspendFrameDecoding(void)
	{
	if (stripeAccess) display->DoneStripeAccess();

	return TRUE;
	}

void SliceParser::ResumeFrameDecoding(void)
	{
	if (stripeAccess) display->BeginStripeAccess(curFrame, 16, pictureStructure);
	}
/*
FrameStore * SliceParser::AllocFrameStore(PictureCodingType type, bool stripeOptimized)
	{
	return new FrameStore(width, height, stripeOptimized);
	}
*/
void SliceParser::Parse(void)
	{
	if (vcs == VCS_MPEG_2)
		ParseMPEG2Slice();
	else
		ParseMPEG1Slice();
	}

SliceParser::SliceParser(VideoBitStream * bitStream)
	{
	this->bitStream = bitStream;

	InitInverseScanTable();
	inverseScanTable[0] = &(InverseScanTable[0][0]);
	inverseScanTable[1] = &(InverseScanTable[1][0]);

	brightness = 128;
	uBrightness = 128;
	vBrightness = 128;
	}

void SliceParser::SetQuantScaleCode(int code)
	{
	quantizerScaleCode = (BYTE)code;
	}

void SliceParser::SetVideoCodingStandard(VideoCodingStandard vcs)
	{
	this->vcs = vcs;
	}


