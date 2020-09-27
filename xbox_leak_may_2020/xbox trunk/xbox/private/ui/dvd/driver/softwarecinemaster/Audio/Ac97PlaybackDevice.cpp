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


#include "Ac97PlaybackDevice.h"
#include "..\common\PerformanceMonitor.h"
#include <math.h>
#include "library\common\vddebug.h"

#define AUDIO_DUMP					0
#define	DEBUG_QUEUE					0
#define USE_CPU_CLK_FOR_LOCATION	0
#define DEBUG_LOCATION				0
#define DEBUG_CONTROL				0

#if USE_CPU_CLK_FOR_LOCATION
	int mSecPlayTimeStart;
#endif

#if AUDIO_DUMP
static HANDLE audioStream;
static int audioStreamCount;
#endif

#if DEBUG_LOCATION
static int timeOfLastLoc;
#endif

#if !SILVER
// keep packet event signalling from occuring twice in dup packet mode
void CALLBACK DummyAc97Callback (LPVOID pvStreamContext, LPVOID pvPacketContext, DWORD dwStatus)
	{
	}
#endif

// Description	: This is a thread we use to process notify events from the DS buffer
// Return		: DWORD WINAPI - returns when we are done with DS buffer object
// Argument		: LPVOID param - passed in from CreateThread (this), allows us to call methods
DWORD WINAPI ThreadProcessDSSNotify( LPVOID param )
{
	Ac97PlaybackDevice * dso = (Ac97PlaybackDevice *) param;
	dso->BufferCallbackThread();
	dso->callBackThreadID = NULL;
	return 0;
}

void Ac97PlaybackDevice::BufferCallbackThread(void)
	{
	while(TRUE)
		{
		DWORD   dwEvent;
		dwEvent = WaitForMultipleObjects(numBuffers+1, pBufferEvents, FALSE, INFINITE);
		dwEvent -= WAIT_OBJECT_0;

		// events during flush, can come back in any order (not in posted order)
		if(dwEvent < (DWORD)numBuffers)
			{
#if DEBUG_QUEUE
			DP("Packet Done %d bufCompletedSize %d bufStatus %x", dwEvent, bufCompletedSize[dwEvent], bufStatus[dwEvent]);
#endif
			bufCompletedSize[dwEvent] = 0;
			bufStatus[dwEvent] = XMEDIAPACKET_STATUS_SUCCESS;
			DoCallback();
			bufsPosted--;
			}

		// if we get the last event it's the stop signal
		else if( dwEvent == numBuffers )
			{
			break;
			}
		}
	}


Error Ac97PlaybackDevice::AllocateBufferCallback(void)
	{
	int i;

	pBufferEvents = new HANDLE [numBuffers+1];

	// allocate buffer events
	for (i=0; i<numBuffers+1; i++)
		{
		pBufferEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == pBufferEvents[i])
			{
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			}
		}

	if (NULL ==(hNotifyThread = CreateThread(NULL, 0, ThreadProcessDSSNotify, this, 0, &callBackThreadID)))
		{
		DP("CreateThread Error");
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}

	if (NULL == SetThreadPriority(hNotifyThread, THREAD_PRIORITY_TIME_CRITICAL))
		{
		DP("SetPriorityClass Error");
		//Not a real error
		}

	return S_OK;
	}


void Ac97PlaybackDevice::ReleaseBufferCallback(void)
	{
	if (hNotifyThread)
		{
		SetEvent(pBufferEvents[numBuffers]);
		while (callBackThreadID)
			{
			Sleep(1);
			}
		CloseHandle(hNotifyThread);
		hNotifyThread = NULL;
		}

	if (pBufferEvents)
		{
		for(int i=0; i<numBuffers+1; i++)
			{
			if (pBufferEvents[i]==NULL) break;
			if (CloseHandle(pBufferEvents[i]) == NULL)  break;
			}

		delete[] pBufferEvents;
		}

	}


void Ac97PlaybackDevice::ResetRenderPosition(void)
	{
#if DEBUG_CONTROL
		DP("ResetRenderPosition");
#endif

	if (pAnaRender)
		{
		lastRenderPosition =0;
		bytesWrittenToRender = 0;
		}
	}

