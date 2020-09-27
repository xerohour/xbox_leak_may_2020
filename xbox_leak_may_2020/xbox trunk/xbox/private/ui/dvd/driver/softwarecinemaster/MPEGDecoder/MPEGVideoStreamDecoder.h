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

#ifndef MPEGVIDEOSTREAMDECODER_H
#define MPEGVIDEOSTREAMDECODER_H

#include "SliceParser.h"
#include "..\common\TimedFibers.h"
#include "library\common\profiles.h"

class MPEGVideoStreamDecoder : public VideoBitStream,
                          protected VideoStreamParser,
								  protected PictureParser,
								  private TimedFiber
	{
	protected:
		GenericPictureDisplay	*	display;
		GenericSliceParser		*	decoder;

		TimedFiberEvent				bufferRefillEvent;

		int								requestedBits;

		bool								completing;
		bool								terminate;
		bool								streamingCompleted;
		bool								streaming;
		int								endCodeReceived;
		int								endCodeCompleted;
		VideoPlaybackMode				playbackMode;

		void PictureDecodingCompleted(void);
		bool InternalIsBufferComplete(void);
		bool InternalRefillBuffer(int requestBits, bool lowDataPanic);

		void TrickSegmentRequested(void);
		void WaitForSegmentCompleted(void);
		bool IsSegmentCompleted(void);

		void FiberRoutine(void);
	public:
		MPEGVideoStreamDecoder(GenericPictureDisplay * display,
							   GenericSliceParser * highQualitySliceParser,
			               GenericSliceParser * lowQualitySliceParser,
								GenericProfile * globalProfile,
								GenericProfile * profile,
								StreamFlipCopy * streamFlipCopy);
		~MPEGVideoStreamDecoder(void);

		void BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);

		void SetLine21Decoder(Line21Decoder * line21Decoder) {VideoStreamParser::SetLine21Decoder(line21Decoder);}

		void AdvanceFrame(void);
		bool StreamingCompleted(void);
		DWORD VideoSegmentRequested(void);
		void SetFirstVideoSegment(DWORD seg);
		void SetLastVideoSegment(DWORD seg);
		void AdvanceTrickFrame(int by);

		bool EnoughDataToStart(void);

		DWORD SendData(BYTE * ptr, DWORD num);
		void CompleteData(void);

		virtual void RefillRequest(void) = 0;
		virtual void StreamCompleted(void) {}
		virtual void ScanCompleted(void) {}

		int DecodeWidth(void) {return width;}
		int DecodeHeight(void) {return height;}

		void SetErrorMessenger(ASyncErrorMessenger * errorMessenger) {VideoStreamParser::SetErrorMessenger(errorMessenger);}
	};

#endif
