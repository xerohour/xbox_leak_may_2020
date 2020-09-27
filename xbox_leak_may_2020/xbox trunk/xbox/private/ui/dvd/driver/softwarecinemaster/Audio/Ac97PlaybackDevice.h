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


#ifndef Ac97PlaybackDevice_H
#define Ac97PlaybackDevice_H

#include "AudioPlaybackDevice.h"
#include <queue>

#if _DEBUG
	#define XASSERT(x) if( !(x) ) _asm int 3;
#else
	#define XASSERT(x)
#endif

enum PLAY_STATE
	{
	PS_NONE,
	PS_BEGIN,
	PS_START,
	PS_DONE,
	PS_STOP,
	PS_END
	};


class Ac97PlaybackDevice : public GenericAudioPlaybackDevice
	{
	protected:
		XMEDIAPACKET	  * xmpAna, * xmpDig;
		short			 ** bufMemAna, ** bufMemDig;
		DWORD			  * bufCompletedSize;
		DWORD			  * volatile bufStatus;
		bool			  * bufSPDIFData;
		bool			  * bufSingleBuffer;
		DWORD				lastRenderPosition;
		DWORD				bytesWrittenToRender;



		LPAC97MEDIAOBJECT	pAnaRender;
		LPAC97MEDIAOBJECT	pDigRender;
		HANDLE			  * pBufferEvents;
		HANDLE				hNotifyThread;
		DWORD				callBackThreadID;
		PLAY_STATE			playState;
		std::queue <int>	bufferWaitQ;
		int	volatile		bufsPosted;
		int					numBuffers;
		int					bufferCount;
		int					bufferUsed, bufferDropped;
		int					fillBuffer, dropBuffer;
		int					inputByteCount, dropByteCount;
		int					trashDataSent, trashPlayTimeStart;
		int					sampleRate;
		bool				running, terminate, dropping;
		TimedFiberMutex		lock;

		SPDIFCopyMode		spdifCopyMode;
		bool				spdifDeviceMode, spdifDataMode;


		GenericAudioPlaybackCallback * callback;

		friend DWORD WINAPI ThreadProcessDSSNotify(LPVOID);
		void BufferCallbackThread(void);

		Error AllocateBufferCallback(void);
		void ReleaseBufferCallback(void);
		Error AllocateAudioBuffers(void);
		void ReleaseAudioBuffers(void);
		void ResetRenderPosition(void);
		Error OpenDevice(void);
		void CloseDevice(void);

		DWORD GetCurrentRenderPosition(void);
		void FlushAndRequeueUnPlayedData(void);
//		void ReQueueFlushedBuffers(void);
		void ReQueueFlushedBuffers(DWORD flushPos);
		bool WaitForRendererCompletionTimout(int timeout);
		void DumpQueue(void);
		void WriteBufferFromQueue(void);
		Error WriteBuffer(int buffer);

		bool IsBufferDone(int id);
		bool WaitForCompletionTimeout(int timeout);

		void DoCallback(void);
		bool DSndSetSpeed(DWORD dwNewFrequency);
		void ResampleBlock(short * src, short * dst, int sourceNum, int destNum, int & sourceDone, int & destDone);
		void ResampleBlock(short * src, int sourceNum, int & sourceDone);
		void ProcessResampleBuffer(void);
		bool BuffersInQueue(void);

		Error SetSPDIFDeviceOut(bool enable);
		void PostBuffer(int usedSize);

	public:
		Ac97PlaybackDevice(GenericProfile * globalProfile, GenericProfile * profile, int numBuffers, int bufferSize);
		~Ac97PlaybackDevice(void);

		bool SupportsSPDIFDataOut(void);
		bool SupportsVolumeControl(void) {return FALSE;}

		void BeginStreaming(GenericAudioPlaybackCallback * callback);
		void StartStreaming(void);
		void StopStreaming(void);
		void EndStreaming(bool fullReset);
		void DoneStreaming(void);

		bool GetBuffer(int requestSize, short * & buffer, int & size);
		bool GetBuffers(int requestSize, short * & bufA, short * & bufD, int & size);
		void PostBuffer(int usedSize, int time) {PostBuffer(usedSize);}

		int CurrentLocation(void);
		int AvailBufferSpace(void);

		int GetSamplingFrequency(void) {return sampleRate;}
		bool SetPlaybackRate(DWORD rate){return FALSE;}

		void WaitForCompletion(void);
		bool IsCompleted(void);
		Error CheckHardwareResources(void) {if (pAnaRender == NULL) GNRAISE(GNR_NO_AUDIO_HARDWARE); else GNRAISE_OK;}

		bool EnterDropDataMode(void);
		void LeaveDropDataMode(void);
		void DropData(int bytes);

		DWORD GetVolume(void){return 0xffffffff;}
		void SetVolume(DWORD volume) {;}

		Error SetCopyProtectionLevel(SPDIFCopyMode sCopyMode);
		Error SetSPDIFDataOut(bool enable);
	};

#endif