Error Ac97PlaybackDevice::OpenDevice(void)
	{
	HRESULT hr;
	int i;

	if (pAnaRender)
		{
		CloseDevice();
		}

#ifdef SILVER

	WAVEFORMATEX	wformat = {WAVE_FORMAT_PCM, 2, sampleRate, sampleRate * 4, 4, 16, 0};
	DSSTREAMDESC    dssd;

	ZeroMemory(&dssd, sizeof(dssd));
	dssd.dwFlags				= DSBCAPS_CTRLVOLUME | DSSTREAMCAPS_ACCURATENOTIFY;
	dssd.dwMaxAttachedPackets	= numBuffers;
	dssd.lpwfxFormat			= &wformat;
	hr = DirectSoundCreateStream(&dssd, &pAnaRender);
	if(FAILED(hr)) GNRAISE(GNR_NO_AUDIO_HARDWARE);

	hr = DirectSoundCreateStream(&dssd, &pDigRender);
	if(FAILED(hr)) GNRAISE(GNR_NO_AUDIO_HARDWARE);
	pDigRender->SetVolume(-10000);

	PMMSGX("DirectSoundCreateStream sample rate %dHz\n", sampleRate);

#else

    hr = Ac97CreateMediaObject(DSAC97_CHANNEL_ANALOG, NULL, NULL, &pAnaRender);
	if(FAILED(hr)) GNRAISE(GNR_NO_AUDIO_HARDWARE);

    hr = Ac97CreateMediaObject(DSAC97_CHANNEL_DIGITAL, NULL, NULL, &pDigRender);
	if(FAILED(hr)) GNRAISE(GNR_NO_AUDIO_HARDWARE);

	spdifDeviceMode = false;
	hr = SetSPDIFDeviceOut(spdifDeviceMode);
	if(FAILED(hr)) GNRAISE(GNR_NO_AUDIO_HARDWARE);

	PMMSGX("Ac97CreateMediaObject\n");

#endif // SILVER


	bufsPosted = 0;

	GNRAISE_OK;
	}


void Ac97PlaybackDevice::CloseDevice(void)
	{
	int i;

	playState = PS_NONE;

	if (pAnaRender)
		{
		WaitForRendererCompletionTimout(5000);

		pDigRender->Release();
		pDigRender = NULL;
		pAnaRender->Release();
		pAnaRender = NULL;
		}
	}


Error Ac97PlaybackDevice::AllocateAudioBuffers(void)
	{
	int i;

	xmpAna = new XMEDIAPACKET[numBuffers];
	xmpDig = new XMEDIAPACKET[numBuffers];
	bufMemAna = new short *[numBuffers];
	bufMemDig = new short *[numBuffers];
	bufStatus = new DWORD[numBuffers];
	bufCompletedSize = new DWORD[numBuffers];
	bufSPDIFData = new bool [numBuffers];
	bufSingleBuffer = new bool [numBuffers];

	ZeroMemory(xmpAna, sizeof(XMEDIAPACKET) * numBuffers);
	ZeroMemory(xmpDig, sizeof(XMEDIAPACKET) * numBuffers);

	for(i=0; i<numBuffers; i++)
		{
//		bufMemAna[i] = new short[bufferCount * 2];
//		bufMemDig[i] = new short[bufferCount * 2];
        bufMemAna[i] = (short*) XPhysicalAlloc( sizeof( short ) * bufferCount * 2, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE );
        bufMemDig[i] = (short*) XPhysicalAlloc( sizeof( short ) * bufferCount * 2, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE );
		xmpAna[i].pvBuffer = (char *)bufMemAna[i];
		xmpDig[i].pvBuffer = (char *)bufMemDig[i];
		xmpAna[i].dwMaxSize = 4 * bufferCount;
		xmpAna[i].pdwStatus = &bufStatus[i];
		bufStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;
		xmpAna[i].pdwCompletedSize  = &bufCompletedSize[i];
		bufCompletedSize[i] = 0;
		xmpAna[i].hCompletionEvent = pBufferEvents[i];
		bufSPDIFData[i] = false;
		bufSingleBuffer[i] = false;
		}

	GNRAISE_OK;
	}

