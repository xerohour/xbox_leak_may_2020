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


#include "LPCMDecoder.h"


void LPCMDecoder::lpf96kpcmBuffer(int qwCnt)
	{
	short * pbuf = (short * )&pcmBuffer;
	// filter 96k data and produce 48k data
	// in this case we are producing one output sample for every 2 input samples
	// therefore we deal with the input in quad and not do interquad shifting.
	__asm
		{
		mov			ecx, [this]
		mov			eax, [qwCnt]

		mov			edi, [pbuf]
		mov			edx, [pbuf]

loop1:
												//   hi			  low
		pshufw		mm0, [edi], 11011000b		// r1, r0		l1, l0
		pshufw		mm2, [edi+8], 11011000b		// r3, r2		l3, l2

		mov			esi, (NTAP/2-1)*8
		pxor		mm7, mm7
		pxor		mm6, mm6

taploop:
		movq		mm4, LPF96FIRTable[esi]
		movq		mm1, [ecx+esi].lastSamples
		movq		[ecx+esi].lastSamples, mm2
		movq		mm3, mm0
		pmaddwd		mm0, mm4
		pmaddwd		mm2, mm4
		sub			esi, 8
		paddd		mm7, mm0
		movq		mm0, mm1
		paddd		mm6, mm2
		movq		mm2, mm3
		jge			taploop

		psrad		mm7, 16-1				// int*int correction
		psrad		mm6, 16-1				// int*int correction
		packssdw	mm7, mm7				// r0,l0		r0,l0
		packssdw	mm6, mm6				// r1,l1		r1,l1
		punpckhdq	mm7, mm6				// r1,l1		r0,l0

		add			edi, 16
		movq		[edx], mm7
		add			edx, 8
		sub			eax, 2					// decrement loop count
		jnz			loop1
		emms
		}
	}


LPCMDecoder::LPCMDecoder(AudioBitStream	*	strm,
			                PCMWaveOut			*	pcmWaveOut,
				             TimingSlave		*	timingSlave)
	{
	this->strm = strm;
	this->pcmWaveOut = pcmWaveOut;
	this->timingSlave = timingSlave;

	channels = 2;
	bitsPerSample = 16;
	samplesPerSecond = 48000;
	}

LPCMDecoder::~LPCMDecoder(void)
	{
	}

void LPCMDecoder::ParseFrame(void)
	{
	int time;
	int smp;
	WORD w;

	time = strm->GetCurrentTimeStamp();

	if (time != INVALID_TIME_STAMP && time != lastTime)
		{
		lastTime = time;
		time -= pcmWaveOut->GetBufferDelayTime();
		if (time >= 0 && timingSlave) timingSlave->UpdateTimer(time);
		}

	if (!strm->RefillBuffer(frameSize + 16, hurryUp)) return;

	if (channels == 1)
		{
		for(smp = 0; smp < targetBufSize; smp+=4)
			{
			w = strm->GetWord();
			pcmBuffer[smp+0] = (short)(w);
			pcmBuffer[smp+1] = (short)(w);
			w = strm->GetWord();
			pcmBuffer[smp+2] = (short)(w);
			pcmBuffer[smp+3] = (short)(w);
			strm->Advance(skip1);
			}
		}
	else if (skip0 || skip1)
		{
		for(smp = 0; smp < targetBufSize; smp+=4)
			{
			pcmBuffer[smp+0] = (short)(strm->GetWord());
			pcmBuffer[smp+1] = (short)(strm->GetWord());
			strm->Advance(skip0);
			pcmBuffer[smp+2] = (short)(strm->GetWord());
			pcmBuffer[smp+3] = (short)(strm->GetWord());
			strm->Advance(skip1);
			}
		}
	else
		{
		for(smp = 0; smp < targetBufSize; smp+=4)
			{
			pcmBuffer[smp+0] = (short)(strm->GetWord());
			pcmBuffer[smp+1] = (short)(strm->GetWord());
			pcmBuffer[smp+2] = (short)(strm->GetWord());
			pcmBuffer[smp+3] = (short)(strm->GetWord());
			}
		}

	if (samplesPerSecond == 96000)
		{
		static bool first = true;

		if (first)
			{
			first = false;
			int i;
			for (i=0; i<targetBufSize; i+=2)
				{
				pcmBuffer[i] = (short)i;
				pcmBuffer[i+1] = 0;
				}
			}
		lpf96kpcmBuffer(80);
		}

	pcmWaveOut->AddPCMData((short *)&pcmBuffer, 80, lastTime);

	}

void LPCMDecoder::SetAudioCodingMode(int samplesPerSecond,
								             int bitsPerSample,
								             int channels)
	{
	this->samplesPerSecond = samplesPerSecond;
	this->bitsPerSample = bitsPerSample;
	this->channels = channels;
	}


void LPCMDecoder::InitLocalsNewStream(void)
	{
	frameSize = samplesPerSecond / 600 * channels * bitsPerSample;

	if (channels > 2)
		skip0 = (channels - 2) * 16;
	else
		skip0 = 0;

	skip1 = skip0 + 2 * (bitsPerSample - 16) * channels;

	if (samplesPerSecond == 44100)
		{
		pcmWaveOut->SetSampleRate(44100);
		}
	else
		pcmWaveOut->SetSampleRate(48000);


	if (samplesPerSecond == 96000)
		targetBufSize = 320;
	else
		targetBufSize = 160;

	ZeroMemory(&lastSamples, sizeof(lastSamples));

	lastTime = -1;
	}


void LPCMDecoder::BeginStreaming(void)
	{
	pcmWaveOut->BeginStreaming();
	InitLocalsNewStream();
	}

void LPCMDecoder::StartStreaming(int playbackSpeed)
	{
	pcmWaveOut->StartStreaming(playbackSpeed);
	}

void LPCMDecoder::StopStreaming(void)
	{
	pcmWaveOut->StopStreaming();
	}

void LPCMDecoder::EndStreaming(bool fullReset)
	{
	pcmWaveOut->EndStreaming(fullReset);
	}

void LPCMDecoder::DetachStreaming(void)
	{
	pcmWaveOut->DetachStreaming();
	}

void LPCMDecoder::AttachStreaming(void)
	{
	pcmWaveOut->AttachStreaming();
	InitLocalsNewStream();
	}
