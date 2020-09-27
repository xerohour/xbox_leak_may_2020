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

#ifndef VIDEOSTREAMPARSER_H
#define VIDEOSTREAMPARSER_H

#include "huffdec.h"
#include "MPEGUserDataDecoder.h"
#include "..\common\TimedFibers.h"
#include "..\common\ASyncErrorMessages.h"

typedef int QuantizationMatrix[64];

enum QuantMatrices
	{
	LUMA_INTRA_QUANTMATRIX,
	LUMA_NONINTRA_QUANTMATRIX,
	CHROMA_INTRA_QUANTMATRIX,
	CHROMA_NONINTRA_QUANTMATRIX
	};

enum VideoCodingStandard
	{
	VCS_MPEG_1,
	VCS_MPEG_2
	};

enum VideoPlaybackMode
	{
	VDPM_PLAY_FORWARD,
	VDPM_PLAY_BACKWARD,
	VDPM_SCANNING,
	VDPM_TRICKPLAY
	};

enum PictureCodingType
	{
	INTRA_CODED	= 1,
	PREDICTIVE_CODED = 2,
	BIDIRECTIONAL_PREDICTIVE_CODED = 3
	};

enum PictureDecodeMode
	{
	PPM_DECODE_AND_DISPLAY,
	PPM_DECODE_AND_DROP,
	PPM_DECODE_AND_KEEP,
	};

class GenericPictureParser
	{
	protected:
		MPEGUserDataDecoder * userDataDecoder;
	public:
		void SetUserDataDecoder(MPEGUserDataDecoder * userDataDecoder) {this->userDataDecoder = userDataDecoder;}
		virtual void SetVideoCodingStandard(VideoCodingStandard vcd) = 0;
		virtual void SetFrameRate(int framesPerSecond) = 0;
		virtual void SetImageSize(int width, int height, int aspectRatio) = 0;
		virtual void SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix) = 0;
		virtual void SetClosedGOP(void) = 0;
		virtual void SetBrokenLink(void) = 0;
		virtual void SetProgressiveSequence(bool progressiveSequence) = 0;

		virtual void Parse(PictureDecodeMode mode) = 0;
		virtual void PreParse(PictureCodingType & codingType) = 0;
	};

class VideoStreamSyntaxError : public Exception
	{
	public:
		int	subCode;
		VideoStreamSyntaxError(int subCode) {this->subCode = subCode;}
	};

class PictureHeaderSyntaxError : public Exception {};

class VideoStreamParser : public MPEGUserDataDecoder
	{
	protected:
		GenericPictureParser	*	pictureParser;
		VideoBitStream			*	bitStream;
		VideoPlaybackMode			playbackMode;
		__int64						prevVideoBitBufferPosition;
		TimedFiberEvent			event;
		volatile bool				terminate;

		struct GOPFrameData
			{
			PictureCodingType		codingType;
			__int64					position;
			int						displayPosition;
			int						displayFrame;
			} gopFrameData[2][32];

		int							gopFrameDataIndex;
		int							numGOPFrames;
		int							currentGOPFrame;
		int							firstGOPIPFrame;
		int							lastGOPIPFrame;
		int							lastGOPIFrame;
		__int64						nextGOPPosition;
		volatile int				requestedFrameID;
		int							currentFrameID;
		int							firstFrameID;
		DWORD							currentSegmentIndex;
		DWORD							firstSegmentIndex, lastSegmentIndex;

		bool							usingDefaultQuantizerTable[2];
		bool							hasValidSequenceHeader;
		bool							progressiveSequence;

		int							sequenceWidth, sequenceHeight, sequenceFPS, sequenceAspectRatio;
		ASyncErrorMessenger	*	errorMessenger;

		enum GOPFrameState
			{
			GFS_INITIAL,
			GFS_PREFRAMES,
			GFS_POSTFRAMES
			};

		void PreParseVideoSequence(void);
		void ParseVideoSequenceReverse(void);
		void ParseVideoSequenceTrick(void);

		void ParseVideoSequence(void);
		void ParseMPEG1GroupOfPictures(DWORD & header);

		void ParseSequenceHeader(void);
		void ParseExtensionAndUserData(void);
		void ParseSequenceExtension(void);
		void ParseSequenceDisplayExtension(void);
		void ParseGroupOfPicturesHeader(void);

		virtual void TrickSegmentRequested(void) = 0;
		virtual void WaitForSegmentCompleted(void) = 0;
		virtual bool IsSegmentCompleted(void) = 0;
	public:
		VideoStreamParser(GenericPictureParser * pictureParser, VideoBitStream * bitStream);

		void BeginStreaming(VideoPlaybackMode playbackMode);
		void EndStreaming(bool fullReset);

		void Parse(void);

		DWORD VideoSegmentRequested(void);
		void SetFirstVideoSegment(DWORD seg);
		void SetLastVideoSegment(DWORD seg);
		void AdvanceTrickFrame(int by);

		void SetErrorMessenger(ASyncErrorMessenger * errorMessenger) {this->errorMessenger = errorMessenger;}
	};

#endif