void Ac97PlaybackDevice::ReleaseAudioBuffers(void)
	{
	int i;

	for(i = 0;i<numBuffers; i++)
		{
//	    delete[] bufMemAna[i];
//	    delete[] bufMemDig[i];
        XPhysicalFree( bufMemAna[i] );
        XPhysicalFree( bufMemDig[i] );
		}

	delete[] xmpAna;
	delete[] xmpDig;
	delete[] bufMemAna;
	delete[] bufMemDig;
	delete[] bufStatus;
	delete[] bufCompletedSize;
	delete[] bufSPDIFData;
	delete[] bufSingleBuffer;
}


Ac97PlaybackDevice::Ac97PlaybackDevice(GenericProfile * globalProfile, GenericProfile * profile, int numBuffers, int bufferSize)
	{
	this->numBuffers = numBuffers;
	bufferCount = bufferSize;			// bufferCount = number of audio frames (Left/Right word pairs)
	bufferUsed = 0;

	playState = PS_NONE;
	bufsPosted = 0;

	fillBuffer = 0;
	inputByteCount = 0;
	callback = NULL;
	pAnaRender = NULL;
	pDigRender =NULL;
	spdifDataMode = false;

	dropping = FALSE;
	sampleRate = 48000;

	if (FAILED(AllocateBufferCallback()))
		DP("AllocateBufferCallback FAILED");

	if (FAILED(AllocateAudioBuffers()))
		DP("AllocateAudioBuffers FALIED");

	if (FAILED(OpenDevice()))
		{
		DP("Ac97PlaybackDevice::Ac97PlaybackDevice OpenDevice() FAILED");
		PMMSGX("Ac97PlaybackDevice::Ac97PlaybackDevice OpenDevice() FAILED");
		XASSERT(NULL);
		}


//#if AUDIO_DUMP
//	audioStream = ::CreateFile("f:\\ac3\\strm.pcm",
//							  GENERIC_WRITE,
//							  FILE_SHARE_WRITE,
//							  NULL,
//							  CREATE_ALWAYS,
//							  FILE_ATTRIBUTE_NORMAL,
//							  NULL);
//#endif
	}

Ac97PlaybackDevice::~Ac97PlaybackDevice(void)
	{
	CloseDevice();

	ReleaseBufferCallback();

	ReleaseAudioBuffers();

//#if AUDIO_DUMP
//	::CloseHandle(audioStream);
//#endif
	}


void Ac97PlaybackDevice::DumpQueue(void)
	{
	while (bufferWaitQ.size())
		{
		bufStatus[bufferWaitQ.front()] = XMEDIAPACKET_STATUS_SUCCESS;
#if DEBUG_QUEUE
		DP("DumpQueue bufferWaitQ.pop(%d)", bufferWaitQ.front());
#endif
		bufferWaitQ.pop();
		}
	}

DWORD Ac97PlaybackDevice::GetCurrentRenderPosition(void)
	{
	DWORD rPos;

	pAnaRender->GetCurrentPosition(&rPos);

	if (rPos < lastRenderPosition)
		{
		DP("rPos < lastRenderPosition,  rPos %d  lastRenderPosition %d delta",rPos, lastRenderPosition, lastRenderPosition-rPos);

		int i;
		for (i=0; i<5; i++)
			{
			pAnaRender->GetCurrentPosition(&rPos);
			if (rPos >= lastRenderPosition)
				{
				DP("rPos %d is now good retry %d",rPos, i);
				break;
				}
			Sleep(1);
			}
		}

	if (rPos > bytesWrittenToRender)
		{
		if (bytesWrittenToRender)
			{
			DP("rPos %d > bytesWrittenToRender %d", rPos , bytesWrittenToRender);
			}
		rPos = bytesWrittenToRender;
		}

	if (rPos < lastRenderPosition)
		{
		DP("lastRenderPosition %d, rPos %d, diff %d, bytesWrittenToRender %d ",
			lastRenderPosition, rPos, lastRenderPosition-rPos, bytesWrittenToRender);

		// render has run out of data and reset counter to zero so
		// account for byte which were rendered but won't show up in the position
#if DEBUG_CONTROL
		DP("GetCurrentRenderPosition ->Flush");
#endif
		dropByteCount += bytesWrittenToRender;
		if(pDigRender)
			pDigRender->Flush();
		pAnaRender->Flush();
		WaitForRendererCompletionTimout(100);
		ResetRenderPosition();
		}

	lastRenderPosition = rPos;

	return rPos;
	}


