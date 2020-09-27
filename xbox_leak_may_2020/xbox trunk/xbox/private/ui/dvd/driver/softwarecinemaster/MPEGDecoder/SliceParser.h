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

#ifndef SLICEPARSER_H
#define SLICEPARSER_H

#include "PictureParser.h"
#include "Mp2HuffTables.h"

#define SPCCF_BRIGHTNESS					0x00000001l
#define SPCCF_CONTRAST						0x00000002l
#define SPCCF_HUE								0x00000004l
#define SPCCF_SATURATION					0x00000008l
#define SPCCF_SHARPNESS						0x00000010l
#define SPCCF_GAMMA							0x00000020l
#define SPCCF_RED_BRIGHTNESS				0x00001000l
#define SPCCF_BLUE_BRIGHTNESS				0x00002000l
#define SPCCF_GREEN_BRIGHTNESS			0x00004000l

class SliceParser : public GenericSliceParser
	{
	protected:
		PadOnQWord				pad0;

		static unsigned char	InverseScanTable[2][64];
		static const int		QuantizerScale[2][32];

		BYTE						fcodes[2][2];
		BYTE						intraDCPrecision;
		PictureStructure		pictureStructure;
		BYTE						pictureFlags;
		PictureCodingType		pictureCodingType;
		bool						forwardFullPel;
		BYTE						forwardFCode;
		bool						backwardFullPel;
		BYTE						backwardFCode;
		int						width, height;
		int						height16; // height - 16

		int						ffcodes[2][2];
		int						ifcodes[2][2];
		int						currentFieldID;
		int						stopMacroblockY;
		int						pictureHeight;

		QuantizationMatrix	quantMatrix[4];

		int						macroblockAddressDecrement;
		int						macroblocksPerRow;
		HuffmanTable		*	intraNonDCTable;
		unsigned char		*	scanTable;
		HuffmanTable		*	intraDCTable;
		unsigned char		*	inverseScanTable[2];

		HuffmanTable		*	macroblockTypeTable;

		BYTE						quantizerScaleCode;

		int						sliceVerticalPosition;
		int						macroblockAddress;
		int						macroblockX, macroblockY;
		int						macroblockType;
		bool						dctType;

		PredictionType			predictionType;
		int						motionVectorCount;
		int						dmv;

		int						motionVerticalFieldSelect[2][2];
		int						motionValue[2][2][2];

		int						codedBlockPattern;
		int						dcPredictor[3];
		int						dcLumaPredictorInitializer;
		int						dcChromaUPredictorInitializer;
		int						dcChromaVPredictorInitializer;

		int						numFLines;

		int						pmv[2][2][2];
		int						vectors[4][2][2];
		int						dmvector[2];

		FrameStore			*	predFrame;
		FrameStore			*	succFrame;
		FrameStore			*	curFrame;
		FrameStore			*	predFields[2];
		PictureDisplayBuffer * display;
		VideoBitStream		*	bitStream;

		bool						directAccess;
		bool						stripeAccess;

		int						finalMacroblockAddress;

		int						brightness, uBrightness, vBrightness;

		bool						frameIsComplete, errorInPicture, errorInSlice;

		VideoCodingStandard	vcs;
	private:
//		int	pad00;
	protected:

		virtual void MotionCompensation(int mx, int my, bool copyY, bool copyUV) = 0;
		virtual void ParseBlock(int n, int yuvType) {}
		virtual void ParseIntraBlock(int n, int yuvType) {ParseBlock(n, yuvType);}
		virtual void ParseNonIntraBlock(int n, int yuvType) {ParseBlock(n, yuvType);}
		virtual void ClearBlocks(void) = 0;
		virtual void SetQuantScaleCode(int code);
		virtual void ExecuteSkippedMacroblock(int mx, int my);

		virtual bool AttemptDirectAccess(void) {return FALSE;}
		virtual void DoneDirectAccess(void) {}

		void InitInverseScanTable(void);

		void CalcMotionComponents(void);
		void SkipMacroblocks(int num);
		void ResetPredictors(void);
		void ResetPMV(void);
		void ParseMacroblockModes(void);
		int GetMacroblockAddressIncrement(HuffmanTable * table);
		int GetMotionVector(int fcode, HuffmanTable * table);

		void ParseCodedBlockPattern(void);
		void ParseMotionVector(int r, int s);
		void ParseDMVMotionVector(int s);
		void ParseMotionVectors(int s);

		bool ParseMacroblock(void);

		void ParseMPEG2Slice(void);
		void ParseMPEG1Slice(void);

		virtual void ParseIntraMacroblock(int mx, int my);
		virtual void ParsePatternMacroblock(int mx, int my);
		virtual void ParseSkippedMacroblock(int mx, int my);

		virtual void MarkFrameComplete(void);
	public:
		void SetFCodes(BYTE fc00, BYTE fc01, BYTE fc10, BYTE fc11);
		void SetPictureParameters(BYTE intraDCPrecision,
		                          PictureStructure pictureStructure,
									  	  BYTE pictureFlags);
		void SetCodingType(PictureCodingType pictureCodingType);
		void SetForwardVector(bool fullPel, BYTE fcode);
		void SetBackwardVector(bool fullPel, BYTE fcode);
		void SetImageSize(int width, int height);
		void SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix);
		void SetVideoCodingStandard(VideoCodingStandard vcd);

		bool BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp);
		bool SuspendFrameDecoding(void);
		void DoneFirstFieldDecoding(void);
		void BeginSecondFieldDecoding(void);
		void ResumeFrameDecoding(void);
		void DoneFrameDecoding(void);

		FrameStore * AllocFrameStore(PictureCodingType type, bool stripeOptimized) = 0;

		void Parse(void);

		virtual void SetBitStream(VideoBitStream * bitStream) {this->bitStream = bitStream;}

		SliceParser(VideoBitStream * bitStream);

		virtual void SetBrightness(int brightness, int uBrightness, int vBrightness)
			{
			this->brightness = brightness;
			this->uBrightness = uBrightness;
			this->vBrightness = vBrightness;
			}

		virtual void SetContrast(int contrast) {}

		virtual void SetSaturation(int saturation) {}

		virtual DWORD QueryColorControl(void) {return SPCCF_BRIGHTNESS |
				                                        SPCCF_RED_BRIGHTNESS |
															       SPCCF_BLUE_BRIGHTNESS |
																	 SPCCF_GREEN_BRIGHTNESS;}
	};

#endif
