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

#include "GenericMMXSliceDecoder.h"
#include "library\common\vddebug.h"

#pragma warning (disable : 4799)

MMXShort8	tmmxf[32];

////////////////////////////////////////////////////////////////////
//
//  Generic MMX Slice Decoder
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericMMXSliceDecoder::GenericMMXSliceDecoder(VideoBitStream * bitStream, GenericMMXMacroBlockIDCTDecoder * idctDecoder)
	: SliceParser(bitStream)
	{
	static unsigned char translate[8] = {0, 4, 2, 6, 1, 5, 3, 7};
	unsigned char v, u;

	int i, j;

	for(i=0; i<2; i++)
		{
		for(j=0; j<64; j++)
			{
			u = InverseScanTable[i][j];
			u = ((u >> 3) | (u << 3)) & 0x3f;
			v = (u & ~7) | translate[u & 7];
			AlternateInverseScanTable[i][j] = v;
			AlternateInverseScanTableX[i][j] = ((unsigned short)v << 8) | ((u & 7) + 1);
			}
		}

	inverseScanTable[0] = &(AlternateInverseScanTable[0][0]);
	inverseScanTable[1] = &(AlternateInverseScanTable[1][0]);

	if (idctDecoder)
		this->idctDecoder = idctDecoder;
	else
		this->idctDecoder = new GenericMMXMacroBlockIDCTDecoder();

	this->idctDecoder->SetBitStream(bitStream);

	xbrightness = 128;
	xubrightness = 128;
	xvbrightness = 128;
	contrast = 128;
	saturation = 128;

	__asm {emms}

	lumaDCTMulTable.CalculateTable(contrast);
	chromaDCTMulTable.CalculateTable(saturation);

	this->idctDecoder->SetDCTMulTable(&lumaDCTMulTable, &chromaDCTMulTable);
	}

//
//  Destructor
//

GenericMMXSliceDecoder::~GenericMMXSliceDecoder(void)
	{
	delete idctDecoder;
	}

//
//  Begin Frame Decoding
//

bool GenericMMXSliceDecoder::BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp)
	{
	this->hurryUp = hurryUp && pictureCodingType == BIDIRECTIONAL_PREDICTIVE_CODED;

	if (this->hurryUp)
		{
		if (PF_ALTERNATE_SCAN(pictureFlags))
			orgQuant[1] = orgQuantX[1][1][1];
		else
			orgQuant[1] = orgQuantX[1][0][1];
		}

	idctDecoder->SetPictureParameters(this->hurryUp, vcs, intraDCPrecision, intraNonDCTable);

	lastIntraQuantScaleCode[0] = -1;
	lastIntraQuantScaleCode[1] = -1;
	lastIntraQuantScaleCode[2] = -1;
	lastIntraQuantScaleCode[3] = -1;
	lastNonIntraQuantScaleCode[0] = -1;
	lastNonIntraQuantScaleCode[1] = -1;
	lastNonIntraQuantScaleCode[2] = -1;
	lastNonIntraQuantScaleCode[3] = -1;

	memclr128(tmmxf);

	curr->SetPrecision(8);
	if (SliceParser::BeginFrameDecoding(pred, succ, curr, display, this->hurryUp))
		{
		__asm {pxor	mm0, mm0}

		return TRUE;
		}
	else
		return FALSE;
	}

//
//  Done Frame Decoding
//

void GenericMMXSliceDecoder::DoneFrameDecoding(void)
	{
	SliceParser::DoneFrameDecoding();

	__asm {emms}
	}

void GenericMMXSliceDecoder::BeginSecondFieldDecoding(void)
	{
	idctDecoder->SetPictureParameters(this->hurryUp, vcs, intraDCPrecision, intraNonDCTable);

	SliceParser::BeginSecondFieldDecoding();
	}

//
//  Set Brightness
//

void GenericMMXSliceDecoder::SetBrightness(int brightness, int uBrightness, int vBrightness)
	{
	xbrightness = brightness;
	xubrightness = uBrightness;
	xvbrightness = vBrightness;

	SliceParser::SetBrightness(xbrightness + 64*128 / contrast - 64,
		                        xubrightness + 16384 / saturation - 128,
										xvbrightness + 16384 / saturation - 128);
	}

//
//  Set Contrast
//

void GenericMMXSliceDecoder::SetContrast(int contrast)
	{
	this->contrast = contrast;
	lumaDCTMulTable.CalculateTable(contrast);

	SetBrightness(xbrightness, xubrightness, xvbrightness);
	}

//
//  Set Saturation
//

void GenericMMXSliceDecoder::SetSaturation(int saturation)
	{
	this->saturation = saturation;
	chromaDCTMulTable.CalculateTable(saturation);

	SetBrightness(xbrightness, xubrightness, xvbrightness);
	}

