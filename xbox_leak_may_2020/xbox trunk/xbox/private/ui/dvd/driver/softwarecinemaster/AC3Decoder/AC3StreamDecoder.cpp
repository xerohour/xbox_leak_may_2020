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

#include "AC3StreamDecoder.h"
#include "library\common\vddebug.h"

#pragma warning(disable : 4355)
AC3StreamDecoder::AC3StreamDecoder(PCMWaveOut * pcmWaveOut, TimingSlave * timingSlave, GenericProfile * globalProfile, GenericProfile * profile, StreamFlipCopy * streamFlipCopy)
									: AC3Decoder(this, pcmWaveOut, timingSlave, globalProfile, profile),
									  AudioStreamDecoder(pcmWaveOut, streamFlipCopy),
									  supervisor(this),
									  TimedFiber(2)
	{
	streaming = FALSE;
	frameDuration = 32;
	this->streamFlipCopy = streamFlipCopy;
	wordCopySwap = new StreamFlipWordCopy;
	}
#pragma warning(default : 4355)

AC3StreamDecoder::~AC3StreamDecoder(void)
	{
	if (streaming) EndStreaming(TRUE);
	delete wordCopySwap;
	}

void AC3StreamDecoder::FiberRoutine(void)
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

AC3StreamDecoder::Supervisor::Supervisor(AC3StreamDecoder * decoder)
	: TimedFiber(4)
	{
	this->decoder = decoder;
	}

void AC3StreamDecoder::Supervisor::FiberRoutine(void)
	{
	SetFiberPriority(3);
	decoder->SupervisorFiberRoutine();
	}

void AC3StreamDecoder::SupervisorFiberRoutine(void)
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


void AC3StreamDecoder::InitLocalsNewStream(void)
	{
	SetFiberPriority(3);
	lowPriority = FALSE;
	streaming = TRUE;
	avgFrameSize = 0;
	byteOrder = BO_UNKNOWN;
	AudioStreamDecoder::SetStreamFlipCopy(streamFlipCopy);
	supervisor.StartFiber();
	StartFiber();
	}

void AC3StreamDecoder::ClosingCurrentStream(void)
	{
	CompleteFiber();
	supervisor.CompleteFiber();
	FlushBuffer();
	streaming = FALSE;
	}

void AC3StreamDecoder::BeginStreaming(void)
	{
	AudioStreamDecoder::BeginStreaming();
	AC3Decoder::BeginStreaming();
	InitLocalsNewStream();
	}

void AC3StreamDecoder::StartStreaming(int playbackSpeed)
	{
	AC3Decoder::StartStreaming(playbackSpeed);
	AudioStreamDecoder::StartStreaming(playbackSpeed);
	}

void AC3StreamDecoder::StopStreaming(void)
	{
	AC3Decoder::StopStreaming();
	AudioStreamDecoder::StopStreaming();
	}

void AC3StreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		AudioStreamDecoder::EndStreaming(fullReset);
		AC3Decoder::EndStreaming(fullReset);
		ClosingCurrentStream();
		}
	}

void AC3StreamDecoder::DetachStreaming(void)
	{
	if (streaming)
		{
		AudioStreamDecoder::DetachStreaming();
		AC3Decoder::DetachStreaming();
		ClosingCurrentStream();
		}
	}

void AC3StreamDecoder::AttachStreaming(void)
	{
	AudioStreamDecoder::AttachStreaming();
	AC3Decoder::AttachStreaming();
	InitLocalsNewStream();
	}

void AC3StreamDecoder::CompleteData(void)
	{
	BYTE fakeheader[] = {0x0b, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	if (!completing)
		{
		BYTE revfakeheader[] = {0x77, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		if (byteOrder == BO_REVERSE)
			{
			AddData(revfakeheader, 8);
			}
		else
			{
			AddData(fakeheader, 8);
			}
		StuffBuffer();						// Fill up to next DWORD
		completing = TRUE;
		bufferRefillEvent.SetEvent();
		}
	}

//
//  Send Data
//
DWORD AC3StreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	// check byte order for AC3 file playback.  This is not a complete solution for
	// all AC3 decoding cases.

	// do the most common path first
	if (byteOrder != BO_UNKNOWN)
		{
		return(AudioStreamDecoder::SendData(ptr, num));
		}

	else
		{
		byteOrder = BO_FORWARD;
		if (ptr[0] == 0x0b && ptr[1] == 0x77)
			{
//			DP("AC3 Stream Forward");
			}

		else if (ptr[0] == 0x77 && ptr[1] == 0x0b)
			{
			byteOrder = BO_REVERSE;
			AudioStreamDecoder::SetStreamFlipCopy(wordCopySwap);
//			DP("AC3 Stream Reversed");
			}
		return(AudioStreamDecoder::SendData(ptr, num));
		}

	}


