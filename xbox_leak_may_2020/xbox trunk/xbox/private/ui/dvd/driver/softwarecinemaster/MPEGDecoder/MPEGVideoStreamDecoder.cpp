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

#include "MPEGVideoStreamDecoder.h"

#pragma warning(disable : 4355)
MPEGVideoStreamDecoder::MPEGVideoStreamDecoder(GenericPictureDisplay * display,
															  GenericSliceParser * highQualitySliceParser,
			                                      GenericSliceParser * lowQualitySliceParser,
															  GenericProfile * globalProfile,
															  GenericProfile * profile,
															  StreamFlipCopy * streamFlipCopy)
												 : VideoStreamParser(this, this), VideoBitStream(streamFlipCopy),
												   PictureParser(highQualitySliceParser, lowQualitySliceParser, display, this, globalProfile, profile),
													bufferRefillEvent(FALSE, TRUE),
													TimedFiber(10)
	{
	this->display = display;

	highQualitySliceParser->SetBitStream(this);
	lowQualitySliceParser->SetBitStream(this);

//	InitializeCriticalSection(&lock);

	streaming = FALSE;
	}
#pragma warning(default : 4355)

MPEGVideoStreamDecoder::~MPEGVideoStreamDecoder(void)
	{
	if (streaming) EndStreaming(TRUE);

//	DeleteCriticalSection(&lock);
	}

void MPEGVideoStreamDecoder::FiberRoutine(void)
	{
	while (RefillBuffer())
		{
		VideoStreamParser::Parse();
		if (playbackMode == VDPM_SCANNING)
			{
			if (!terminate)
				{
				completing = FALSE;
				FlushBuffer();
				}

			ScanCompleted();
			}
		else
			{
			PictureParser::DoneStreaming();
			if (endCodeReceived)
				{
				endCodeReceived--;
				endCodeCompleted++;
				}
			}
		}

	streamingCompleted = TRUE;
	StreamCompleted();
	}

void MPEGVideoStreamDecoder::PictureDecodingCompleted(void)
	{
	switch (playbackMode)
		{
		case VDPM_SCANNING:
			while (!terminate && !completing)
				{
				RefillRequest();

				bufferRefillEvent.Wait(100);
				}

			if (!terminate)
				{
				completing = FALSE;
				FlushBuffer();
				}

			ScanCompleted();
			break;
		case VDPM_TRICKPLAY:
			break;
		default:
			YieldTimedFiber(0);
		}
	}

bool MPEGVideoStreamDecoder::EnoughDataToStart(void)
	{
	return endCodeReceived > 0 || endCodeCompleted > 0;
	}

void MPEGVideoStreamDecoder::BeginStreaming(VideoPlaybackMode playbackMode, bool highQuality)
	{
	this->playbackMode = playbackMode;

	terminate = FALSE;
	completing = FALSE;
	streamingCompleted = FALSE;

	endCodeReceived = 0;
	endCodeCompleted = 0;

	VideoStreamParser::BeginStreaming(playbackMode);
	PictureParser::BeginStreaming(playbackMode, highQuality);

	streaming = TRUE;

	StartFiber();
	}

void MPEGVideoStreamDecoder::StartStreaming(int playbackSpeed)
	{
	PictureParser::StartStreaming(playbackSpeed);
	}

void MPEGVideoStreamDecoder::StopStreaming(void)
	{
	PictureParser::StopStreaming();
	}

void MPEGVideoStreamDecoder::EndStreaming(bool fullReset)
	{
	if (streaming)
		{
		terminate = TRUE;
		bufferRefillEvent.SetEvent();
		display->EndStreaming(fullReset);
		VideoStreamParser::EndStreaming(fullReset);
		CompleteFiber();
		PictureParser::EndStreaming(fullReset);
		FlushBuffer();
		streaming = FALSE;
		}
	}

void MPEGVideoStreamDecoder::AdvanceFrame(void)
	{
	PictureParser::AdvanceFrame();
	}

bool MPEGVideoStreamDecoder::StreamingCompleted(void)
	{
	return streamingCompleted;
	}

DWORD MPEGVideoStreamDecoder::VideoSegmentRequested(void)
	{
	return VideoStreamParser::VideoSegmentRequested();
	}

void MPEGVideoStreamDecoder::SetFirstVideoSegment(DWORD seg)
	{
	VideoStreamParser::SetFirstVideoSegment(seg);
	}

void MPEGVideoStreamDecoder::SetLastVideoSegment(DWORD seg)
	{
	VideoStreamParser::SetLastVideoSegment(seg);
	}


void MPEGVideoStreamDecoder::AdvanceTrickFrame(int by)
	{
	VideoStreamParser::AdvanceTrickFrame(by);
	}

DWORD MPEGVideoStreamDecoder::SendData(BYTE * ptr, DWORD num)
	{
	DWORD done;
	static BYTE stuff[] = {0x00, 0x00, 0x00, 0x00};

//	EnterCriticalSection(&lock);

	done = AddData(ptr, num);

	if (AvailBits() > requestedBits) bufferRefillEvent.SetEvent();
	if (done == num && num > 4 && *(DWORD*)(ptr + num - 4) == 0xb7010000)
		{
		endCodeReceived++;
		AddData(stuff, 4);
		bufferRefillEvent.SetEvent();
		}

//	LeaveCriticalSection(&lock);

	return done;
	}

void MPEGVideoStreamDecoder::TrickSegmentRequested(void)
	{
	completing = FALSE;
	}

void MPEGVideoStreamDecoder::WaitForSegmentCompleted(void)
	{
	while (!terminate && !completing)
		{
		RefillRequest();
		bufferRefillEvent.Wait(100);
		}
	}

bool MPEGVideoStreamDecoder::IsSegmentCompleted(void)
	{
	return completing;
	}

void MPEGVideoStreamDecoder::CompleteData(void)
	{
//	EnterCriticalSection(&lock);

	if (!completing)
		{
		StuffBuffer();
		completing = TRUE;
		bufferRefillEvent.SetEvent();
		}
	endCodeReceived = 0;

//	LeaveCriticalSection(&lock);
	}

bool MPEGVideoStreamDecoder::InternalIsBufferComplete(void)
	{
	return completing;
	}

bool MPEGVideoStreamDecoder::InternalRefillBuffer(int requestBits, bool lowDataPanic)
	{
//	EnterCriticalSection(&lock);

//	if (AvailBits() == 0) endCodeReceived = FALSE;

	requestedBits = requestBits;

	while (!terminate && !completing && !endCodeReceived && requestBits > AvailBits())
		{
		RefillRequest();

//		LeaveCriticalSection(&lock);
		bufferRefillEvent.Wait(100);
//		EnterCriticalSection(&lock);
		}

	if (!terminate && !completing && AvailBits() < 8 * VIDEO_STREAM_BUFFER_SIZE) RefillRequest();

//	LeaveCriticalSection(&lock);

	return !terminate && ((endCodeReceived || completing) && AvailBits() > 0 || AvailBits() >= requestBits);
	}

