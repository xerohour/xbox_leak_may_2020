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

#ifndef AUDIOSTREAMDECODER_H
#define AUDIOSTREAMDECODER_H

#include "AudioBitStream.h"
#include "PCMWaveOut.h"
#include "..\common\TimedFibers.h"
#include "library\hardware\audio\generic\ac3setup.h"

class AudioStreamDecoder : public AudioBitStream
	{
	protected:
		PCMWaveOut					*	pcmWaveOut;

		TimedFiberEvent				bufferRefillEvent;

		int								requestedBits;
		int								avgFrameSize, frameDuration;

		bool								completing;
		bool								terminate;
		bool								streamingCompleted;

		bool InternalIsBufferComplete(void);
		bool InternalRefillBuffer(int requestBits, bool lowDataPanic);
	public:
		AudioStreamDecoder(PCMWaveOut * pcmWaveOut, StreamFlipCopy * streamFlipCopy);
		virtual ~AudioStreamDecoder(void);

		virtual void SetAudioCodingMode(int samplesPerSecond, int bitsPerSample, int channels) {}

		virtual void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg) {}

		virtual void AdvanceFrame(void) {pcmWaveOut->AdvanceFrame();}

		virtual void BeginStreaming(void);
		virtual void StartStreaming(int playbackSpeed);
		virtual void StopStreaming(void);
		virtual void EndStreaming(bool fullReset);
		virtual void DetachStreaming(void);
		virtual void AttachStreaming(void);

		virtual bool StreamingCompleted(void);

		virtual DWORD SendData(BYTE * ptr, DWORD num);
		virtual void CompleteData(void);

		virtual void RefillRequest(void) = 0;
		virtual void StreamCompleted(void) {}

		virtual __int64 CurrentPlaybackLocation(void);

		virtual int TotalAudioBufferTime(void);

	};

#endif
