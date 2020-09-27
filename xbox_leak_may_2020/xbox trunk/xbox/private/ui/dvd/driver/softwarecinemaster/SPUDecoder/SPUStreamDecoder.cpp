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


#include "SPUStreamDecoder.h"

#pragma warning(disable : 4355)
SPUStreamDecoder::SPUStreamDecoder(SPUDisplay * display)
	 : SPUDecoder(this, display), bufferRefillEvent(FALSE, TRUE), TimedFiber(8)
	{
//	InitializeCriticalSection(&lock);
	streaming = FALSE;
	}
#pragma warning(default : 4355)

SPUStreamDecoder::~SPUStreamDecoder(void)
	{
	if (streaming) EndStreaming(TRUE);

//	DeleteCriticalSection(&lock);
	}

void SPUStreamDecoder::FiberRoutine(void)
	{
	Parse();

	streamingCompleted = TRUE;
	StreamCompleted();
	}

void SPUStreamDecoder::BeginStreaming(bool highQuality)
	{
	terminate = FALSE;
	completing = FALSE;
	streamingCompleted = FALSE;

	SPUDecoder::BeginStreaming(highQuality);

	streaming = TRUE;

	SetFiberPriority(1);
	StartFiber();
	}

void SPUStreamDecoder::StartStreaming(int playbackSpeed)
	{
	SPUDecoder::StartStreaming(playbackSpeed);
	}

void SPUStreamDecoder::StopStreaming(void)
	{
	SPUDecoder::StopStreaming();
	}

void SPUStreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		terminate = TRUE;
		bufferRefillEvent.SetEvent();
		SPUDecoder::EndStreaming(fullReset);
		CompleteFiber();
		FlushBuffer();
		streaming = FALSE;
		}
	}

DWORD SPUStreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	DWORD done;

//	EnterCriticalSection(&lock);

	done = AddData(ptr, num);

	if (AvailBits() > requestedBits) bufferRefillEvent.SetEvent();

//	LeaveCriticalSection(&lock);

	return done;
	}

void SPUStreamDecoder::CompleteData(void)
	{
//	EnterCriticalSection(&lock);

	if (!completing)
		{
		StuffBuffer();
		completing = TRUE;
		bufferRefillEvent.SetEvent();
		}

//	LeaveCriticalSection(&lock);
	}

bool SPUStreamDecoder::InternalIsBufferComplete(void)
	{
	return completing;
	}

bool SPUStreamDecoder::InternalRefillBuffer(int requestBits, bool lowDataPanic)
	{
//	EnterCriticalSection(&lock);

	requestedBits = requestBits;

	while (!terminate && !completing && requestBits > AvailBits())
		{
		RefillRequest();

//		LeaveCriticalSection(&lock);
		bufferRefillEvent.Wait(100);
//		EnterCriticalSection(&lock);
		}

	if (!terminate && !completing && AvailBits() < 8 * SPU_STREAM_BUFFER_SIZE) RefillRequest();

//	LeaveCriticalSection(&lock);

	return !terminate && (completing && AvailBits() > 0 || AvailBits() >= requestBits);
	}

bool SPUStreamDecoder::StreamingCompleted(void)
	{
	return streamingCompleted;
	}