void Ac97PlaybackDevice::WriteBufferFromQueue(void)
	{

	if (playState == PS_START && !dropping)
		{
		while (bufferWaitQ.size())
			{
			int j, i = bufferWaitQ.front();

			xmpDig[i].dwMaxSize = xmpAna[i].dwMaxSize;
			if (bufSingleBuffer[i])
				{
				xmpDig[i].pvBuffer = (char *)bufMemAna[i];
				}
			else
				{
				xmpDig[i].pvBuffer = (char *)bufMemDig[i];
				}

			// there seems to be a long enough delay to set this here instead of in callback
			if (bufSPDIFData[i] != spdifDeviceMode)
				{
				SetSPDIFDeviceOut(bufSPDIFData[i]);
				}

			pDigRender->Process(&xmpDig[i], NULL);
			pAnaRender->Process(&xmpAna[i], NULL);
			bufsPosted++;
			// now that data is safely written check to see if we ran out of data
			bytesWrittenToRender += xmpAna[i].dwMaxSize;
#if DEBUG_QUEUE
			DP("bufferWaitQ.pop(%d)",i);
#endif
			bufferWaitQ.pop();
			}
		}
	}

Error Ac97PlaybackDevice::WriteBuffer(int buffer)
	{
	if (!xmpAna[buffer].dwMaxSize)
		{
#if DEBUG_CONTROL
		DP("Trying to write Zero length!!!");
#endif
		return(S_OK);
		}

	bufStatus[buffer] = XMEDIAPACKET_STATUS_PENDING;
	bufSPDIFData[buffer] = spdifDataMode;
//	DP("bufSPDIFData[%d] = %d", buffer, bufSPDIFData[buffer]);

#if DEBUG_QUEUE
	DP("bufferWaitQ.push(%d)",buffer);
#endif
	bufferWaitQ.push(buffer);
	WriteBufferFromQueue();

	return (S_OK);
	}


void Ac97PlaybackDevice::BeginStreaming(GenericAudioPlaybackCallback * callback)
	{
	lock.Enter();
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::BeginStreaming");
#endif

#if DEBUG_LOCATION
	timeOfLastLoc = 0;
#endif

	this->callback = callback;

	running = FALSE;
	terminate = FALSE;
	bufferUsed = 0;
	fillBuffer = 0;
	dropping = FALSE;

	inputByteCount = 0;
	dropByteCount = 0;
	ResetRenderPosition();

	trashDataSent = 0;
	trashPlayTimeStart = 0;
	playState = PS_BEGIN;


	XASSERT(!bufsPosted);
	bufsPosted = 0;

#if AUDIO_DUMP
	char	buf[80];
	wsprintf(buf,"D:\\Dump\\dsdump%d.pcm", audioStreamCount++);

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


	lock.Leave();
	}

void Ac97PlaybackDevice::StartStreaming(void)
	{
	lock.Enter();
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::StartStreaming+");
#endif

	playState = PS_START;

	running = TRUE;
	if (pAnaRender)
		{
		WriteBufferFromQueue();
		}
	else
		trashPlayTimeStart = GetInternalTime() - trashPlayTimeStart;

#if USE_CPU_CLK_FOR_LOCATION
	mSecPlayTimeStart = GetInternalTime();
#endif

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::StartStreaming-");
#endif
	lock.Leave();
	}


