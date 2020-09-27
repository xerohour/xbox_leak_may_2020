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

////////////////////////////////////////////////////////////////////
//
//  DVDTime Class
//
////////////////////////////////////////////////////////////////////

#include "DVDTime.h"

//
//  Constructor
//

DVDTime::DVDTime(int hours, int minutes, int seconds, int frames, int frameRate)
	{
	if (frames >= frameRate)
		{
		seconds += frames / frameRate;
		frames %= frameRate;
		}

	while (frames < 0)
		{
		frames += frameRate;
		seconds -= 1;
		}

	if (seconds >= 60)
		{
		minutes += seconds / 60;
		seconds %= 60;
		}

	while (seconds < 0)
		{
		seconds += 60;
		minutes -= 1;
		}

	if (minutes >= 60)
		{
		hours += minutes / 60;
		minutes %= 60;
		}

	while (minutes < 0)
		{
		minutes += 60;
		hours -= 1;
		}

	if (hours < 0)
		{
		hours = minutes = seconds = frames = 0;
		}

	stamp = MKBF(28, 4, hours / 10) |
	        MKBF(24, 4, hours % 10) |
	        MKBF(20, 4, minutes / 10) |
	        MKBF(16, 4, minutes % 10) |
	        MKBF(12, 4, seconds / 10) |
	        MKBF(8, 4, seconds % 10) |
	        MKBF(6, 2, frameRate == 25 ? 1 : 3) |
	        MKBF(4, 2, frames / 10) |
	        MKBF(0, 4, frames % 10);
	}

//
//  Constructor from fraction of millisecs (meant e.g. for the 90 KHz time stamps on DVD)
//

DVDTime::DVDTime(int millisecs, int divider, int frameRate)
	{
	int hours = 0, minutes = 0, seconds = 0, frames;

	frames = ScaleDWord(millisecs, divider * 1000, frameRate);

	if (frames >= frameRate)
		{
		seconds += frames / frameRate;
		frames %= frameRate;
		}

	while (frames < 0)
		{
		frames += frameRate;
		seconds -= 1;
		}

	if (seconds >= 60)
		{
		minutes += seconds / 60;
		seconds %= 60;
		}

	while (seconds < 0)
		{
		seconds += 60;
		minutes -= 1;
		}

	if (minutes >= 60)
		{
		hours += minutes / 60;
		minutes %= 60;
		}

	while (minutes < 0)
		{
		minutes += 60;
		hours -= 1;
		}

	if (hours < 0)
		{
		hours = minutes = seconds = frames = 0;
		}

	stamp = MKBF(28, 4, hours / 10) |
	        MKBF(24, 4, hours % 10) |
	        MKBF(20, 4, minutes / 10) |
	        MKBF(16, 4, minutes % 10) |
	        MKBF(12, 4, seconds / 10) |
	        MKBF(8, 4, seconds % 10) |
	        MKBF(6, 2, frameRate == 25 ? 1 : 3) |
	        MKBF(4, 2, frames / 10) |
	        MKBF(0, 4, frames % 10);
	}

//
//  Operator +
//

DVDTime operator+ (const DVDTime u, const DVDTime v)
	{
	return DVDTime(u.Hours() + v.Hours(),
	               u.Minutes() + v.Minutes(),
	               u.Seconds() + v.Seconds(),
	               u.Frames() + v.Frames(),
	               u.FrameRate());
	}

//
//  Operator -
//

DVDTime operator- (const DVDTime u, const DVDTime v)
	{
	return DVDTime(u.Hours() - v.Hours(),
	               u.Minutes() - v.Minutes(),
	               u.Seconds() - v.Seconds(),
	               u.Frames() - v.Frames(),
	               u.FrameRate());
	}

//
//  Operator +=
//

DVDTime & DVDTime::operator+= (const DVDTime u)
	{
	*this = *this + u;
	return *this;
	}

//
//  Operator -=
//

DVDTime & DVDTime::operator-= (const DVDTime u)
	{
	*this = *this - u;
	return *this;
	}

//
//  Compare
//

int DVDTime::Compare(const DVDTime u) const
	{
	if (stamp < u.stamp)
		return -1;
	else if (stamp > u.stamp)
		return 1;
	else
		return 0;
	}

////////////////////////////////////////////////////////////////////
//
//  DVDLocation Class
//
////////////////////////////////////////////////////////////////////

//
//  Compare two DVDLocations
//  Note that this function does not compare the times!!!
//

