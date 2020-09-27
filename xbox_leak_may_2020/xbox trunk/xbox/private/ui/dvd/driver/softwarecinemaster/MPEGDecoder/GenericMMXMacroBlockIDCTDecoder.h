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

#ifndef GENERICMMXMACROBLOCKIDCTDECODER_H
#define GENERICMMXMACROBLOCKIDCTDECODER_H

#include "SliceParser.h"
#include "IDCTTables.h"

extern MMXShort8	tmmxf[32];

class GenericMMXMacroBlockIDCTDecoder
	{
	protected:
		VideoBitStream		*	bitStream;

		bool						hurryUp;
		HuffmanTable		*	splitIntraDCTable[3];

		DCTMulTable			*	dctMulTable[3];

		VideoCodingStandard	vcs;
		int						intraDCPrecision;
		HuffmanTable		*	intraNonDCTable;

	protected:

		void InverseDCT(short * mb, int stride, DCTMulTable * table, int numLines, int numColumns);
		void InverseBYTEIntraDCT(BYTE * mb, int stride, DCTMulTable * table, int numLines, int numColumns);

		int ParseIntraBlockMatrix(int yuvType, int & dcPredictor, int * matrix);
		int ParseNonIntraBlockMatrix(int * matrix);

	public:
		GenericMMXMacroBlockIDCTDecoder(void);
		virtual ~GenericMMXMacroBlockIDCTDecoder(void);

		virtual void ParseIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, short * mb, int stride);
		virtual void ParseNonIntraBlock(int num, int yuvType, int * matrix, short * mb, int stride);
		virtual void ParseNonIntraBlocks(int codedBlockPattern, short ** yuvp, int * yuvd, int * matrix, int * offset);
		virtual void ParseBaseNonIntraBlocks(int codedBlockPattern, short * yuvbp, int * yuvbo, int * yuvd, int * matrix);

		virtual void ParseBYTEIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, BYTE * mb, int stride);

		void SetBitStream(VideoBitStream * bitStream) {this->bitStream = bitStream;}

		void SetDCTMulTable(DCTMulTable * lumaDCTMulTable, DCTMulTable * chromaDCTMulTable)
			{
			dctMulTable[0] = lumaDCTMulTable;
			dctMulTable[1] = chromaDCTMulTable;
			dctMulTable[2] = chromaDCTMulTable;
			}

		void SetPictureParameters(bool hurryUp,
										  VideoCodingStandard vcs,
										  int intraDCPrecision,
		                          HuffmanTable * intraNonDCTable);
	};

class GenericXMMXMacroBlockIDCTDecoder : public GenericMMXMacroBlockIDCTDecoder
	{
	protected:
		void InverseDCT(short * mb, int stride, DCTMulTable * table, int numLines, int numColumns);
		void InverseBYTEIntraDCT(BYTE * mb, int stride, DCTMulTable * table, int numLines, int numColumns);

	public:
		GenericXMMXMacroBlockIDCTDecoder(void);

		void ParseIntraBlock(int num, int yuvType, int & dcPredictor, int * matrix, short * mb, int stride);
		void ParseNonIntraBlock(int num, int yuvType, int * matrix, short * mb, int stride);
		void ParseNonIntraBlocks(int codedBlockPattern, short ** yuvp, int * yuvd, int * matrix, int * offset);
	};


#endif
