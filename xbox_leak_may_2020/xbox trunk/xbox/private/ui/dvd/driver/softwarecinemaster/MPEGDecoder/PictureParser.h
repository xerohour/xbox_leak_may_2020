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

#ifndef PICTUREPARSER_H
#define PICTUREPARSER_H

#include "VideoStreamParser.h"
#include "..\video\FrameStore.h"
#include "..\video\VideoDisplay.h"
#include "library\common\profiles.h"
#include "..\spudecoder\SPUDisplayBuffer.h"

enum BobWeaveLevel
	{
	BWL_FORCE_BOB,
	BWL_FORCE_WEAVE,
	BWL_PREFERE_BOB,
	BWL_PREFERE_WEAVE,
	BWL_SMART_DECISION
	};

#define PF_TOP_FIELD_FIRST(x)					((x & 0x20) != 0)
#define PF_FRAME_PRED_FRAME_DCT(x)			((x & 0x10) != 0)
#define PF_CONCEALMENT_MOTION_VECTORS(x)	((x & 0x08) != 0)
#define PF_Q_SCALE_TYPE(x)						((x & 0x04) != 0)
#define PF_INTRA_VLC_FORMAT(x)				((x & 0x02) != 0)
#define PF_ALTERNATE_SCAN(x)					((x & 0x01) != 0)

class GenericSliceParser : public SPUDisplayBufferFactory
	{
	public:
		virtual ~GenericSliceParser(void) {}

		virtual void SetVideoCodingStandard(VideoCodingStandard vcd) = 0;
		virtual void SetFCodes(BYTE fc00, BYTE fc01, BYTE fc10, BYTE fc11) = 0;
		virtual void SetPictureParameters(BYTE intraDCPrecision,
			                               PictureStructure pictureStructure,
												  	 BYTE pictureFlags) = 0;
		virtual void SetCodingType(PictureCodingType pictureCodingType) = 0;
		virtual void SetForwardVector(bool fullPel, BYTE fcode) = 0;
		virtual void SetBackwardVector(bool fullPel, BYTE fcode) = 0;
		virtual void SetImageSize(int width, int height) = 0;
		virtual void SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix) = 0;

		virtual bool BeginFrameDecoding(FrameStore * pred, FrameStore * succ, FrameStore * curr, PictureDisplayBuffer * display, bool hurryUp) = 0;
		virtual void DoneFirstFieldDecoding(void) = 0;
		virtual void BeginSecondFieldDecoding(void) = 0;
		virtual void DoneFrameDecoding(void) = 0;

		virtual bool SuspendFrameDecoding(void) {return FALSE;}
		virtual void ResumeFrameDecoding(void) {}

		virtual void Parse(void) = 0;

		virtual void SetBitStream(VideoBitStream * bitStream) = 0;

		virtual FrameStore * AllocFrameStore(PictureCodingType type, bool stripeOptimized) = 0;

		virtual int MinimumStretch(void) {return 1;}

		virtual void SetBrightness(int Brightness, int uBrightness, int vBrightness) = 0;
		virtual void SetContrast(int contrast) = 0;
		virtual void SetSaturation(int saturation) = 0;
		virtual DWORD QueryColorControl(void) = 0;

		virtual void BeginFrameSequence(void) {}
		virtual void EndFrameSequence(void) {}

		virtual void BeginFrameReference(FrameStore * frame) {}
		virtual void EndFrameReference(FrameStore * frame) {}
		virtual void InvalidateFrameReferences(void) {}
	};

