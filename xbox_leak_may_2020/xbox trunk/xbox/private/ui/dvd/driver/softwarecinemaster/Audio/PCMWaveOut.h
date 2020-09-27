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

#ifndef PCMWAVEOUT_H
#define PCMWAVEOUT_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\TimedFibers.h"
#include "library\common\profiles.h"
#include "AudioPlaybackDevice.h"
#include "..\common\StreamTiming.h"


class PCMWaveOut : private TimedFiber, GenericAudioPlaybackCallback
	{
	protected:
		GenericProfile		*	globalProfile;
		GenericProfile		*	profile;

		int					sampleRate;
		int					inputByteCount, outputByteCount;
		int					playbackSpeed;
		TimedFiberEvent		event;
		int					downMixLevel, userDownMixLevel;

		bool				advanceFrame;
		bool				terminate, threadRunning, running, dropping, detaching;
		bool				monoOutput;
//		bool				resampling;

		int					dataDropStartTime, dataDropStartPosition;
		int					maxAudioBuffer;

		TimingSlave		*	timingSlave;

		TimedFiberEvent	fiberEvent;
		GenericAudioPlaybackDevice * device;

		void FiberRoutine(void);

		void WaveDataRequest(void);

		int GetOutputPosition(void);

		void AddDualData(short * leftRightAna, short * leftRightDig, int num, int time);


	public:
		PCMWaveOut(GenericProfile * globalProfile, GenericProfile * profile, GenericAudioPlaybackDevice * device, TimingSlave * timingSlave);
		~PCMWaveOut(void);


		void AddPCMData(short * leftRight, int num, int time);
//		void AddPCMDualData(short * leftRight, short * leftRight1, int num, int time);
		void AddPCMDigData(short * leftRightAna, short * leftRightDig, int num, int time);
		void WaitForCompletion(void);

		void BeginStreaming(void);
		void StartStreaming(int playbackSpeed);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);
		void CompleteStreaming(void);
		void DetachStreaming(void);
		void AttachStreaming(void);

		void AdvanceFrame(void);

		int GetBufferDelayTime();

		void SetSampleRate(int rate);

		bool SupportsSPDIFDataOut(void) {return device->SupportsSPDIFDataOut();}

		void SetSPDIFDataOut(bool enable);
//		void GetSPDIFDataOut(bool &enable);
		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg);

		Error CheckHardwareResources(void) {return device->CheckHardwareResources();}
		bool SupportsVolumeControl(void) {return device->SupportsVolumeControl();}
	};


#endif