//
//  Calculate Quantizer Matrix
//

static inline void CalcQuantMatrix(int quant, int * dst, unsigned short * src, unsigned short * scan)
	{
	__asm
		{
		movd			mm0, [quant]

		punpcklwd	mm0, mm0
		mov			eax, [src]

		punpckldq	mm0, mm0
		mov			ecx, 0

		mov			edx, [dst]
		mov			ebx, [scan]
loop1:
		movq			mm1, [eax+ecx]
		cmp			ecx, 128 - 16

		movq			mm4, [eax+8+ecx]
		pmullw		mm1, mm0

		movq			mm3, [ebx+ecx]
		pmullw		mm4, mm0

		movq			mm6, [ebx+8+ecx]

		paddb			mm3, mm3
		paddb			mm6, mm6

		movq			mm2, mm1
		punpcklwd	mm1, mm3

		punpckhwd	mm2, mm3
		movq			mm5, mm4

		movq			[edx+2*ecx], mm1
		punpcklwd	mm4, mm6

		movq			[edx+8+2*ecx], mm2
		punpckhwd	mm5, mm6

		movq			[edx+16+2*ecx], mm4

		movq			[edx+24+2*ecx], mm5

		lea			ecx, [ecx + 16]
		jne			loop1
		}
	}

//
//  Parse Intra Block
//

void GenericMMXSliceDecoder::ParseIntraBlock(int num, int yuvType)
	{
	int * matrix;

	matrix = intraQuant[quantizerScaleCode & 3];
	if (quantizerScaleCode != lastIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[0], scanTableX);
		}

	idctDecoder->ParseIntraBlock(num, yuvType, dcPredictor[yuvType], matrix, yuvp[dctType][num], yuvd[dctType][yuvType]);
	}

//
//  Parse Intra Blocks
//

void GenericMMXSliceDecoder::ParseIntraBlocks(void)
	{
	int i, t;
	static char tp[] = {0, 0, 0, 0, 1, 2};
	int * matrix;

	matrix = intraQuant[quantizerScaleCode & 3];
	if (quantizerScaleCode != lastIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[0], scanTableX);
		}

	for(i=0; i<6; i++)
		{
		t = tp[i];
		idctDecoder->ParseIntraBlock(i, t, dcPredictor[t], matrix, yuvp[dctType][i], yuvd[dctType][t]);
		}
	}

//
//  Parse Non Intra Block
//

void GenericMMXSliceDecoder::ParseNonIntraBlock(int num, int yuvType)
	{
	int * matrix;

	matrix = nonIntraQuant[quantizerScaleCode & 3];

	if (quantizerScaleCode != lastNonIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastNonIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[1], scanTableX);
		}

	idctDecoder->ParseNonIntraBlock(num, yuvType, matrix, yuvp[dctType][num], yuvd[dctType][yuvType]);
	}

static inline int DIV2(int x)
	{
	return (x - (x >> 31)) >> 1;
	}
//
//  Parse Non Intra Blocks
//

void GenericMMXSliceDecoder::ParseNonIntraBlocks(void)
	{
	int * matrix;
	int offset[3];

	matrix = nonIntraQuant[quantizerScaleCode & 3];

	if (quantizerScaleCode != lastNonIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastNonIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[1], scanTableX);
		}

	if ((vectors[0][0][0] & 1) && (vectors[0][0][1] & 1))
		offset[0] = -3;
	else
		offset[0] = 0;

	if ((DIV2(vectors[0][0][0]) & 1) && (DIV2(vectors[0][0][1]) & 1))
		offset[1] = offset[2] = -3;
	else
		offset[1] = offset[2] = 0;

	idctDecoder->ParseNonIntraBlocks(codedBlockPattern, yuvp[dctType], yuvd[dctType], matrix, offset);
	}

//
//  Parse Pattern Macroblock
//

void GenericMMXSliceDecoder::ParsePatternMacroblock(int mx, int my)
	{
	CalcMotionComponents();

	ClearBlocks();

	ParseNonIntraBlocks();

	MotionCompensation(mx, my, (codedBlockPattern & 0x3c) == 0, (codedBlockPattern & 0x03) == 0);
	}

//
//  Set Quantizer Scale Code
//

void GenericMMXSliceDecoder::SetQuantScaleCode(int code)
	{
	SliceParser::SetQuantScaleCode(code);

	if (!PF_Q_SCALE_TYPE(pictureFlags))
		quantizerScaleValue = QuantizerScale[0][quantizerScaleCode];
	else
		quantizerScaleValue = QuantizerScale[1][quantizerScaleCode];
	}

