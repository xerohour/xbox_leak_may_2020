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

#ifndef VCDPLAYSEQ_H
#define VCDPLAYSEQ_H

#include "VCDStreamServer.h"
#include "VCDHeader.h"
#include "EventSender.h"
#include "Breakpoint.h"

class VCDPlayListSequencer : public VCDStreamSequencer, virtual public ERSBreakpointControl
	{
	protected:
		VCDFileSystem	*	vcdfs;
		VCDInfo        *	info;
		VCDEntries		*	entries;
		VCDTracks		*	tracks;
		VCDSearch		*	search;
		VCDPlayList		*	list;
		VCDDataFile		*	file;

		int					currentItem;			// Index of current play item withing playlist
		WORD					repeatCount;
		WORD					waitTime;
		WORD					currentTrack;
		WORD					currentSegment;
		int					scanSpeed;				// Please use SetScanSpeed method to change this
		WORD					numberOfTitles;
		BOOL					playAllTracks;
		BOOL					playingAudioTrack;	// TRUE while playing CDDA track
		DWORD					currentDurationInMilliSecs;

		enum BreakpointState { BPS_NONE, BPS_NORMAL, BPS_EOT } bpReached;

		// WinPort functions.
		virtual void Message (WPARAM wParam, LPARAM lParam);

		enum PlsState {pls_idle, pls_stopped, pls_playing, pls_autopause, pls_still, pls_scanning, pls_paused} state;	// Please use SetPlaybackMode method to change this
		inline void SetPlaybackMode(PlsState newState);
		inline void SetScanSpeed(int newSpeed);

		// VCDStreamSequencer functions.
		virtual void TransferCompleted(void);
		virtual void PlaybackCompleted(void);
		virtual void AutoPauseReached(void);

		Error OpenTrack(WORD num);
		Error OpenSegment(WORD num);

		Error StopPlayItem(void);

		Error AdvancePlayItem(void);

		virtual Error PlayListCompleted(Error error = GNR_OK) {GNRAISE_OK;}
		virtual void PlayItemCompleted(void) {}

      virtual Error StartStillPhase(WORD delay); // calls StillPhaseCompleted() after delay secs
      virtual Error CancelStillPhase(void);

      Error StillPhaseCompleted(void);

		virtual void StartBreakpointStillCallback(void);

		WORD TranslatePartToEntry(WORD track, WORD part);
		WORD TranslateBlockToPart(WORD track, DWORD block);

	public:
		VCDPlayListSequencer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		~VCDPlayListSequencer(void);

		Error Initialize(VCDFileSystem * vcdfs, VCDInfo * info, VCDEntries * entries, VCDTracks * tracks, VCDSearch * search);

		Error StartPlayItem(WORD item, WORD wait, WORD repeat = 0, DWORD numSectors = 0, BOOL toPause = FALSE);
		Error StartTrackAt(WORD track, DWORD startSector, BOOL toPause = FALSE);
		Error RestartTrackAt(DWORD startSector, BOOL toPause = FALSE);
		Error RestartTrackAtItem(WORD item);
		virtual Error StartPlayList(VCDPlayList * list, DWORD flags = DDPSPF_NONE);
		Error AbortPlayList(void);
		Error CompletePlayItem(void);

		Error GoPrevItem(void);
		Error GoNextItem(void);

		Error StartForwardScan(WORD scanSpeed);
		Error StartBackwardScan(WORD scanSpeed);
		Error StopScan(BOOL toPause);
		Error TimeSearch(DVDTime time);
		virtual Error Pause(void);
		Error Resume(void);

		Error EnterStopState(void);

		Error GetAvailStreams(BYTE & audio, DWORD & subPicture);
		WORD	GetCurrentAudioStream(void);
		Error GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format);
		Error AudioStreamChange(WORD stream);

		WORD	GetScanSpeed(void) { if (scanSpeed >= 0) return (WORD)scanSpeed; else return (WORD)-scanSpeed; }

		WORD	CurrentPlayItem(void);

		Error StopStillPhase(void);

		BOOL IsStillPhase(void) {return state == pls_autopause || state == pls_still;}

		BOOL IsPlaying(void) {return state == pls_playing || state == pls_idle;}
		BOOL IsScanning(void) {return state == pls_scanning;}
		BOOL IsPaused(void) {return state == pls_paused;}
		BOOL IsIdle(void) {return state == pls_idle || state == pls_stopped;}
		BOOL IsStopped(void) {return state == pls_stopped;}
		virtual BOOL IsPlayingForward(void);

		virtual DVDPlayerMode InternalGetMode(void);
		virtual WORD NumberOfTitles(void);

		DWORD CurrentBlock(void);
		virtual Error CurrentDuration(DWORD & dur);
		virtual Error CurrentPlayingTime(DWORD & time);

		//
		//  Freezing Stuff
		//

		struct VCDPLSState
			{
			int		currentItem;
			WORD		currentTrack;
			WORD		repeatCount;
			WORD		waitTime;
			DWORD		numSectors;
			BOOL		playAllTracks;
			DWORD		transferCount;	// contains the number of bytes sent to the decoder while freeze command was executed
			};

		Error Freeze(VCDPLSState * buffer);
		Error Defrost(VCDPLSState * buffer, VCDPlayList * list, DWORD flags);

		virtual Error GetUOPs(DWORD & uops) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  Inline Functions
//
////////////////////////////////////////////////////////////////////

//
//  Set Playback Mode
//

void VCDPlayListSequencer::SetPlaybackMode(PlsState newState)
	{
	DVDPlayerMode oldMode, newMode;

	oldMode = InternalGetMode();
	state = newState;
	newMode = InternalGetMode();

	if (oldMode != newMode)
		SendEvent(DNE_PLAYBACK_MODE_CHANGE, newMode);
	}

//
//  Set Scan Speed
//
void VCDPlayListSequencer::SetScanSpeed(int newSpeed)
	{
	//
	//  Set new scan speed and send event if necessary
	//

	if (newSpeed != scanSpeed)
		{
		SendEvent(DNE_SCAN_SPEED_CHANGE, newSpeed);
		scanSpeed = newSpeed;
		}
	}

#endif