void Ac97PlaybackDevice::ReQueueFlushedBuffers(DWORD flushPos)
	{
	int buf, bufCnt, bytes;

	bytes = inputByteCount - flushPos;

	if (bytes > numBuffers * bufferCount * 4)
		{
		DP("ReQueueFlushedBuffers() bytes impossibly large %d", bytes);
		// this is an error, for now just plop down in the middle of the buffer
		bytes = numBuffers * bufferCount * 4 - bufferCount*2;
		}

#if DEBUG_CONTROL
	DP("ReQueueFlushedBuffers() bytes %d", bytes);
#endif

	if (!bytes || bytes < 0)
		return;


	bytes &= ~3;

#if DEBUG_CONTROL
	DP("inputByteCount %d flushPos %d, bytes %d, fillBuffer %d",inputByteCount, flushPos, bytes, fillBuffer);
#endif

	if (fillBuffer)
		{
		xmpAna[bufferUsed].dwMaxSize = fillBuffer;
		fillBuffer = 0;
		bufferUsed = (bufferUsed + 1) % numBuffers;
		}

	buf = bufferUsed;
	bufCnt = 0;

	while (bytes > 0)
		{
		buf = (buf + (numBuffers - 1)) % numBuffers;
		bytes -= xmpAna[buf].dwMaxSize;
		bufCnt++;
		}
	bytes =- bytes;	//create buffer pointer

#if DEBUG_QUEUE
	DP("Was in buffer %d",buf);
#endif

	if (bytes)
		{
		LPBYTE lpDst;
		lpDst = (LPBYTE)xmpAna[buf].pvBuffer;
		memcpy(lpDst, lpDst+bytes, xmpAna[buf].dwMaxSize-bytes);
		lpDst = (LPBYTE)xmpDig[buf].pvBuffer;
		memcpy(lpDst, lpDst+bytes, xmpAna[buf].dwMaxSize-bytes);
		xmpAna[buf].dwMaxSize = xmpAna[buf].dwMaxSize-bytes;
		}

	while (bufCnt)
		{
		WriteBuffer(buf);
		buf = (buf + 1) % numBuffers;
		bufCnt--;
		}
	}


bool Ac97PlaybackDevice::WaitForRendererCompletionTimout(int timeout)
	{
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForRendererCompletionTimout+");
#endif

	timeout += GetInternalTime();

	if (pAnaRender)
		{
		while (bufsPosted && GetInternalTime() < timeout)
			{
#if DEBUG_CONTROL
			DP("WaitForRendererCompletionTimout Yield");
#endif
			YieldTimedFiber(20);
			}
		}

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForRendererCompletionTimout-");
#endif
	XASSERT(bufsPosted==0);

	return (bufsPosted==0);
	}


void Ac97PlaybackDevice::FlushAndRequeueUnPlayedData(void)
	{
	DWORD flushPos;

	flushPos = CurrentLocation() * 4;
#if DEBUG_CONTROL
	DP("->Flush");
#endif
	if(pDigRender)
		pDigRender->Flush();
	pAnaRender->Flush();
	WaitForRendererCompletionTimout(100);
	ResetRenderPosition();
	dropByteCount = flushPos;
	ReQueueFlushedBuffers(flushPos);
	}


void Ac97PlaybackDevice::StopStreaming(void)
	{
	lock.Enter();
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::StopStreaming");
#endif

	playState = PS_STOP;

	if (pAnaRender)
		{
		if (!dropping)
			{
			FlushAndRequeueUnPlayedData();
			}
		}
	else
		{
		trashPlayTimeStart = GetInternalTime() - trashPlayTimeStart;
		}

	running = FALSE;

	lock.Leave();
	}


void Ac97PlaybackDevice::EndStreaming(bool fullReset)
	{
	int i;
	lock.Enter();
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::EndStreaming+");
#endif

	playState = PS_END;

	FlushAndRequeueUnPlayedData();
	DumpQueue();

	this->callback = NULL;
	terminate = TRUE;

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::EndStreaming-");
#endif

#if AUDIO_DUMP
	::CloseHandle(audioStream);
#endif

	lock.Leave();
	}




void Ac97PlaybackDevice::DoneStreaming(void)
	{
	lock.Enter();
#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::DoneStreaming+");
#endif

//	playState = PS_DONE;  // continue normal posting of data (PS_START state)

	if (fillBuffer && pAnaRender)
		{
		xmpAna[bufferUsed].dwMaxSize = fillBuffer;
		if (!dropping)
			WriteBuffer(bufferUsed);
		else
			bufStatus[bufferUsed] = XMEDIAPACKET_STATUS_PENDING;
		fillBuffer = 0;
		bufferUsed = (bufferUsed + 1) % numBuffers;
		}

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::DoneStreaming-");
#endif
	lock.Leave();
	}