static const int scantrans[64] =
	{
	 0,  4,  1,  2,  5,  6, 20,  7,
	 8,  3, 10,  9, 19, 21, 22, 36,
	23, 24, 18, 17, 11, 12, 16, 26,
	25, 34, 37, 38, 52, 39, 40, 35,
	30, 27, 15, 13, 14, 28, 31, 42,
	41, 50, 53, 54, 51, 46, 43, 32,
	29, 33, 44, 47, 56, 55, 60, 57,
	48, 45, 49, 58, 61, 62, 59, 63
	};

//
//  Set Quantiation Matrix
//

void GenericMMXSliceDecoder::SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix)
	{
	int i;

	if (type < 2)
		{
		for(i=0; i<64; i++)
			{
			orgQuantX[0][0][type][i] = (BYTE)matrix[i];
			orgQuantX[0][1][type][scantrans[i]] = (BYTE)matrix[i];
			if (((InverseScanTable[0][i] & 7) >= 4) ||
				 ( InverseScanTable[0][i]      >= 32))
				{
				orgQuantX[1][0][type][i] = 0;
				}
			else
				{
				orgQuantX[1][0][type][i] = (BYTE)matrix[i];
				}
			orgQuantX[1][1][type][scantrans[i]] = (BYTE)matrix[i];
			}
		}

	lastIntraQuantScaleCode[0] = -1;
	lastIntraQuantScaleCode[1] = -1;
	lastIntraQuantScaleCode[2] = -1;
	lastIntraQuantScaleCode[3] = -1;
	lastNonIntraQuantScaleCode[0] = -1;
	lastNonIntraQuantScaleCode[1] = -1;
	lastNonIntraQuantScaleCode[2] = -1;
	lastNonIntraQuantScaleCode[3] = -1;
	}

//
//  Oops
//

unsigned char	GenericMMXSliceDecoder::AlternateInverseScanTable[2][64];
unsigned short	GenericMMXSliceDecoder::AlternateInverseScanTableX[2][64];

//
//  Set Picture Parameters
//

void GenericMMXSliceDecoder::SetPictureParameters(BYTE intraDCPrecision,
		                                            PictureStructure pictureStructure,
								                          BYTE pictureFlags)
	{
	SliceParser::SetPictureParameters(intraDCPrecision, pictureStructure, pictureFlags);

	if (PF_ALTERNATE_SCAN(pictureFlags))
		{
		scanTableX = AlternateInverseScanTableX[1];
		orgQuant[0] = orgQuantX[0][1][0];
		orgQuant[1] = orgQuantX[0][1][1];
		}
	else
		{
		scanTableX = AlternateInverseScanTableX[0];
		orgQuant[0] = orgQuantX[0][0][0];
		orgQuant[1] = orgQuantX[0][0][1];
		}
	}

////////////////////////////////////////////////////////////////////
//
//  Generic Packed Intra MMX Slice Decoder
//
////////////////////////////////////////////////////////////////////

//
//  Parse Intra Block
//

void GenericPackedIntraMMXSliceDecoder::ParseIntraBlock(int num, int yuvType)
	{
	int * matrix;

	matrix = intraQuant[quantizerScaleCode & 3];
	if (quantizerScaleCode != lastIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[0], scanTableX);
		}

	idctDecoder->ParseBYTEIntraBlock(num, yuvType, dcPredictor[yuvType], matrix, iyuvbp[dctType][num] + iyuvbpoffset[yuvType], iyuvbd[dctType][yuvType]);
	}

//
//  Parse Intra Blocks
//

void GenericPackedIntraMMXSliceDecoder::ParseIntraBlocks(void)
	{
	int i, t;
	static char tp[] = {0, 0, 0, 0, 1, 2};
	int * matrix;

	matrix = intraQuant[quantizerScaleCode & 3];
	if (quantizerScaleCode != lastIntraQuantScaleCode[quantizerScaleCode & 3])
		{
		lastIntraQuantScaleCode[quantizerScaleCode & 3] = quantizerScaleCode;

		CalcQuantMatrix(quantizerScaleValue, matrix, orgQuant[0], scanTableX);
		}

	for(i=0; i<6; i++)
		{
		t = tp[i];
		idctDecoder->ParseBYTEIntraBlock(i, t, dcPredictor[t], matrix, iyuvbp[dctType][i] + iyuvbpoffset[t], iyuvbd[dctType][t]);
		}
	}

//
//  Constructor
//

GenericPackedIntraMMXSliceDecoder::GenericPackedIntraMMXSliceDecoder(VideoBitStream * bitStream, GenericMMXMacroBlockIDCTDecoder * idctDecoder)
	: GenericMMXSliceDecoder(bitStream, idctDecoder)
	{
	iyuvbpoffset[0] = 0;
	iyuvbpoffset[1] = 0;
	iyuvbpoffset[2] = 0;
	}



