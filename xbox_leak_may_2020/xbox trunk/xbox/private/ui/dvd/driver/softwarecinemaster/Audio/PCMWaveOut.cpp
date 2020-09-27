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


#include "PCMWaveOut.h"
#include "..\common\TimedFibers.h"
#include "..\common\PerformanceMonitor.h"
#include "library\common\vddebug.h"

#define AUDIO_DUMP					1
#define EXT_NAME_FOR_PCM_AUDIO_DUMP	1

#if AUDIO_DUMP
static HANDLE audioStream;
static int audioStreamCount;
#endif

void PCMWaveOut::WaveDataRequest(void)
	{
	event.SetEvent();
	}

PCMWaveOut::PCMWaveOut(GenericProfile * globalProfile, GenericProfile * profile, GenericAudioPlaybackDevice * device, TimingSlave * timingSlave)
	: event(FALSE, TRUE), fiberEvent(FALSE, TRUE), TimedFiber(5)
	{
	this->globalProfile = globalProfile;
	this->profile = profile;
	this->device = device;
	this->timingSlave = timingSlave;

	monoOutput = false;
	inputByteCount = 0;
	running = FALSE;

	sampleRate = device->GetSamplingFrequency();
	SetSampleRate(sampleRate);

	maxAudioBuffer = device->AvailBufferSpace();
	threadRunning = FALSE;

//#if AUDIO_DUMP
//	audioStream = ::CreateFile("f:\\ac3\\strm.pcm",
//							  GENERIC_WRITE,
//							  FILE_SHARE_WRITE,
//							  NULL,
//							  CREATE_ALWAYS,
//							  FILE_ATTRIBUTE_NORMAL,
//							  NULL);

//	if(audioStream == INVALID_HANDLE_VALUE)
//		{
//		int err = ::GetLastError();
//		}
//
//#endif
	}

void PCMWaveOut::SetSampleRate(int rate)
	{
	if (rate != 48000)
		{
		DP("SetSampleRate = %d ", rate);
		PMMSGX("SetSampleRate = %d \n", rate);
		}
	}

PCMWaveOut::~PCMWaveOut(void)
	{
//#if AUDIO_DUMP
//	::CloseHandle(audioStream);
//#endif
	}

#define	WORD_M3DB_DIV2	(WORD)(0.707106781 * 32768.0/2)

MMXShort<4> MMX_WORD_M3DB_DIV2 = {WORD_M3DB_DIV2, WORD_M3DB_DIV2, WORD_M3DB_DIV2, WORD_M3DB_DIV2};

inline void MonoMixMoveBuffer(short * sdp, short * ssp, int numSamplePairs)
	{
	if (!numSamplePairs)
		{
		return;
		}
// mmx mix move is slighty better in release build and much better in debug....
//	short * wSp = (short*)sap;
//	short * wDp = (short*)dp;
//
//	for(int i=0; i<max; i++)
//		{
//		*wDp = (*wSp + *(wSp+1)) * 0.707106781 ;
//		*(wDp+1) = *wDp;
//		wDp += 2;
//		wSp += 2;
//		}

	__asm
		{
		mov			ecx, [numSamplePairs]
		mov			esi, [ssp]
		mov			edi, [sdp]
		mov			eax, ecx
		and			eax, 1
		shr			ecx, 1
		movq		mm7, MMX_WORD_M3DB_DIV2
		jz			noQuad
loop1:
												//   hi			  low
		movq		mm0, [esi]					// r1, l1		r0, l0
		add			esi, 8
		pmaddwd		mm0, mm7					// .7r1+.7l1	.7r0+.7l0
		add			edi, 8
		psrad		mm0, 16-2					// int*int correction + .707*32768.0/2 correction
		packssdw	mm0, mm0					// y1 y0		y1 y0 (saturation)
		pshufw		mm0, mm0, 01010000b			// y1 y1		y0 y0
		dec			ecx
		movq		[edi-8], mm0
		jg			loop1

noQuad:
		cmp			eax, 1
		jl			done
		movd		mm0, [esi]					// xx, xx		r0, l0
		pmaddwd		mm0, mm7					// xx, xx		.7r0+.7l0
		psrad		mm0, 16-2					// int*int correction + .707*32768.0/2 correction
		packssdw	mm0, mm0					// y1 y0		y1 y0 (saturation)
		pshufw		mm0, mm0, 01010000b			// xx xx		y0 y0
		movd		[edi], mm0
done:
		emms
		}
	}



// num = short pairs (*4 = bytes)
void PCMWaveOut::AddPCMData(short * leftRight, int num, int time)
	{
	device->SetSPDIFDataOut(false);
	AddDualData(leftRight, NULL, num, time);
	}