bool Ac97PlaybackDevice::IsBufferDone(int id)
	{
	return !pAnaRender || ((DWORD)bufStatus[id] == XMEDIAPACKET_STATUS_SUCCESS);
	}



bool Ac97PlaybackDevice::GetBuffer(int requestSize, short * & buffer, int & size)
	{
	short * bufTemp;
	bool ret = GetBuffers(requestSize, buffer, bufTemp, size);
	bufSingleBuffer[bufferUsed] = true;
	return ret;
	}

bool Ac97PlaybackDevice::GetBuffers(int requestSize, short * & bufA, short * & bufD, int & size)
	{
	static short trash[512];

	lock.Enter();

	bufSingleBuffer[bufferUsed] = false;

	if (!terminate)
		{
		if (pAnaRender)
			{
			if (IsBufferDone(bufferUsed))
				{
				size = 4 * bufferCount - fillBuffer;
				bufA = (short *)((char *)bufMemAna[bufferUsed] + fillBuffer);
				bufD = (short *)((char *)bufMemDig[bufferUsed] + fillBuffer);

				lock.Leave();

				return TRUE;
				}
			else
				{
				lock.Leave();

				return FALSE;
				}
			}
		else
			{
			bufA = trash;
			bufD = trash;
			size = sizeof(trash);

			lock.Leave();

			return TRUE;
			}
		}
	else
		{
		lock.Leave();

		return FALSE;
		}
	}




void Ac97PlaybackDevice::PostBuffer(int usedSize)
	{
	lock.Enter();

	if (pAnaRender)
		{
		inputByteCount += usedSize;
		fillBuffer += usedSize;

		if (fillBuffer == 4 * bufferCount)
			{
			if (terminate)
				{
				lock.Leave();
				return;
				}

#if AUDIO_DUMP
			DWORD dummy;
			::WriteFile(audioStream, bufMemAna[bufferUsed], fillBuffer, &dummy, NULL);
#endif

			xmpAna[bufferUsed].dwMaxSize = fillBuffer;

			if (!dropping)
				{
				WriteBuffer(bufferUsed);
				}
			else
				{
				bufStatus[bufferUsed] = XMEDIAPACKET_STATUS_PENDING;
				}

			fillBuffer = 0;
			bufferUsed = (bufferUsed + 1) % numBuffers;
			}
		}
	else
		{
		trashDataSent += usedSize;
		}

	lock.Leave();
	}

int Ac97PlaybackDevice::CurrentLocation(void)
	{
	DWORD outputByteCount;

#if DEBUG_LOCATION
	bool showLoc = false;

	if (GetInternalTime() > timeOfLastLoc+200)
		{
		showLoc = true;
		timeOfLastLoc = GetInternalTime();
		}
#endif

	if (pAnaRender)
		{
		if (inputByteCount)
			{
			if (dropping)
				{
#if DEBUG_LOCATION
				if (showLoc)
					DP("CurrentLocation %d", dropByteCount >> 2);
#endif
				return dropByteCount >> 2;
				}
			else
				{
				int i;

#if USE_CPU_CLK_FOR_LOCATION
				outputByteCount = (GetInternalTime() - mSecPlayTimeStart) * 192;
#else
				outputByteCount = GetCurrentRenderPosition();
//chzTest
//				static int posAdd = 0;
//				if (outputByteCount)
//					{
//					outputByteCount += posAdd;
//					}

				if (outputByteCount > (DWORD)inputByteCount)
					{
					DP("outputByteCount %d > inputByteCount %d", outputByteCount , inputByteCount);
					outputByteCount = inputByteCount;
					}
#endif

#if DEBUG_LOCATION
				if (showLoc)
					DP("CurrentLocation %d, outputByteCount %d, dropByteCount %d, inputByteCount %d",
						(outputByteCount + dropByteCount)/4, outputByteCount, dropByteCount, inputByteCount);
#endif
				return ((outputByteCount + dropByteCount) >> 2);
				}
			}
		else
			return 0;
		}
	else
		{
		int time;

		if (running)
			time = GetInternalTime() - trashPlayTimeStart;
		else
			time = trashPlayTimeStart;

#if DEBUG_LOCATION
		if (showLoc)
			DP("CurrentLocation %d", ScaleDWord(time, 1000, sampleRate));
#endif
		return ScaleDWord(time, 1000, sampleRate);
		}
	}

