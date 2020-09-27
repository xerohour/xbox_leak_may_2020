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

#ifndef GENERICMMXSLICEDECODER_H
#define GENERICMMXSLICEDECODER_H

#include "SliceParser.h"
#include "IDCTTables.h"
#include "GenericMMXMacroBlockIDCTDecoder.h"

extern MMXShort8	tmmxf[32];

//
//  The mother of all slice decoders
//

class GenericMMXSliceDecoder : public SliceParser
	{
	protected:
		static unsigned char		AlternateInverseScanTable[2][64];
		static unsigned short	AlternateInverseScanTableX[2][64];

		PadOnQWord				pad00;

		int						intraQuant[4][64];
		int						nonIntraQuant[4][64];

		unsigned short			orgQuantX[2][2][2][64];
		unsigned short		*	orgQuant[2];

		int						lastIntraQuantScaleCode[4];
		int						lastNonIntraQuantScaleCode[4];

		bool						hurryUp;

		short					*	yuvp[2][8];
		int						yuvd[2][4];

		int						quantizerScaleValue;
		unsigned short		*	scanTableX;

		int						numFColumns;

		PadOnQWord				pad01;

		DCTMulTable				lumaDCTMulTable, chromaDCTMulTable;

		int						contrast;
		int						saturation;
		int						xbrightness, xubrightness, xvbrightness;

		GenericMMXMacroBlockIDCTDecoder * idctDecoder;
	public:
		GenericMMXSliceDecoder(VideoBitStream * bitStream, GenericMMXMacroBlockIDCTDecoder * idctDecoder = NULL);
		~GenericMMXSliceDecoder(void);

		bool BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp);
		void DoneFrameDecoding(void);

		void BeginSecondFieldDecoding(void);

		void SetBrightness(int brightness, int uBrightness, int vBrightness);
		void SetContrast(int contrast);
		void SetSaturation(int saturation);

		void ParseIntraBlock(int num, int yuvType);
		virtual void ParseIntraBlocks(void);

		void ParseNonIntraBlock(int num, int yuvType);
		virtual void ParseNonIntraBlocks(void);
		void ParsePatternMacroblock(int mx, int my);

		void SetQuantScaleCode(int code);
		void SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix);

		void SetPictureParameters(BYTE intraDCPrecision,
		                          PictureStructure pictureStructure,
									  	  BYTE pictureFlags);


		void SetBitStream(VideoBitStream * bitStream) {this->bitStream = bitStream; idctDecoder->SetBitStream(bitStream);}
	};

//
//  Generic Packed Intra MMX Slice Decoder
//

class GenericPackedIntraMMXSliceDecoder : public GenericMMXSliceDecoder
	{
	protected:
		BYTE					*	iyuvbp[2][8];
		int						iyuvbd[2][4];
		int						iyuvbpoffset[4];
	public:
		GenericPackedIntraMMXSliceDecoder(VideoBitStream * bitStream, GenericMMXMacroBlockIDCTDecoder * idctDecoder = NULL);

		void ParseIntraBlock(int num, int yuvType);
		void ParseIntraBlocks(void);
	};


#endif
