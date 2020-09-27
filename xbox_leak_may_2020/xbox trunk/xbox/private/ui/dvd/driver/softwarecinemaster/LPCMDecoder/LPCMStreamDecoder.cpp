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

#include "LPCMStreamDecoder.h"

#pragma warning(disable : 4355)
LPCMStreamDecoder::LPCMStreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, StreamFlipCopy * streamFlipCopy)
									: LPCMDecoder(this, pcmWaveOut, timingSlave),
									  AudioStreamDecoder(pcmWaveOut, streamFlipCopy),
									  supervisor(this),
									  TimedFiber(2)
	{
	streaming = FALSE;
	frameDuration = 10;
	}
#pragma warning(default : 4355)

LPCMStreamDecoder::~LPCMStreamDecoder(void)
	{
	if (streaming)
		EndStreaming(TRUE);
	}

void LPCMStreamDecoder::FiberRoutine(void)
	{
	int buffer;
	int cnt;

	while (strm->RefillBuffer(16, hurryUp))
		{
		cnt = 0;
		while (cnt < 20 && strm->RefillBuffer(16, hurryUp))
			{
			ParseFrame();
			cnt++;
			}

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

		if (!hurryUp)
			{
			YieldFiber(20);
			}
		}

	if (!terminate)
		{
		AudioStreamDecoder::pcmWaveOut->CompleteStreaming();

		AudioStreamDecoder::pcmWaveOut->WaitForCompletion();
		}

	streamingCompleted = TRUE;
	StreamCompleted();
	}

LPCMStreamDecoder::Supervisor::Supervisor(LPCMStreamDecoder * decoder)
	: TimedFiber(4)
	{
	this->decoder = decoder;
	}

void LPCMStreamDecoder::Supervisor::FiberRoutine(void)
	{
	SetFiberPriority(3);
	decoder->SupervisorFiberRoutine();
	}

void LPCMStreamDecoder::SupervisorFiberRoutine(void)
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

void LPCMStreamDecoder::SetAudioCodingMode(int samplesPerSecond,
								                   int bitsPerSample,
								                   int channels)
	{
	LPCMDecoder::SetAudioCodingMode(samplesPerSecond, bitsPerSample, channels);
	}

void LPCMStreamDecoder::InitLocalsNewStream(void)
	{
	streaming = TRUE;
	hurryUp = FALSE;
	lowPriority = FALSE;
	avgFrameSize = frameSize * 6;

	supervisor.StartFiber();
	StartFiber();
	}
void LPCMStreamDecoder::ClosingCurrentStream(void)
	{
	CompleteFiber();
	supervisor.CompleteFiber();
	FlushBuffer();
	streaming = FALSE;
	}

void LPCMStreamDecoder::BeginStreaming(void)
	{
	AudioStreamDecoder::BeginStreaming();
	LPCMDecoder::BeginStreaming();
	InitLocalsNewStream();
	}

void LPCMStreamDecoder::StartStreaming(int playbackSpeed)
	{
	LPCMDecoder::StartStreaming(playbackSpeed);
	AudioStreamDecoder::StartStreaming(playbackSpeed);
	}

void LPCMStreamDecoder::StopStreaming(void)
	{
	LPCMDecoder::StopStreaming();
	AudioStreamDecoder::StopStreaming();
	}

void LPCMStreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		AudioStreamDecoder::EndStreaming(fullReset);
		LPCMDecoder::EndStreaming(fullReset);
		ClosingCurrentStream();
		}
	}

void LPCMStreamDecoder::DetachStreaming(void)
	{
	if (streaming)
		{
		AudioStreamDecoder::DetachStreaming();
		LPCMDecoder::DetachStreaming();
		ClosingCurrentStream();
		}
	}

void LPCMStreamDecoder::AttachStreaming(void)
	{
	AudioStreamDecoder::AttachStreaming();
	LPCMDecoder::AttachStreaming();
	InitLocalsNewStream();
	}