int Ac97PlaybackDevice::AvailBufferSpace(void)
	{
	if (pAnaRender)
		return numBuffers * bufferCount - (inputByteCount >> 2) + CurrentLocation();
	else
		return 4096;
	}


bool Ac97PlaybackDevice::WaitForCompletionTimeout(int timeout)
	{
	int i;

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForCompletionTimeout+");
#endif

	timeout += GetInternalTime();

	if (pAnaRender)
		{
		for(i=0; i<numBuffers; i++)
			{
			while (!IsBufferDone(i) && GetInternalTime() < timeout)
				{
				YieldTimedFiber(20);
				}

			if (!IsBufferDone(i))
				{
				DP("Ac97PlaybackDevice::WaitForCompletionTimeout- ret FALSE");
				return FALSE;
				}
			}
		}

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForCompletionTimeout-");
#endif
	return TRUE;
	}

void Ac97PlaybackDevice::WaitForCompletion(void)
	{
	int i;

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForCompletion+");
#endif

	if (pAnaRender)
		{
		for(i=0; i<numBuffers; i++)
			{
			while (!IsBufferDone(i))
				YieldTimedFiber(100);
			}
		}

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::WaitForCompletion-");
#endif
	}

bool Ac97PlaybackDevice::IsCompleted(void)
	{
	int i;

	if (pAnaRender)
		{
		for(i=0; i<numBuffers; i++)
			{
			if (!IsBufferDone(i)) return FALSE;
			}
		}

	return TRUE;
	}

bool Ac97PlaybackDevice::EnterDropDataMode(void)
	{
	lock.Enter();
#if DEBUG_CONTROL
	DP("EnterDropDataMode+");
#endif

	if (!terminate && !running && pAnaRender)
		{
		DoneStreaming();

		if (!IsCompleted())
			{
			dropByteCount = CurrentLocation() * 4;
			dropping = TRUE;

			if (bufsPosted)
				{
				FlushAndRequeueUnPlayedData();
				}

			DumpQueue();
			ResetRenderPosition();

			bufferDropped = bufferUsed;
			dropBuffer = inputByteCount - dropByteCount;

			do {
				bufferDropped = (bufferDropped + (numBuffers - 1)) % numBuffers;
				dropBuffer -= xmpAna[bufferDropped].dwMaxSize;
				bufStatus[bufferDropped] = XMEDIAPACKET_STATUS_PENDING;

				} while (dropBuffer > 0);

			dropBuffer = - dropBuffer;

			if (dropByteCount & 3)
				{
				dropBuffer += 4 - (dropByteCount & 3);
				dropByteCount += 4 - (dropByteCount & 3);
				}

#if DEBUG_CONTROL
			DP("EnterDropDataMode dropBuffer=%d, bufferDropped=%d bufferUsed=%d inputByteCount=%d dropByteCount=%d",
				dropBuffer, bufferDropped, bufferUsed, inputByteCount, dropByteCount);
#endif
			}
		else
			{
			// nothing posted in render or queued
			ResetRenderPosition();

			bufferDropped = bufferUsed;
			dropBuffer = 0;

			dropByteCount = CurrentLocation() * 4;
			dropping = TRUE;
			}

		lock.Leave();
#if DEBUG_CONTROL
			DP("EnterDropDataMode-");
#endif
		return TRUE;
		}
	else
		{
		lock.Leave();
#if DEBUG_CONTROL
			DP("EnterDropDataMode-");
#endif
		return FALSE;
		}
	}

