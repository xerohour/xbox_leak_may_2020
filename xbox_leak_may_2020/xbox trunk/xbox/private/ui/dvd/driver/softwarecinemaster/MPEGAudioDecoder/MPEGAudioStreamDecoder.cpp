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

#include "MPEGAudioStreamDecoder.h"


#pragma warning(disable : 4355)
MPEGAudioStreamDecoder::MPEGAudioStreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy)
									: MPEGAudioDecoder(this, pcmWaveOut, timingSlave, globalProfile, profile),
									  AudioStreamDecoder(pcmWaveOut, streamFlipCopy),
									  supervisor(this),
									  TimedFiber(2)
	{
	streaming = FALSE;
	frameDuration = 24;
	}
#pragma warning(default : 4355)

MPEGAudioStreamDecoder::~MPEGAudioStreamDecoder(void)
	{
	if (streaming) EndStreaming(TRUE);
	}

void MPEGAudioStreamDecoder::FiberRoutine(void)
	{
	int buffer;

	while (strm->RefillBuffer(12288))
		{
		ParseFrame();

		if (!avgFrameSize)
			avgFrameSize = frameSize;
		else
			avgFrameSize = (avgFrameSize * 7 + frameSize) >> 3;

		buffer = AudioStreamDecoder::pcmWaveOut->GetBufferDelayTime();

		if (hurryUp && buffer > 400)
			{
			hurryUp = FALSE;

			SetFiberPriority(2);
			}
		else if (!hurryUp && buffer < 300)
			{
			hurryUp = TRUE;
			lowPriority = FALSE;

			SetFiberPriority(3);
			}
		else if (lowPriority && buffer < 500)
			{
			lowPriority = FALSE;

			SetFiberPriority(2);
			}
		else if (!lowPriority && buffer > 600)
			{
			lowPriority = TRUE;
			hurryUp = FALSE;

			SetFiberPriority(1);
			}

		if (!hurryUp) YieldFiber(0);
		else YieldFiber(-1);
		}

	if (!terminate)
		{
		AudioStreamDecoder::pcmWaveOut->CompleteStreaming();

		AudioStreamDecoder::pcmWaveOut->WaitForCompletion();
		}

	streamingCompleted = TRUE;
	StreamCompleted();
	}

MPEGAudioStreamDecoder::Supervisor::Supervisor(MPEGAudioStreamDecoder * decoder)
	: TimedFiber(4)
	{
	this->decoder = decoder;
	}

void MPEGAudioStreamDecoder::Supervisor::FiberRoutine(void)
	{
	SetFiberPriority(3);
	decoder->SupervisorFiberRoutine();
	}

void MPEGAudioStreamDecoder::SupervisorFiberRoutine(void)
	{
	int buffer;

	while (!terminate)
		{
		buffer = AudioStreamDecoder::pcmWaveOut->GetBufferDelayTime();

		if (hurryUp && buffer > 400)
			{
			hurryUp = FALSE;

			SetFiberPriority(2);
			}
		else if (!hurryUp && buffer < 300)
			{
			hurryUp = TRUE;
			lowPriority = FALSE;

			SetFiberPriority(3);
			}
		else if (lowPriority && buffer < 500)
			{
			lowPriority = FALSE;

			SetFiberPriority(2);
			}
		else if (!lowPriority && buffer > 600)
			{
			lowPriority = TRUE;
			hurryUp = FALSE;

			SetFiberPriority(1);
			}

		YieldTimedFiber(100);
		}
	}

void MPEGAudioStreamDecoder::InitLocalsNewStream(void)
	{
	SetFiberPriority(3);
	lowPriority = FALSE;
	streaming = TRUE;
	avgFrameSize = 0;
	supervisor.StartFiber();
	StartFiber();
	}

void MPEGAudioStreamDecoder::ClosingCurrentStream(void)
	{
	CompleteFiber();
	supervisor.CompleteFiber();
	FlushBuffer();
	streaming = FALSE;
	}

void MPEGAudioStreamDecoder::BeginStreaming(void)
	{
	AudioStreamDecoder::BeginStreaming();
	MPEGAudioDecoder::BeginStreaming();
	InitLocalsNewStream();
	}

void MPEGAudioStreamDecoder::StartStreaming(int playbackSpeed)
	{
	MPEGAudioDecoder::StartStreaming(playbackSpeed);
	AudioStreamDecoder::StartStreaming(playbackSpeed);
	}

void MPEGAudioStreamDecoder::StopStreaming(void)
	{
	MPEGAudioDecoder::StopStreaming();
	AudioStreamDecoder::StopStreaming();
	}

void MPEGAudioStreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		AudioStreamDecoder::EndStreaming(fullReset);
		MPEGAudioDecoder::EndStreaming(fullReset);
		ClosingCurrentStream();
		}
	}

void MPEGAudioStreamDecoder::DetachStreaming(void)
	{
	if (streaming)
		{
		AudioStreamDecoder::DetachStreaming();
		MPEGAudioDecoder::DetachStreaming();
		ClosingCurrentStream();
		}
	}

void MPEGAudioStreamDecoder::AttachStreaming(void)
	{
	AudioStreamDecoder::AttachStreaming();
	MPEGAudioDecoder::AttachStreaming();
	InitLocalsNewStream();
	}
