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

//////////////////////////////////////////////////////////////////////
//
//  DVD Player Class
//
//////////////////////////////////////////////////////////////////////

#ifndef DVDPLAYER_H
#define DVDPLAYER_H

#include "DVDStreamServer.h"
#include "DVDHliCtrl.h"
#include "Boards/Generic/vdgendrv.h"
#include "Library/Common/WinPorts.h"
#include "Library/Common/KrnlSync.h"
#include "DVDTime.h"

class DVDPlayer : public DVDStreamPlayer, public DVDSPUDisplay, private EventPortWorkerThread
	{
	protected:
		UnitSet				units, decUnits;
		DVDStreamServer * callback;
		DVDStreamServer * refiller;
		VDLocalMutex		lock;

		DWORD					callbackPosition;

		BOOL scanning, trickplay, expectingDone, backward, paused;

		void WorkMessage(WPARAM wParam, LPARAM dParam);

		KernelInt64 transferTotal;

		WORD playbackSpeed;
		WORD videoAttr;
		WORD audioStreamID;
		WORD audioAttr;
		WORD subPictureStreamID;
		WORD subPictureAttr;
		BOOL stillFrameSequence;
		DisplayPresentationMode presentationMode;
		VideoStandard videoStandard;

		DVDTime	timeBase;
		BOOL		timeBaseValid;
		LONG		lastDecoderTime;

		volatile BOOL inCallback;

		void PerformSeek(void);

	public:
		DVDPlayer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~DVDPlayer(void);

		Error Pause(void);
		Error Resume(void);
		Error Step(void);
		Error StepBy(int value);
		Error SetPlaybackSpeed(WORD speed);
		WORD	GetPlaybackSpeed(void) {return playbackSpeed;}

		BOOL IsPaused(void);
		BOOL IsPreempted(void);

		void	ExtractAudioAttributes(WORD audioAttr, MPEG2AudioType & type, WORD & channels,
											  DWORD & audioBitsPerSecond, DWORD & samplesPerSecond, DWORD &bitsPerSample);
		Error SetAudioStreamAttributes(WORD audioStreamID, WORD audioAttr);
		Error GetAudioInformation(MPEG2PrologicStatus & prologicStatus, MPEG2LFEStatus & lfeStatus, AC3AudioCodingMode & ac3Mode);
		Error SetSubPictureStreamAttributes(WORD subPictureStreamID, WORD subPictureAttr);
		Error SetStreamAttributes(WORD videoAttr, WORD audioStreamID, WORD audioAttr,
		                          WORD subPictureStreamID, WORD subPictureAttr, DisplayPresentationMode mode);
		Error SetSubPicturePalette(int entry, DWORD color);

		Error				SetVideoStandard(WORD videoAttr);
		VideoStandard	GetCurrentVideoStandard(void) { return videoStandard; }
		int				GetCurrentFrameRate(void)		{ return videoStandard == VSTD_NTSC ? 30 : 25; }

		Error SetAnalogCopyProtectionMode(int mode);

		Error SetCopyManagementMode(GenericCopyManagementInfo mode);

		DWORD SendData(HBPTR data, DWORD size);
		DWORD SendDataMultiple(MPEGDataSizePair * data, DWORD size);
		void CompleteData(void);
		void CancelData(void);

		void SetRefiller(DVDStreamServer * refiller) {this->refiller = refiller;}
		void DeleteRefiller(void) {refiller = NULL;}
		void RequestRefill(void);

		void SetCallback(DWORD position, DVDStreamServer * callback);
		void CancelCallback(void);
		void WaitForCallbackCanceled(void);
		void RequestCallback(DWORD position);
		DWORD CurrentLocation(void);

		BOOL Completed(void);
		void SetStillFrameSequenceMode(BOOL stillFrameSequence);
		void StartPlayback(void);
		void StartScan(void);
		void StartTrickplay(void);
		void SetReversePlayback(BOOL reverse);
		void TerminatePlayback(void);

		Error SetButtonColors(DWORD select, DWORD active);
		Error SetButtonPosition(WORD group, WORD button, WORD x, WORD y, WORD w, WORD h);
		Error SetButtonOff(void);
		Error SetButtonSelect(void);
		Error SetButtonActive(void);

		Error TransferDiskKey(DVDHeaderFile * file);
		Error TransferTitleKey(DVDDataFile * file);
		Error CancelTitleKey(void);

		void	SetTimeBase(DVDTime timeBase);
		Error GetPlaybackTime(DVDTime & playbackTime);
		DWORD GetRequestedVOBUID(void);
		void SetFirstAvailableVOBUID(DWORD id);
		void SetLastAvailableVOBUID(DWORD id);
	};

#endif