void Ac97PlaybackDevice::LeaveDropDataMode(void)
	{
	lock.Enter();
	int pos, max;

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::LeaveDropDataMode+");
#endif

	if (dropping)
		{
		DoneStreaming();

		max = numBuffers;
		pos = 0;
		while (max && !IsBufferDone(bufferDropped))
			{
			if (dropBuffer)
				{
				LPBYTE p;
				p = (BYTE *)bufMemAna[bufferDropped];
				memcpy(p, p + dropBuffer,  xmpAna[bufferDropped].dwMaxSize - dropBuffer);
				p = (BYTE *)bufMemDig[bufferDropped];
				memcpy(p, p + dropBuffer,  xmpAna[bufferDropped].dwMaxSize - dropBuffer);
				xmpAna[bufferDropped].dwMaxSize -= dropBuffer;
				dropBuffer = 0;
				}
			bufStatus[bufferDropped] = XMEDIAPACKET_STATUS_SUCCESS;
			WriteBuffer(bufferDropped);
			pos += xmpAna[bufferDropped].dwMaxSize;
			bufferDropped = (bufferDropped + 1) % numBuffers;
			max--;
			}

		dropping = FALSE;
		}

#if DEBUG_CONTROL
	DP("Ac97PlaybackDevice::LeaveDropDataMode-");
#endif
	lock.Leave();
	}


void Ac97PlaybackDevice::DropData(int bytes)
	{
	lock.Enter();

	if (dropping)
		{

		if (dropByteCount + bytes > inputByteCount)
			{
#if DEBUG_CONTROL
			DP("dropByteCount+bytes %d > inputByteCount %d", dropByteCount+bytes, inputByteCount);
#endif
			bytes = inputByteCount - dropByteCount;
			}

		bytes &= ~3;

		dropByteCount += bytes;
		dropBuffer += bytes;

		while (!IsBufferDone(bufferDropped) && dropBuffer >= (int)(xmpAna[bufferDropped].dwMaxSize))
			{
			dropBuffer -= xmpAna[bufferDropped].dwMaxSize;
			bufStatus[bufferDropped] = XMEDIAPACKET_STATUS_SUCCESS;
			bufferDropped = (bufferDropped + 1) % numBuffers;

			if (callback) callback->WaveDataRequest();

#if DEBUG_CONTROL
			DP("DropData bufferDropped=%d bufferUsed=%d inputByteCount=%d dropByteCount=%d dropByteCount-dropBuffer %d",
				bufferDropped, bufferUsed, inputByteCount, dropByteCount, dropByteCount-dropBuffer);
#endif
			}

		}

	lock.Leave();
	}


void Ac97PlaybackDevice::DoCallback(void)
	{
	if (!dropping)
		{
		if (callback)
			callback->WaveDataRequest();
		}
	}

Error Ac97PlaybackDevice::SetCopyProtectionLevel(SPDIFCopyMode sCopyMode)
	{
	spdifCopyMode = sCopyMode;
#if DEBUG_CONTROL
	DP("Setting spdifCopyMode %d", spdifCopyMode);
#endif
	GNRAISE_OK;
	}


Error Ac97PlaybackDevice::SetSPDIFDeviceOut(bool data)
	{
	Error hr;

	hr = pDigRender->SetMode(data ? DSAC97_MODE_ENCODED : DSAC97_MODE_PCM);
	spdifDeviceMode = data;
	return hr;
	}

Error Ac97PlaybackDevice::SetSPDIFDataOut(bool enable)
	{
	Error hr = S_OK;

	if (enable != spdifDataMode)
		{
#if DEBUG_CONTROL
		DP("SetSPDIFDataOut(%d), spdifDataMode %d", enable, spdifDataMode);
#endif

		if (!bufsPosted && bufferWaitQ.empty())
			{
#if DEBUG_CONTROL
			DP("bufsPosted %d, bufferWaitQ.empty() %d", bufsPosted, bufferWaitQ.empty());
#endif
			hr = SetSPDIFDeviceOut(enable);
			}
		else
			{
			// we aren't really done but we will have a new data type
			// so write the current buffer
			DoneStreaming();
#if DEBUG_CONTROL
			DP("SetSPDIFDataOut(%d) delayed, bufferUsed %d(new data in this buffer)",enable, bufferUsed);
#endif
			}
		spdifDataMode = enable;
		}

	return hr;
	}


bool Ac97PlaybackDevice::SupportsSPDIFDataOut(void)
	{
	return (XGetAudioFlags() & (DSSPEAKER_ENABLE_AC3 | DSSPEAKER_ENABLE_DTS)) != 0;
	}