class PictureParser : public GenericPictureParser, protected TimingClient
	{
	protected:
		GenericSliceParser		*	sliceParser;
		GenericPictureDisplay	*	display;
		VideoBitStream				*	bitStream;
		GenericProfile				*	globalProfile;
		GenericProfile				*	profile;

		GenericSliceParser		*	lowQualitySliceParser;
		GenericSliceParser		*	highQualitySliceParser;

		FrameStore					*	ipFrameBuffers[3];
		FrameStore					*	bFrameBuffers[2];
		FrameStore					*	previousFrame;
		int								previousBFrameID;
		BYTE								pictureFlags;
		BYTE								lastIPPictureFlags;
		bool								repeatFirstField;
		bool								progressiveFrame;
		bool								progressiveSequenceStructure;
		int								frameRepeat, ipFrameRepeat, thisFrameRepeat;

		TimedFiberEvent				event;
		TimedFiberMutex				doneStreamingLock;

	private:
		int								pad00;
	protected:
		PictureCodingType				codingType;
		PictureStructure				pictureStructure;
		VideoCodingStandard			vcs;
		PictureStructure				lastIPFieldDisplayMode;
		Inverse32PulldownHint		phint;
		PictureDisplayAspectRatio	aspectRatio;
		int								panScanOffset;

		int								frameContinuationCounter;

		DWORD								frameDropIncrement, frameDropCounter;

		bool								isIPFrame;
		bool								initialized;
		VideoPlaybackMode				playbackMode;
		bool								isStreamPlayback;
		bool								progressiveSequence, pullDownSequence, mixedPullDownSequence;
		bool								initialBFrameSkip;
		BobWeaveLevel					bobWeaveLevel;
		bool								pullDownReconstruction;
		bool								lastIPRepeatFirstField;
		bool								lastIPProgressiveFrame;
		bool								highQuality;
		bool								alwaysHurryUp;

		DWORD								progressiveHistory, repeatFirstHistory, progressiveDisplayHistory;
		int								pastIPFrame, futureIPFrame;
		int								fieldCount, lowFieldCount;
		int								framesPerSecond, fieldTime;
		int								timeOffset, previousTimeStamp;
		int								lastIPTimeStamp;
		int								playbackSpeed;
		int								width, height;
		DWORD								currentDeinterlaceMode;

		PictureDisplayBuffer		*	ipDisplayBuffer;
		PictureDisplayBuffer		*	bDisplayBuffer;
		PictureDisplayBuffer		*	previousDisplayBuffer;
		bool								previousWasIPFrame;
		int								previousDisplayTime;

		void ParsePictureHeader(void);
		void ParsePictureCodingExtension(void);
		void ParseQuantMatrixExtension(void);
		void ParsePictureDisplayExtension(void);
		void ParseExtensionAndUserData(void);

		bool WaitForPictureDataComplete(void);
		bool ParsePictureBody(void);
		bool SkipPictureBody(void);
		void ParsePicture(void);

		void PreParsePicture(PictureCodingType & codingType);
		void ParseTempPicture(void);

		void PostIPFrameBuffer(int displayTime, bool hurryUp);
		void PostBFrameBuffer(int displayTime, Inverse32PulldownHint bphint, bool hurryUp);
		void PostPreviousFrame(void);

		void ReleaseFrameBuffers(void);
		void AllocateFrameBuffers(void);

		virtual void PictureDecodingCompleted(void) {}
	public:
		void SetImageSize(int width, int height, int aspectRatio);
		void SetQuantMatrix(QuantMatrices type, const QuantizationMatrix & matrix);
		void SetFrameRate(int framesPerSecond);
		void SetClosedGOP(void);
		void SetBrokenLink(void);
		void SetVideoCodingStandard(VideoCodingStandard vcs);
		void SetProgressiveSequence(bool progressiveSequence);

		void Parse(PictureDecodeMode mode);
		void PreParse(PictureCodingType & codingType);

		void BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality);
		void EndStreaming(bool fullReset);

		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);

		void DoneStreaming(void);

		void AdvanceFrame(void);

		PictureParser(GenericSliceParser * highQualitySliceParser,
			           GenericSliceParser * lowQualitySliceParser,
						  GenericPictureDisplay * display,
						  VideoBitStream * bitStream,
						  GenericProfile * globalProfile,
						  GenericProfile * profile);
		~PictureParser(void);
	};

#endif
