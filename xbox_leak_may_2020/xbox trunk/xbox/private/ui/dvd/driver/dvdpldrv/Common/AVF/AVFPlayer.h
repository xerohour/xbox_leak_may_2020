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

////////////////////////////////////////////////////////////////////////////////
//
//  Audio/Video File Player Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef AVFPLAYER_H
#define AVFPLAYER_H

class AVFPlayer;

#include "Library/Common/Prelude.h"
#include "Library/Common/WinPorts.h"
#include "Library/Common/KrnlSync.h"
#include "boards/generic/vdgendrv.h"

#include "Driver/Dvdpldrv/Common/DVDTime.h"
#include "Driver/Dvdpldrv/Common/EventSender.h"
#include "AVFStreamServer.h"

#include "Library/hardware/mpeg2dec/generic/mp2eldec.h"

#ifdef _DEBUG
#include "Library/Support/Shell/ShellToolKit.h"
#endif

class AVFPlayer : private WinPortWorkerThread, virtual public EventSender
	{
	protected:
		DWORD					totalBytesSent;
		WORD					playbackSpeed;
		UnitSet				units;
		UnitSet				decUnits;

		AVFStreamServer * refiller;		// Stream Server for refill
		VDLocalMutex		decoderLock;	// To serve and to protect the decoder

		void	WorkMessage(WPARAM wParam, LPARAM dParam);
		Error SetCallbackPositionLocked(DWORD position);
		void SetTotalBytesSent(DWORD count) { totalBytesSent = count; }
		void AddToTotalBytesSent(DWORD add) { totalBytesSent += add; }

	public:
		AVFPlayer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		~AVFPlayer(void);

		Error Init(void);

		//
		// Playback control
		//

		Error	StartPlayback(BOOL playRequest=TRUE, BOOL reduceVolume=FALSE, BOOL mute=FALSE);
		Error	StopPlayback(void);
		Error	PausePlayback(void);
		Error	ResumePlayback(void);
		Error	SetPlaybackSpeed(WORD speed);
		WORD	GetPlaybackSpeed(void) {return playbackSpeed;}

		Error SetAudioStreamAttributes(AVFInfo *pAttr);

		//
		// Player to decoder routines
		//

		DWORD	SendData(HBPTR data, DWORD size);
		DWORD SendDataMultiple(MPEGDataSizePair * data, DWORD size);
		void	CompleteData(void);
		void	CancelData(void);
		DWORD GetTotalBytesSent(void) { return totalBytesSent; }

		//
		// Callback routines
		//

		Error	SetCallbackPosition(DWORD position);
		void	CancelCallback(void);
		void	SetRefiller(AVFStreamServer * refiller) {this->refiller = refiller;}

		//
		// Misc
		//

		DWORD	GetPlaybackPosition(void);
		BOOL	IsPreempted(void);

		//
		// Copy protection / management
		//

		Error SetCopyManagementMode(GenericCopyManagementInfo mode);
		Error GetAudioInformation(MPEG2PrologicStatus & prologicStatus, MPEG2LFEStatus & lfeStatus, AC3AudioCodingMode & ac3Mode);
#ifdef _DEBUG
		ShellToolKit	shell;
#endif
	};

#endif

