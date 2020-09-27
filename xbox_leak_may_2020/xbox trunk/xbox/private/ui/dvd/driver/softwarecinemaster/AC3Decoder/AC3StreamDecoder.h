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

#ifndef AC3STREAMDECODER_H
#define AC3STREAMDECODER_H

#include "AC3Decoder.h"
#include "..\audio\AudioStreamDecoder.h"
#include "..\common\TimedFibers.h"

enum	ByteOrder
	{
	BO_UNKNOWN,
	BO_FORWARD,
	BO_REVERSE
	};

class AC3StreamDecoder : public AudioStreamDecoder,
                         protected AC3Decoder,
								 private TimedFiber
	{
	protected:
		bool								lowPriority;
		bool								streaming;
		ByteOrder							byteOrder;

		StreamFlipCopy * wordCopySwap;
		StreamFlipCopy * streamFlipCopy;

		class Supervisor : public TimedFiber
			{
			protected:
				AC3StreamDecoder * decoder;
			public:
				Supervisor(AC3StreamDecoder * decoder);
				void FiberRoutine(void);
			} supervisor;

		void FiberRoutine(void);
		void SupervisorFiberRoutine(void);
		void InitLocalsNewStream(void);
		void ClosingCurrentStream(void);


		friend class AC3StreamDecoder::Supervisor;
	public:
		AC3StreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy);
		~AC3StreamDecoder(void);

		void BeginStreaming(void);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);
		void DetachStreaming(void);
		void AttachStreaming(void);
		DWORD SendData(BYTE * ptr, DWORD num);

		void CompleteData(void);

		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
			{AC3Decoder::SetAudioOutConfig(spkCfg, dualModeCfg, karaokeCfg);}

		void SetDolbyTestMode(AC3OperationalMode op, WORD hf, WORD lf, AC3DialogNorm dn)
			{AC3Decoder::SetDolbyTestMode(op, hf, lf, dn);}

		void GetDolbyTestMode(AC3OperationalMode &op, WORD &hf, WORD &lf, AC3DialogNorm &dn)
			{AC3Decoder::GetDolbyTestMode(op, hf, lf, dn);}

//		void SetAC3Config(AC3SpeakerConfig spkCfg, AC3DynamicRange dr, BOOL enableLFE)
//			{AC3Decoder::SetAC3Config(spkCfg, dr, enableLFE);}
//
//		void GetAC3Config(AC3SpeakerConfig &spkCfg, AC3DynamicRange &dr, BOOL &enableLFE)
//			{AC3Decoder::GetAC3Config(spkCfg, dr, enableLFE);}

	};

#endif
