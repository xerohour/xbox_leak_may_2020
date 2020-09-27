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

#ifndef MPEGAUDIOSTREAMDECODER_H
#define MPEGAUDIOSTREAMDECODER_H

#include "MPEGAudioDecoder.h"
#include "..\audio\AudioStreamDecoder.h"
#include "..\common\TimedFibers.h"

class MPEGAudioStreamDecoder : public AudioStreamDecoder,
                         protected MPEGAudioDecoder,
								 private TimedFiber
	{
	protected:
		bool								lowPriority;
		bool								streaming;

		class Supervisor : public TimedFiber
			{
			protected:
				MPEGAudioStreamDecoder * decoder;
			public:
				Supervisor(MPEGAudioStreamDecoder * decoder);
				void FiberRoutine(void);
			} supervisor;

		void FiberRoutine(void);
		void SupervisorFiberRoutine(void);
		void InitLocalsNewStream(void);
		void ClosingCurrentStream(void);

		friend class MPEGAudioStreamDecoder::Supervisor;
	public:
		MPEGAudioStreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy);
		~MPEGAudioStreamDecoder(void);

		void BeginStreaming(void);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);
		void DetachStreaming(void);
		void AttachStreaming(void);

		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
			{MPEGAudioDecoder::SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);}
	};

#endif
