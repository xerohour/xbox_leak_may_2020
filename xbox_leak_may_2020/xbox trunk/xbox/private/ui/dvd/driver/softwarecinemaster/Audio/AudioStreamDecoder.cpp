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

#include "AudioStreamDecoder.h"
#include "library\common\vddebug.h"

//
//  Constructor
//

AudioStreamDecoder::AudioStreamDecoder(PCMWaveOut * pcmWaveOut, StreamFlipCopy * streamFlipCopy)
	: bufferRefillEvent(FALSE, TRUE), AudioBitStream(streamFlipCopy)
	{
	this->pcmWaveOut = pcmWaveOut;

	//	bufferRefillEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	avgFrameSize = 0;
	}

//
//  Destructor
//

AudioStreamDecoder::~AudioStreamDecoder(void)
	{
//	CloseHandle(bufferRefillEvent);
//	DeleteCriticalSection(&lock);
	}

//
//  Begin Streaming
//

void AudioStreamDecoder::BeginStreaming(void)
	{
	terminate = FALSE;
	completing = FALSE;
	streamingCompleted = FALSE;
	}

void AudioStreamDecoder::DetachStreaming(void)
	{
	terminate = TRUE;
	bufferRefillEvent.SetEvent();
	}

void AudioStreamDecoder::AttachStreaming(void)
	{
	terminate = FALSE;
	completing = FALSE;
	streamingCompleted = FALSE;
	}

//
// Start Streaming
//

void AudioStreamDecoder::StartStreaming(int playbackSpeed)
	{
	}

//
//  Stop Streaming
//

void AudioStreamDecoder::StopStreaming(void)
	{
	}

//
//  End Streaming
//

void AudioStreamDecoder::EndStreaming(bool fullReset)
	{
	terminate = TRUE;
	bufferRefillEvent.SetEvent();
	}

//
//  Send Data
//

DWORD AudioStreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	DWORD done;

	done = AddData(ptr, num);

	if (AvailBits() >= requestedBits) bufferRefillEvent.SetEvent();

	return done;
	}

//
//  Complete Data
//

void AudioStreamDecoder::CompleteData(void)
	{
	if (!completing)
		{
		StuffBuffer();						// Fill up to next DWORD
		completing = TRUE;
		bufferRefillEvent.SetEvent();
		}
	}

//
//  Internal Is Buffer Complete
//

bool AudioStreamDecoder::InternalIsBufferComplete(void)
	{
	return completing;
	}

//
//  Internal Refill Buffer
//

bool AudioStreamDecoder::InternalRefillBuffer(int requestBits, bool lowDataPanic)
	{
	requestedBits = requestBits;

	//
	//  Repeat requesting data as long we do not want to terminate and do not have enough data
	//

	while (!terminate && !completing && requestBits > AvailBits())
		{
		RefillRequest();

		bufferRefillEvent.Wait(100);
		}

	if (!terminate && !completing && AvailBits() < 8 * AUDIO_STREAM_BUFFER_SIZE) RefillRequest();

	return !terminate && (completing && AvailBits() > 0 || AvailBits() >= requestBits);
	}

//
//  Streaming Completed
//

bool AudioStreamDecoder::StreamingCompleted(void)
	{
	return streamingCompleted;// && pcmWaveOut->GetBufferDelayTime() < 200;
	}

//
//  Current Playback Location
//

__int64 AudioStreamDecoder::CurrentPlaybackLocation(void)
	{
	__int64 pos = CurrentLocation();
	int delta;

	if (avgFrameSize && !terminate)
		{
		delta = pcmWaveOut->GetBufferDelayTime();

		if (delta < 0x100000 && avgFrameSize)
			delta = ScaleLong(delta, frameDuration, avgFrameSize);
		else
			delta = 0;

		if (pos < delta)
			return 0;
		else
			return pos - delta;
		}
	else
		return pos;
	}

//
//  Total Audio Buffer Time
//

int AudioStreamDecoder::TotalAudioBufferTime(void)
	{
	if (avgFrameSize)
		return pcmWaveOut->GetBufferDelayTime() + ScaleLong(AvailBits(), avgFrameSize, frameDuration);
	else
		return pcmWaveOut->GetBufferDelayTime();
	}
