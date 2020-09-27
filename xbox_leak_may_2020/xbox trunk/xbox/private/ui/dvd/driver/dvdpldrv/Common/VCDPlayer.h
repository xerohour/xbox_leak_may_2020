////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  VCD Player Classes
//
////////////////////////////////////////////////////////////////////

#ifndef VCDPLAYER_H
#define VCDPLAYER_H

#include "Library/Common/WinPorts.h"
#include "Library/Common/KrnlSync.h"
#include "EventSender.h"
#include "DVDTime.h"
#include "Boards/generic/vdgendrv.h"

////////////////////////////////////////////////////////////////////
//
//  VCD Stream Server Class
//
////////////////////////////////////////////////////////////////////

class VCDStreamServer
	{
	public:
		virtual Error PerformTransfer(int maxTransfer = 0xffff) = 0;
      virtual void PlayerCallback(DWORD signalPosition) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  VCD Player Class
//
////////////////////////////////////////////////////////////////////

class VCDPlayer : private WinPortWorkerThread, virtual public EventSender
	{
	protected:
		UnitSet				units, decUnits;
		VCDStreamServer * callback;
		VCDStreamServer * refiller;
		VDLocalMutex		lock;

		DWORD					callbackPosition;

		BOOL playing, scanning, paused;

		void WorkMessage(WPARAM wParam, LPARAM dParam);

		WORD playbackSpeed;

		enum VCDVideoStandard
			{
			VCDVID_NTSC, VCDVID_PAL, VCDVID_UNKNOWN
			} vidStandard;

		enum VCDCodingStandard
			{
			VCDCDS_MPEG1, VCDCDS_MPEG2, VCDCDS_UNKNOWN
			};

		//
		// contains the current audio stream id. When we have single channel audio then the
		//	audio stream id is zero. In dual channel mode the first channel has the audio stream id
		// 0 and the second channel audio stream id 1.
		//
		WORD audioStreamID;

		//
		// Video-CD contains only MPEG-1 coded material (audio and video)
		// Super-Video-CD contains always MPEG-2 video but may contain MPEG-1 or MPEG-2 audio.
		//
		VCDCodingStandard videoCodingStandard;
		VCDCodingStandard audioCodingStandard;

		//HANDLE fileHandle;
	public:
		DWORD	transferCount;

		VCDPlayer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~VCDPlayer(void);

		Error Init(void);

		Error Pause(void);
		Error Resume(void);
		Error Step(void);
		Error SetPlaybackSpeed(WORD speed);
		WORD	GetPlaybackSpeed(void) {return playbackSpeed;}

		BOOL IsPaused(void);
		BOOL IsPreempted(void);

		BOOL SupportsHiResStills(void);

		Error SetAudioStreamChannel(WORD audioStreamID);
		WORD  GetAudioStreamChannel(void);
		Error SetStreamAttributes(BOOL pal, BOOL mpeg1CodingStandard, BOOL mpeg1Audio = TRUE);

		DWORD SendData(HBPTR data, DWORD size);
		DWORD SendDataMultiple(MPEGDataSizePair * data, DWORD size);
		void CompleteData(void);
		void CancelData(void);

		void SetRefiller(VCDStreamServer * refiller) {this->refiller = refiller;}
		void RequestRefill(void);

		void SetCallback(DWORD position, VCDStreamServer * callback);
		void CancelCallback(void);

		BOOL IsCompleted(void);
		void StartPlayback(BOOL toPause = FALSE);
		void StartScan(void);
		void TerminatePlayback(void);

		Error TranslateButtonPosition(int & x, int & y);

		DWORD CurrentLocation(void);

		VideoStandard GetCurrentVideoStandard(void) { return (vidStandard == VCDVID_NTSC ? VSTD_NTSC : VSTD_PAL); }
		int			  GetCurrentFrameRate(void) { return vidStandard == VCDVID_NTSC ? 30 : 25; }

		Error GetPlaybackTime(DVDTime & playbackTime);

		Error GetAudioInformation(MPEG2PrologicStatus & prologicStatus, MPEG2LFEStatus & lfeStatus, AC3AudioCodingMode & ac3Mode);
	};

#endif
