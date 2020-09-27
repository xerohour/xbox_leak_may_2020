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

#ifndef SPUDISPLAYBUFFER_H
#define SPUDISPLAYBUFFER_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"

struct SPUColorEntry
	{
	BYTE y, u, v, t;
	};

class SPUDisplayBuffer
	{
	public:
		int				left, top, width, height;
		bool				changed;
		int				nonTransparentTop, nonTransparentHeight;
		DWORD				mainNonZeroLinesMap[18], extNonZeroLinesMap[18];
		DWORD			*	nonZeroLinesMap;

		virtual ~SPUDisplayBuffer(void) {}

		virtual void SetColorTable(SPUColorEntry * colorTable) = 0;
		virtual void SetImage(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height, DWORD color, BYTE * colcon, DWORD uniqueID) = 0;
		virtual void SetButton(int buttonLeft, int buttonTop, int buttonWidth, int buttonHeight, DWORD color) = 0;
		virtual void PerformButtonChange(void) {}
		virtual void ReleaseBuffer(void) {}
		virtual void RestoreBuffer(void) {}

		virtual void CalculateNonTransparentRegion(void);
	};

class SPUDisplayBufferFactory
	{
	public:
		virtual SPUDisplayBuffer * CreateSPUDisplayBuffer(void) = 0;
	};

class DecodedSPUDisplayBuffer : public SPUDisplayBuffer
	{
	protected:
		virtual void MarkAlternateColorRect(int aleft, int atop, int awidth, int aheight, int val) = 0;
		virtual void RemoveAlternateColors(void) = 0;
		virtual void DecodeRectangle(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height) = 0;
		virtual void RebuildPalette(void);

		void DecodeFieldRectangle4BPP(BYTE * map, BYTE * limit, int preLeft, int width, bool odd, int & topEmpty, int & bottomEmpty, DWORD * nonZeroLines);
		void DecodeFieldRectangle2BPP(BYTE * map, BYTE * limit, int preLeft, int width, bool odd, int & topEmpty, int & bottomEmpty, DWORD * nonZeroLines);

		void CalculateNonTransparentRegion(void);

		DWORD				halfNonZeroLinesMap[18];
		DWORD				colorSet[4];

		bool				buttonChangePending;
	public:
		int				usedColorSets;
		int				bytesPerRow;
		BYTE			*	image;
		BYTE			*	halfImage;

		int				buttonLeft, buttonTop, buttonWidth, buttonHeight;
		DWORD				buttonColor;

		int				topEmpty, bottomEmpty;

		PadOnQWord		pad0;

		SPUColorEntry	nativeColorTable[16];
		SPUColorEntry	translatedColorTable[16];

		DWORD				uniqueID;
		int				visibleTop, visibleBottom;


		DecodedSPUDisplayBuffer(void);
		~DecodedSPUDisplayBuffer(void);

		void SetColorTable(SPUColorEntry * colorTable);
		void SetImage(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height, DWORD color, BYTE * colcon, DWORD uniqueID);
		void SetButton(int buttonLeft, int buttonTop, int buttonWidth, int buttonHeight, DWORD color);
		void PerformButtonChange(void);
	};

class DecodedSPUDisplayBuffer4BPP : public DecodedSPUDisplayBuffer
	{
	protected:
		void MarkAlternateColorRect(int aleft, int atop, int awidth, int aheight, int val);
		void RemoveAlternateColors(void);
		void DecodeRectangle(BYTE * odd, BYTE * even, BYTE * limit, int left, int top, int width, int height);
	public:
		int	minAlternateTop, maxAlternateBottom;
	};


class FullWidthMMXSPUDisplayBuffer : public DecodedSPUDisplayBuffer4BPP
	{
	protected:
		void RebuildPalette(void);
	public:
		PadOnQWord		pad1;

		DDWORD		colorLookup[256];
		DDWORD		transparencyLookup[256];
		DDWORD		hcolorLookup1[256];
		DDWORD		htransparencyLookup1[256];
		DDWORD		hcolorLookup2[256];
		DDWORD		htransparencyLookup2[256];
		DDWORD		byteColorTransparencyLookup[256];

		void GetDisplayParameters(int decodeWidth, int decodeHeight,
			                       int & xosdstart, int & xosdstop,
			                       int & yosdstart, int & yosdstop,
										  DWORD * & nonZeroLinesMap, int & yoffset,
			                       BYTE * & po, int & obpr);
	};


#endif