//void PCMWaveOut::AddPCMDualData(short * leftRight, short * leftRight1, int num, int time)
//	{
//	device->SetSPDIFDataOut(false);
//	AddDualData(leftRight, leftRight1, num, time);
//	}

void PCMWaveOut::AddPCMDigData(short * leftRightAna, short * leftRightDig, int num, int time)
	{
	device->SetSPDIFDataOut(true);
	AddDualData(leftRightAna, leftRightDig, num, time);
	}

void PCMWaveOut::AddDualData(short * leftRight, short * leftRightDig, int num, int time)
	{
	int max;
	DWORD * dp, * sap, * sdp;
	DWORD * xp;
	short * anaBufBase, *digBufBase=NULL;
	int bufferSize, bufferFilled;
	int lastTime = 0;
	bool ret;

	sap = (DWORD *)leftRight;
	sdp = (DWORD *)leftRightDig;
	while (num)
		{
		ret = false;
		while (!detaching && !terminate && !ret)
			{
			if (leftRightDig)
				ret = device->GetBuffers(2 * num, anaBufBase, digBufBase, bufferSize);
			else
				ret = device->GetBuffer(2 * num, anaBufBase, bufferSize);

			if (!ret)
				event.WaitForever();
			}

		if (terminate || detaching) return;

		bufferSize >>= 2;

		dp = (DWORD *)anaBufBase;
		xp = dp + bufferSize;

		max = xp - dp;
		if (max > num) max = num;

		if (monoOutput)
			{
			MonoMixMoveBuffer((short *)dp, (short *)sap, max);
			}
		else
			{
			memcpy(dp, sap, max * 4);
			}

		if (digBufBase)
			memcpy(digBufBase, sdp, max * 4);
		num -= max;
		bufferFilled = max;
		inputByteCount += 4 * max;
		sap += max;
		sdp += max;

#if AUDIO_DUMP
		if (audioStream && audioStream != INVALID_HANDLE_VALUE)
			{
			DWORD dummy;
			if (!monoOutput)
				::WriteFile(audioStream, dp, 4 * bufferFilled, &dummy, NULL);
			else
				{
				short tmp[8192];
				for(int i=0; i< bufferFilled; i++)
					{
					tmp[i] = *((short *)dp + 2*i);
					}
					::WriteFile(audioStream, tmp, 2 * bufferFilled, &dummy, NULL);
				}
			}
#endif

		device->PostBuffer(4 * bufferFilled, time);

		lastTime = time;
		time += (int)((float)1000.0*bufferFilled/(float)sampleRate);
		}
	}


void PCMWaveOut::CompleteStreaming(void)
	{
	device->DoneStreaming();
	}

void PCMWaveOut::WaitForCompletion(void)
	{
	device->WaitForCompletion();
	}

int PCMWaveOut::GetBufferDelayTime()
	{
	int outputByteCount;

	if (inputByteCount)
		{
		outputByteCount = device->CurrentLocation() * 4;

		if (sampleRate)
			return ScaleDWord(inputByteCount - outputByteCount, sampleRate, 250);
		else
			return 0;
		}
	else
		return 0;
	}

int PCMWaveOut::GetOutputPosition(void)
	{
	int outputByteCount;

	if (inputByteCount)
		{
		outputByteCount = device->CurrentLocation() * 4;

		if (sampleRate)
			return ScaleDWord(outputByteCount, sampleRate, 250);
		else
			return 0;
		}
	else
		return 0;
	}

void PCMWaveOut::FiberRoutine(void)
	{
	int time, position, target, bufData, delta;

	while (threadRunning)
		{
		if (dropping)
			YieldFiber(10);
		else
			YieldFiber(30);

		if (advanceFrame)
			{
			time = timingSlave->CurrentTime() - dataDropStartTime;
			}
		else
			{
			time = GetInternalTime() - dataDropStartTime;
			}

		if (!threadRunning)
			break;

		position = GetOutputPosition() - dataDropStartPosition;
		target = ScaleLong(time, 0x10000, playbackSpeed);
		if (target > position)
			{
			if (dropping)
				{
				bufData = (maxAudioBuffer - device->AvailBufferSpace()) * 4;
				delta = (target - position) * sampleRate / 250;

				// -delta means (target - position) is really big so clamp it
				if (delta < 0 || delta > bufData)
					delta = bufData;

				device->DropData(delta);
				}
			else
				{
				device->StartStreaming();
				fiberEvent.Wait(target - position);
				device->StopStreaming();
				}
			}
		}
	}


#if EXT_NAME_FOR_PCM_AUDIO_DUMP

#ifndef DLLCALL
#define DLLCALL __declspec(dllexport)
#endif

extern "C" {
DLLCALL Error WINAPI DEBUG_SetAC3DebugName(char * name);
			}

