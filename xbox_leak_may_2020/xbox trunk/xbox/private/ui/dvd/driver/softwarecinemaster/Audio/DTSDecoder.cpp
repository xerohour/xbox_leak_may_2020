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

#include "DTSDecoder.h"
#include "math.h"
#include <memory.h>
#include "library\common\vddebug.h"
#include "..\common\TimedFibers.h"


#if DUMP_FFT_DATA
#include <stdio.h>
FILE * dfile;
#endif

DTSDecoder::DTSDecoder(AudioBitStream * strm,
						PCMWaveOut * pcmWaveOut,
						TimingSlave * timingSlave,
						GenericProfile			*	globalProfile,
						GenericProfile			*	profile)
	{

	this->strm = strm;
	this->pcmWaveOut = pcmWaveOut;
	this->timingSlave = timingSlave;
	this->globalProfile = globalProfile;
	this->profile = profile;

	dtsDigitalOutput = false;
	frameSize = 0;

	ZeroMemory(leftRightBuffer, sizeof(leftRightBuffer));

#if DUMP_FFT_DATA
	dfile = fopen("f:\\DTS\\dmp.txt", "w");
#endif
	}


bool DTSDecoder::ParseSyncInfo(void)
	{
	int newFrameSize;

	strm->ByteAllign();
	strm->PeekBits(8);
	if (!strm->RefillBuffer(95 * 8)) return FALSE;

	while (strm->AvailBits() >= 95 * 8 && (syncword = (DWORD)(strm->PeekBits(32))) != 0x7ffe8001)
		{
		strm->Advance(8);
		if (!strm->RefillBuffer(95 * 8)) return FALSE;
		}

	if (syncword != 0x7ffe8001)
		{
		DP("Strange DTS syncword failure");
		strm->Advance(strm->AvailBits());
		return FALSE;
		}

	frameStart = strm->CurrentLocation();

	strm->Advance(32);

	WORD w3f, ftype, shrt, cpf, nblks, fsize;

	w3f = (WORD)(strm->PeekBits(6));
	ftype = (WORD)(strm->GetBits(1));
	shrt = (WORD)(strm->GetBits(5));
	cpf = (WORD)(strm->GetBits(1));

	nblks = (WORD)(strm->GetBits(7));
	fsize = (WORD)(strm->GetBits(14));

	if (fsize < 94) return FALSE;
	if (fsize > 8193) return FALSE;

	numPCMSamplePairs = (nblks+1) * 32;  //number of audio l/r pairs (*4=bytes)
	switch (numPCMSamplePairs)
		{
		default:
			DP("DTS numPCMSamplePairs unknown %d", numPCMSamplePairs);
		case 2048:
			dtsType = 0x0d; //type III
			break;
		case 1024:
			dtsType = 0x0c; //type II
			break;
		case 512:
			dtsType = 0x0b; //type I
			break;
		}

	newFrameSize = (fsize+1) * 8;

	//chztbd - should add in aux data size

	if (!strm->RefillBuffer(newFrameSize - 66)) return FALSE;

	if (newFrameSize - 66 <= strm->AvailBits())
		{
		frameSize = newFrameSize;

		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}


void DTSDecoder::FrameDTSDigitalOutput(void)
	{
	int i;
	BYTE * dptr = (BYTE *)&encodedBuffer;

	strm->SetCurrentLocation(frameStart);

	//Pa
	*dptr++ = 0x72;
	*dptr++ = 0xf8;
	//Pb
	*dptr++ = 0x1f;
	*dptr++ = 0x4e;
	//Pc
	*dptr++ = dtsType;
	*dptr++ = 0x00;
	//Pd
	*dptr++ = frameSize & 0xff;
	*dptr++ = frameSize >> 8;


	for (i=0; i<frameSize; i+=16)
		{
		WORD w;
		w = (WORD)(strm->GetBits(16));
		*dptr++ = w & 0xff;
		*dptr++ = w >> 8;
		}

	i = (numPCMSamplePairs*4) - 8 - (frameSize>>3);
	memset(dptr, 0, i);
	}



void DTSDecoder::ParseSyncFrame(void)
	{
	int blk, ch, time;
	__int64 frameEnd;

	do {
		time = strm->GetCurrentTimeStamp();

		if (!ParseSyncInfo()) return;

		if (!strm->RefillBuffer(frameSize + 66)) return;

		frameEnd = frameStart + frameSize;

		} while (PeekWordAt((int)(frameEnd - strm->CurrentLocation())) != 0x7ffe);

	if (time != INVALID_TIME_STAMP && time != lastTime)
		{
		lastTime = time;
		currentDataTime = time;
		time -= pcmWaveOut->GetBufferDelayTime();
		if (time >= 0 && timingSlave)
			{
			timingSlave->UpdateTimer(time);
			}
		}

#if DUMP_FFT_DATA
	fprintf(dfile, "FRAME AT %08lx to %08lx\n", (int)frameStart / 8, (int)frameEnd / 8);
#endif

	if (dtsDigitalOutput)
		{
		FrameDTSDigitalOutput();
		pcmWaveOut->AddPCMDigData(leftRightBuffer, encodedBuffer, numPCMSamplePairs, currentDataTime);
		}
	else
		{
		pcmWaveOut->AddPCMData(leftRightBuffer, numPCMSamplePairs, currentDataTime);
		}

	currentDataTime += (1000 * (numPCMSamplePairs)) / 48000;

	if (!hurryUp) YieldTimedFiber(0);


	strm->Advance((int)(frameEnd - strm->CurrentLocation()));

	FrameCnt++;
	}


DTSDecoder::~DTSDecoder(void)
	{
#if DUMP_FFT_DATA
	fclose(dfile);
#endif
	}

void DTSDecoder::InitLocalsNewStream(void)
	{
	hurryUp = FALSE;
	FrameCnt = 0;
	}

void DTSDecoder::BeginStreaming(void)
	{
	pcmWaveOut->BeginStreaming();
	InitLocalsNewStream();
	}

void DTSDecoder::StartStreaming(int playbackSpeed)
	{
	pcmWaveOut->StartStreaming(playbackSpeed);
	}

void DTSDecoder::StopStreaming(void)
	{
	pcmWaveOut->StopStreaming();
	}

void DTSDecoder::EndStreaming(bool fullReset)
	{
	pcmWaveOut->EndStreaming(fullReset);
	}

void DTSDecoder::DetachStreaming(void)
	{
	pcmWaveOut->DetachStreaming();
	}

void DTSDecoder::AttachStreaming(void)
	{
	pcmWaveOut->AttachStreaming();
	InitLocalsNewStream();
	}


void DTSDecoder::SetDigitalOut(AudioTypeConfig dtsAudioType)
	{
	if (pcmWaveOut->SupportsSPDIFDataOut())
		{
		dtsDigitalOutput = dtsAudioType.config.spdifOutMode == SPDIFOM_DEFAULT;
		}
	else
		{
		dtsDigitalOutput = false;
		}

	}

void DTSDecoder::GetDigitalOut(AudioTypeConfig & dtsAudioType)
	{
	dtsAudioType.config.spdifOutMode = dtsDigitalOutput ? SPDIFOM_DEFAULT : SPDIFOM_DECOMPRESSED;
	}


