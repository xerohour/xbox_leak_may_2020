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

#include "DTSStreamDecoder.h"
#include "library\common\vddebug.h"

#pragma warning(disable : 4355)
DTSStreamDecoder::DTSStreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy)
									: DTSDecoder(this, pcmWaveOut, timingSlave, globalProfile, profile),
									  AudioStreamDecoder(pcmWaveOut, streamFlipCopy),
									  supervisor(this),
									  TimedFiber(14)
	{
	streaming = FALSE;
	frameDuration = 32;
//	this->streamFlipCopy = streamFlipCopy;
	}
#pragma warning(default : 4355)

DTSStreamDecoder::~DTSStreamDecoder(void)
	{
	if (streaming) EndStreaming(TRUE);
	}

void DTSStreamDecoder::FiberRoutine(void)
	{
	int buffer;

	while (strm->RefillBuffer(40))//12288))
		{
		ParseSyncFrame();

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

DTSStreamDecoder::Supervisor::Supervisor(DTSStreamDecoder * decoder)
	: TimedFiber(4)
	{
	this->decoder = decoder;
	}

void DTSStreamDecoder::Supervisor::FiberRoutine(void)
	{
	SetFiberPriority(3);
	decoder->SupervisorFiberRoutine();
	}

void DTSStreamDecoder::SupervisorFiberRoutine(void)
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

void DTSStreamDecoder::InitLocalsNewStream(void)
	{
	SetFiberPriority(3);
	lowPriority = FALSE;
	streaming = TRUE;
	avgFrameSize = 0;

	supervisor.StartFiber();
	StartFiber();
	}

void DTSStreamDecoder::ClosingCurrentStream(void)
	{
	CompleteFiber();
	supervisor.CompleteFiber();
	FlushBuffer();
	streaming = FALSE;
	}

void DTSStreamDecoder::BeginStreaming(void)
	{
	AudioStreamDecoder::BeginStreaming();
	DTSDecoder::BeginStreaming();
	InitLocalsNewStream();
	}

void DTSStreamDecoder::StartStreaming(int playbackSpeed)
	{
	DTSDecoder::StartStreaming(playbackSpeed);
	AudioStreamDecoder::StartStreaming(playbackSpeed);
	}

void DTSStreamDecoder::StopStreaming(void)
	{
	DTSDecoder::StopStreaming();
	AudioStreamDecoder::StopStreaming();
	}

void DTSStreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		AudioStreamDecoder::EndStreaming(fullReset);
		DTSDecoder::EndStreaming(fullReset);
		ClosingCurrentStream();
		}
	}

void DTSStreamDecoder::DetachStreaming(void)
	{
	if (streaming)
		{
		AudioStreamDecoder::DetachStreaming();
		DTSDecoder::DetachStreaming();
		ClosingCurrentStream();
		}
	}

void DTSStreamDecoder::AttachStreaming(void)
	{
	AudioStreamDecoder::AttachStreaming();
	DTSDecoder::AttachStreaming();
	InitLocalsNewStream();
	}

void DTSStreamDecoder::CompleteData(void)
	{
	BYTE fakeheader[] = {0x7f, 0xfe, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00};

	if (!completing)
		{
		AddData(fakeheader, 8);
		StuffBuffer();						// Fill up to next DWORD
		completing = TRUE;
		bufferRefillEvent.SetEvent();
		}
	}

//
//  Send Data
//
DWORD DTSStreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	return(AudioStreamDecoder::SendData(ptr, num));
	}


