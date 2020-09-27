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

#ifndef MP2HUFFTABLES_H
#define MP2HUFFTABLES_H

#include "huffdec.h"

#define PICTURE_START_CODE		0x00000100
#define USER_DATA_START_CODE	0x000001b2
#define SEQUENCE_HEADER_CODE	0x000001b3
#define SEQUENCE_ERROR_CODE	0x000001b4
#define EXTENSION_START_CODE	0x000001b5
#define SEQUENCE_END_CODE		0x000001b7
#define GROUP_START_CODE		0x000001b8

#define IS_SLICE_HEADER(x)		(x >= 0x00000101 && x <= 0x000001af)

#define SEQUENCE_EXTENSION_ID							1
#define SEQUENCE_DISPLAY_EXTENSION_ID				2
#define QUANT_MATRIX_EXTENSION_ID					3
#define COPYRIGHT_EXTENSION_ID						4
#define SEQUENCE_SCALABLE_EXTENSION_ID				5

#define PICTURE_DISPLAY_EXTENSION_ID				7
#define PICTURE_CODING_EXTENSION_ID					8
#define PICTURE_SPATIAL_EXTENSION_ID				9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID	10

enum PredictionType
	{
	PT_FIELD_BASED,
	PT_FRAME_BASED,
	PT_DUAL_PRIME,
	PT_16x8MC
	};

#define RL(run, level)	((run << 12) | (level & 0xfff))
#define RLF(run, level)	((run << 12) | (level & 0xfff))
#define RLRUN(x)		(int)((unsigned)x >> 20)
#define RLLEVEL(x)	((int)x << 12 >> 20)

#define MF_DUAL_PRIME_PREDICTION				0x200
#define MF_16X8_PREDICTION						0x100
#define MF_ALTERNATE_DCT						0x080
#define MF_FIELD_PREDICTION					0x040
#define MF_FRAME_PREDICTION					0x020
#define MF_QUANT									0x010
#define MF_MOTION_FORWARD						0x008
#define MF_MOTION_BACKWARD						0x004
#define MF_PATTERN								0x002
#define MF_INTRA									0x001

#define IS_MACROBLOCK_ALTERNATE_DCT(x)				((x & MF_ALTERNATE_DCT) != 0)
#define IS_MACROBLOCK_QUANT(x)						((x & MF_QUANT) != 0)
#define IS_MACROBLOCK_PATTERN(x)						((x & MF_PATTERN) != 0)
#define IS_MACROBLOCK_INTRA(x)						((x & MF_INTRA) != 0)
#define IS_MACROBLOCK_FIELD_PREDICTION(x)			((x & MF_FIELD_PREDICTION) != 0)
#define IS_MACROBLOCK_FRAME_PREDICTION(x)			((x & MF_FRAME_PREDICTION) != 0)
#define IS_MACROBLOCK_16X8_PREDICTION(x)			((x & MF_16X8_PREDICTION) != 0)
#define IS_MACROBLOCK_DUAL_PRIME_PREDICTION(x)	((x & MF_DUAL_PRIME_PREDICTION) != 0)
#define IS_MACROBLOCK_MOTION_FORWARD(x)			((x & MF_MOTION_FORWARD) != 0)
#define IS_MACROBLOCK_MOTION_BACKWARD(x)			((x & MF_MOTION_BACKWARD) != 0)
#define IS_MACROBLOCK_MOTION(x)						((x & (MF_MOTION_FORWARD | MF_MOTION_BACKWARD)) != 0)

#define MKMFMVCNT(t)	(IS_MACROBLOCK_MOTION(t) ? (IS_MACROBLOCK_FIELD_PREDICTION(t) ? 2 : 1) : 0)
#define MKMFPT(t) (IS_MACROBLOCK_FIELD_PREDICTION(t) ? PT_FIELD_BASED : (IS_MACROBLOCK_DUAL_PRIME_PREDICTION(t) ? PT_DUAL_PRIME : PT_FRAME_BASED))
#define MKMIMVCNT(t)	(IS_MACROBLOCK_MOTION(t) ? (IS_MACROBLOCK_16X8_PREDICTION(t) ? 2 : 1) : 0)
#define MKMIPT(t) (IS_MACROBLOCK_16X8_PREDICTION(t) ? PT_16x8MC : (IS_MACROBLOCK_DUAL_PRIME_PREDICTION(t) ? PT_DUAL_PRIME : PT_FIELD_BASED))

#define MKMF(flags)	flags | (MKMFMVCNT((flags)) << 12) | (MKMFPT((flags)) << 14)
#define MKMI(flags)	flags | (MKMIMVCNT((flags)) << 12) | (MKMIPT((flags)) << 14)
#define MACROBLOCK_MOTION_COUNT(x) ((x >> 12) & 3)
#define MACROBLOCK_PREDICT_TYPE(x) (PredictionType)((x >> 14) & 3)

extern HuffmanTable	macroBlockAddressIncrement;
extern HuffmanTable	iFrameMacroBlockType;
extern HuffmanTable	bFrameMacroBlockType;
extern HuffmanTable	pFrameMacroBlockType;

extern HuffmanTable	iFrameMacroBlockTypeFPFD;
extern HuffmanTable	bFrameMacroBlockTypeFPFD;
extern HuffmanTable	pFrameMacroBlockTypeFPFD;

extern HuffmanTable	iFieldMacroBlockType;
extern HuffmanTable	bFieldMacroBlockType;
extern HuffmanTable	pFieldMacroBlockType;

extern HuffmanTable	codedBlockPattern420;
extern HuffmanTable	motionCodes;
extern HuffmanTable	dmtVector;
extern HuffmanTable	dctDCSizeLuminance;
extern HuffmanTable	dctDCSizeChrominance;
extern HuffmanTable	dctCoefficientZeroFirst;
extern HuffmanTable	dctCoefficientZero;
extern HuffmanTable	dctCoefficientOne;


#endif