DLLCALL Error WINAPI DEBUG_SetAC3DebugName(char * name)
	{
	if (name)
		{
		audioStream = ::CreateFile(name, // D:\ is actually XE:\ //
								  GENERIC_WRITE,
								  FILE_SHARE_WRITE,
								  NULL,
								  CREATE_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);

		if(audioStream == INVALID_HANDLE_VALUE)
			{
			int err = ::GetLastError();
			}
		}
	else
		{
		audioStream = INVALID_HANDLE_VALUE;
		}
	return S_OK;
	}

#endif



void PCMWaveOut::BeginStreaming(void)
	{
	int i;

	detaching = FALSE;
	running = FALSE;
	threadRunning = FALSE;
	terminate = FALSE;
	dropping = FALSE;
//	resampling = FALSE;
	advanceFrame = false;

	inputByteCount = 0;
	outputByteCount = 0;

	playbackSpeed = 0x10000;
	SetFiberPriority(2);

	device->BeginStreaming(this);

#if AUDIO_DUMP && !EXT_NAME_FOR_PCM_AUDIO_DUMP
	char	buf[80];
	wsprintf(buf,"D:\\Dump\\Pcm%d.pcm", audioStreamCount++);

	audioStream = ::CreateFile(buf, // D:\ is actually XE:\ //
							  GENERIC_WRITE,
							  FILE_SHARE_WRITE,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);

	if(audioStream == INVALID_HANDLE_VALUE)
		{
		int err = ::GetLastError();
		}
#endif

	}

void PCMWaveOut::AdvanceFrame(void)
	{
	if (!advanceFrame)
		{
		StopStreaming();
		advanceFrame = true;

		if (device->EnterDropDataMode())
			{
			dropping = TRUE;
			threadRunning = TRUE;

			device->StartStreaming();

			dataDropStartTime = timingSlave->CurrentTime();
			dataDropStartPosition = GetOutputPosition();

			StartFiber();
			}
		else
			{
			threadRunning = TRUE;
			dataDropStartTime = timingSlave->CurrentTime();
			dataDropStartPosition = GetOutputPosition();

			StartFiber();
			}
		}
	}


void PCMWaveOut::StartStreaming(int playbackSpeed)
	{
	if (advanceFrame)
		{
		advanceFrame = false;
		StopStreaming();
		}

	running = TRUE;
	this->playbackSpeed = playbackSpeed;

	if (playbackSpeed == 0x10000 || device->SetPlaybackRate(playbackSpeed))
		{
		device->StartStreaming();
		}
//	else if (device->EnterResampleMode(playbackSpeed))
//		{
//		resampling = TRUE;
//		device->StartStreaming();
//		}
	else if (device->EnterDropDataMode())
		{
		dropping = TRUE;
		threadRunning = TRUE;

		device->StartStreaming();

		dataDropStartTime = GetInternalTime();
		dataDropStartPosition = GetOutputPosition();

		StartFiber();
		}
	else
		{
		threadRunning = TRUE;
		dataDropStartTime = GetInternalTime();
		dataDropStartPosition = GetOutputPosition();

		StartFiber();
		}
	}

void PCMWaveOut::StopStreaming(void)
	{
	if (dropping)
		{
		threadRunning = FALSE;
		fiberEvent.SetEvent();
		CompleteFiber();

		device->StopStreaming();

		device->LeaveDropDataMode();
		dropping = FALSE;
		}
	else if (threadRunning)
		{
		threadRunning = FALSE;
		fiberEvent.SetEvent();
		CompleteFiber();
		}
//	else if (resampling)
//		{
//		resampling = FALSE;
//		device->StopStreaming();
//		device->LeaveResampleMode();
//		}
	else
		{
		device->SetPlaybackRate(0x10000);
		device->StopStreaming();
		}
	running = FALSE;
	}

void PCMWaveOut::EndStreaming(bool fullReset)
	{
	if (advanceFrame)
		{
		advanceFrame = false;
		StopStreaming();
		}

	terminate = TRUE;
	inputByteCount = 0;
	device->EndStreaming(fullReset);
	WaitForCompletion();
	event.SetEvent();

#if AUDIO_DUMP
	if (audioStream != INVALID_HANDLE_VALUE)
		{
		::CloseHandle(audioStream);
		audioStream = INVALID_HANDLE_VALUE;
		}
#endif
	}

void PCMWaveOut::DetachStreaming(void)
	{
	detaching = TRUE;
	event.SetEvent();
	}

void PCMWaveOut::AttachStreaming(void)
	{
	detaching = FALSE;
	}

void PCMWaveOut::SetSPDIFDataOut(bool enable)
	{
	device->SetSPDIFDataOut(enable);
	}

//void PCMWaveOut::GetSPDIFDataOut(bool &enable)
//	{
//	device->GetSPDIFDataOut(enable);
//	}

void PCMWaveOut::SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
	{
	monoOutput = (spkCfg == AC3SC_10);
	}