BOOL NotEqual(const DVDLocation u, const DVDLocation v)
	{
	if (u.domain			!= v.domain				||
		 u.videoTitleSet	!= v.videoTitleSet	||
		 u.title				!= v.title				||
		 u.vtsTitle			!= v.vtsTitle			||
		 u.titleTime		!= v.titleTime			||
		 u.partOfTitle		!= v.partOfTitle		||
		 u.programChain	!= v.programChain		||
		 u.pgcTime			!= v.pgcTime			||
		 u.program			!= v.program			||
		 u.cell				!= v.cell				||
		 u.cellTime			!= v.cellTime)
		 return TRUE;
	else
		return FALSE;
	}

////////////////////////////////////////////////////////////////////
//
//  DVDAudioStreamFormat
//
////////////////////////////////////////////////////////////////////

//
//  Operator==
//

BOOL operator==(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v)
	{
	return (u.languageCode == v.languageCode) &&
			 (u.languageExtension == v.languageExtension) &&
			 (u.codingMode == v.codingMode) &&
			 (u.bitsPerSample == v.bitsPerSample) &&
			 (u.samplesPerSecond == v.samplesPerSecond) &&
			 (u.channels == v.channels);
	}

//
//  Operator!=
//

BOOL operator!=(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v)
	{
	return (u.languageCode != v.languageCode) ||
			 (u.languageExtension != v.languageExtension) ||
			 (u.codingMode != v.codingMode) ||
			 (u.bitsPerSample != v.bitsPerSample) ||
			 (u.samplesPerSecond != v.samplesPerSecond) ||
			 (u.channels != v.channels);
	}

////////////////////////////////////////////////////////////////////
//
//  Video Stream Format
//
////////////////////////////////////////////////////////////////////

//
//  Operator==
//

BOOL operator==(const VideoStreamFormat & u, const VideoStreamFormat & v)
	{
	return (u.compressionMode == v.compressionMode) &&
			 (u.videoStandard == v.videoStandard) &&
			 (u.sourceAspectRatio == v.sourceAspectRatio) &&
			 (u.panScanOn4By3 == v.panScanOn4By3) &&
			 (u.letterboxedOn4By3 == v.letterboxedOn4By3) &&
			 (u.line21Mode == v.line21Mode) &&
			 (u.sourceWidth == v.sourceWidth) &&
			 (u.sourceHeight == v.sourceHeight) &&
			 (u.sourceLetterboxed == v.sourceLetterboxed);
	}

//
//  Operator!=
//

BOOL operator!=(const VideoStreamFormat & u, const VideoStreamFormat & v)
	{
	return (u.compressionMode != v.compressionMode) ||
			 (u.videoStandard != v.videoStandard) ||
			 (u.sourceAspectRatio != v.sourceAspectRatio) ||
			 (u.panScanOn4By3 != v.panScanOn4By3) ||
			 (u.letterboxedOn4By3 != v.letterboxedOn4By3) ||
			 (u.line21Mode != v.line21Mode) ||
			 (u.sourceWidth != v.sourceWidth) ||
			 (u.sourceHeight != v.sourceHeight) ||
			 (u.sourceLetterboxed != v.sourceLetterboxed);
	}

////////////////////////////////////////////////////////////////////
//
//  DVDSubPictureStreamFormat
//
////////////////////////////////////////////////////////////////////

//
//  Operator==
//

BOOL operator==(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v)
	{
	return (u.languageCode == v.languageCode) && (u.languageExtension == v.languageExtension);
	}

//
//  Operator!=
//

BOOL operator!=(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v)
	{
	return (u.languageCode != v.languageCode) || (u.languageExtension != v.languageExtension);
	}

////////////////////////////////////////////////////////////////////
//
//  DVDButtonInformation
//
////////////////////////////////////////////////////////////////////

//
//  Operator==
//

BOOL operator==(const DVDButtonInformation u, const DVDButtonInformation v)
	{
	return (u.x == v.y) && (u.y == v.y) && (u.w == v.w) && (u.h == v.h) && (u.autoAction == v.autoAction) &&
			 (u.upperButton == v.upperButton) && (u.lowerButton == v.lowerButton) &&
			 (u.leftButton == v.leftButton) && (u.rightButton == v.rightButton);
	}

//
//  Operator!=
//

BOOL operator!=(const DVDButtonInformation u, const DVDButtonInformation v)
	{
	return (u.x != v.x) || (u.y != v.y) || (u.w != v.w) || (u.h != v.h) || (u.autoAction != v.autoAction) ||
			 (u.upperButton != v.upperButton) || (u.lowerButton != v.lowerButton) ||
			 (u.leftButton != v.leftButton) || (u.rightButton != v.rightButton);
	}
