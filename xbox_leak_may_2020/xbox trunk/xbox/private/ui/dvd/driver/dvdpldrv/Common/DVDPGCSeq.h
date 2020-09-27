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
//  Program Chain Server Module
//
//////////////////////////////////////////////////////////////////////

#ifndef DVDPGCSEQ_H
#define DVDPGCSEQ_H

#include "NavErrors.h"
#include "DVDAVStreamServer.h"
#include "DVDNavpu.h"
#include "DVDPlayer.h"
#include "DVDHliCtrl.h"

#pragma warning(disable : 4250)

class DVDNavigationTimer : protected WinPort
	{
	protected:
		BOOL							paused;
		BOOL							active, started;
		DVDNavigationControl	*	navControl;
		WORD							pgcn;
		DWORD							time;

		void Message (WPARAM wParam, LPARAM lParam);
	public:
		DVDNavigationTimer(WinPortServer * server, DVDNavigationControl * navControl);
		~DVDNavigationTimer(void);

		Error StartNavigationTimer(void);
		Error StopNavigationTimer(void);

		Error InitNavigationTimer(DWORD time, WORD pgcn);
		Error QueryNavigationTimer(DWORD & time, BOOL & active);
		Error ResetNavigationTimer(void);

		Error PauseNavigationTimer(void);
		Error ResumeNavigationTimer(void);
	};

class DVDPGCSequencer : public DVDSequencer, public DVDNavigationControl, public DVDHighlightControl, virtual protected EventSender
	{
	private:
		DWORD	currentBitrate;	// Please use the Set/Get methods to change this variable
		WORD	scanSpeed;			// Please use the Set/Get methods to change this variable

		Error SetScanSpeed(WORD speed);

	protected:
		DVDPlayer * player;
	   DVDNavigationProcessor * navpu;
		DVDVideoStreamServer * streamServer;

	   DVDOBS		*	vobs;
	   DVDGenericPGCI		*	pgci;

	   WORD 				currentPG, numPG;
	   WORD 				currentCell, finalCell, numCell;
	   WORD 				loopCnt, playingLoopCnt;
	   BYTE 				playbackMode;

		WORD				vobuCellCompletionDelay;

	   DVDTime			cellTime;
		DVDTime		*	cellTimes;

	   BOOL 				playbackDone;
	   BOOL			*	shuffleHistory;
		BOOL			*	playbackShuffleHistory;

	   DVDCell		*	stillCell;

	   WORD				playingCell;
	   WORD				playingPG;
	   DWORD				playingVOBU, nextVOBU;
	   DWORD				vobuUOPs, pgciUOPs;

		DVDNavigationTimer	navigationTimer;


	   WORD				angle;
		WORD				playingAngle;
		BOOL				isMultiAngleScene;
		BOOL				streaming;

   	WORD				resumeCell;
   	DWORD				resumeVOBU;
   	WORD				resumeLoopCnt;
		BOOL				resumeNavtimerActive;
		DWORD				resumeNavtimerTime;

		DisplayPresentationMode	currentPresentationMode;
		DisplayPresentationMode	requestedPresentationMode;
		DWORD				audioStreamSelectionPreferences;

	   enum State
	   	{
	   	pgcs_idle,						// 0
	   	pgcs_preCommand,				// 1
	   	pgcs_cellPlayback,			// 2
	   	pgcs_vobuStill,				// 3
	   	pgcs_cellStill,				// 4
	   	pgcs_cellCommand,				// 5
	   	pgcs_pgcStill,					// 6
	   	pgcs_postCommand,				// 7
	   	pgcs_scanForward,				// 8
	   	pgcs_scanBackward,			// 9
	   	pgcs_navigating,				// A
			pgcs_cellPlaybackReverse,	// B
			pgcs_cellTrickPlay,			// C
			pgcs_aborting,					// D
			pgcs_paused,					// E
			pgcs_awaitingPMLChange		// F
	   	} state, externalState;  // Please do not use "state" directly but use the Set/Get methods below

		// This enum describes the function to be called after a new PML has been set by the app.
		// pmlDeferLocation is only valid when in state pgcs_awaitingPMLChange
		enum PMLDeferLocation { PDL_NONE, PDL_STARTPGC, PDL_COMPLETEPGC, PDL_GOTOPPGC } pmlDeferLocation;
		int  awaitPMLChangePC;
		WORD requestedPML;

		DVDPGCSequencer::State GetState(void)					{return state;}
		DVDPGCSequencer::State GetExternalState(void)		{return externalState;}
		void	SetState(DVDPGCSequencer::State newState);
		BOOL IsInternalState(DVDPGCSequencer::State state);

		BOOL IsStillCell(void);
		Error IsFinalCell(WORD program, WORD cell, BOOL & isFinalCell);

		Error StartProgram(void);
		Error AdvanceCell(void);
		Error SuccCell(DVDGenericPGCI * pgci, WORD angle, WORD & cell, DVDCPBI & cpbi);
		Error PredCell(DVDGenericPGCI * pgci, WORD angle, WORD & cell, DVDCPBI & cpbi);
		Error StartCell(WORD cell, DWORD vobu = 0);
		Error StartCellAt(WORD cell, DVDTime time);
		Error SendNextCell(void);
		Error SendNextProgram(void);
		Error HasNextProgram(BOOL & hasNextProg);
		Error CompletePGCPlayback(void);
		Error PrepareNavigation(void);
		Error PrepareNavigationAccurate(void);

		Error VOBUStarted(DVDVOBU * vobu);
		Error VOBUCompleted(DVDVOBU * vobu);

      Error CellStarted(DVDCell * cell);
      Error CellCompleted(DVDCell * cell);
		Error CellCorrupt(DVDCell * cell);

		Error PlaybackTerminated(Error err);

      virtual Error PGCCompleted(DVDGenericPGCI * pgci) {GNRAISE_OK;}

      virtual Error StartStillPhase(WORD delay); // calls StillPhaseCompleted() after delay secs
      virtual Error CancelStillPhase(void) = 0;

      Error StillPhaseCompleted(void);

		Error GetCurrentPlayingProgram(WORD & pg);

		WORD CurrentAngle(void) { return angle; }
		WORD GetPlayingAngle(BOOL isMultiangle) { return (isMultiangle ? playingAngle:1); }
		virtual void AngleChanged(void);

		Error BuildCellTimes(DVDGenericPGCI * pgci, DVDTime * & times, WORD cellNum);

		Error DoActivateButton(void);

		virtual Error SelectAudioStream(WORD & strm);
		virtual Error SelectSPUStream(WORD & strm, BOOL & enable);
		virtual Error SelectDisplayPresentationMode(DisplayPresentationMode & mode);

		virtual Error PerformStreamChange(void);
		virtual Error PerformAudioStreamChange(void);
		virtual Error PerformSPUStreamChange(void);

		virtual Error SetStreamParameters(void);
		virtual Error SetAudioStreamParameters(void);
		virtual Error SetSPUStreamParameters(void);

		virtual Error AngleChange(void);

		virtual Error CheckTitleChange(void) = 0;
		virtual void  StateChanged(void)= 0;

		//
		//  Breakpoint handling
		//

		virtual BOOL EndOfPGCBreakpointReached(ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual BOOL EndOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp);
		virtual BOOL EndOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual BOOL BeginOfCellBreakpointReached(DVDCell * cell, ERSPlaybackDir dir, ERSBreakpoint & bp);
		virtual BOOL BeginOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp) = 0;
		virtual void StartBreakpointStillCallback(void);

		State breakpointStillState;		// The still state in which the breakpoint leads
		ERSBreakpoint breakpoint;			// The breakpoint we reached
		DWORD restartVOBU;					// This VOBU must be restarted after stilling at a breakpoint
		WORD restartCell;						// This cell must be restarted after stilling at a breakpoint
		BOOL programStarted;					// This is set by StartProgram to prevent BeginOfPTT breakpoints from
													// triggering in the middle of a cell

		//
		//  Parental Management
		//

		void	SetAwaitPMLChangeState(PMLDeferLocation deferLoc, int pc);

		//
		//  Freezing stuff
		//

		class DVDPGCSFreezeState
			{
			public:
				WORD 		loopCnt;

				WORD		vobuCellCompletionDelay;

				DVDTime	cellTime;

				BOOL 		playbackDone;

				BOOL		shuffleHistoryExists;
				DWORD		shuffleHistory[4];

				WORD		playingCell;

				WORD		angle;

   			WORD		resumeCell;
   			DWORD		resumeVOBU;
   			WORD		resumeLoopCnt;
				BOOL		resumeNavtimerActive;
				DWORD		resumeNavtimerTime;

				BOOL		navtimerActive;
				DWORD		navtimerTime;

				DVDHCFreezeState hcFreezeState;
				DVDNavigationProcessor::DVDNPFreezeState npFreezeState;
			};

		Error Freeze (DVDPGCSFreezeState & state);
		Error Defrost(DVDPGCSFreezeState & state, DVDOBS * vobs, DVDGenericPGCI * pgci, DVDDomain domain, DWORD flags);

	public:
	   DVDPGCSequencer(WinPortServer * server, UnitSet units, EventDispatcher * eventDispatcher);
		virtual ~DVDPGCSequencer(void);

		Error Init(DVDFileSystem * dvdfs);

	   virtual Error GetCurrentLocation(DVDLocation & loc);
	   virtual Error GetCurrentDuration(DVDLocation & loc);

	   virtual Error GetUOPs(DWORD & uops);
	   virtual Error UOPPermitted(int uop, BOOL & permitted);
	   BOOL IsScanning(void) {return GetExternalState() == pgcs_scanForward || GetExternalState() == pgcs_scanBackward;}
		BOOL IsReversePlayback(void) {return GetExternalState() == pgcs_cellPlaybackReverse;}
		BOOL IsTrickPlay(void) {return GetExternalState() == pgcs_cellTrickPlay; }
		BOOL IsStill(void) {return GetExternalState() == pgcs_cellStill || GetExternalState() == pgcs_vobuStill ||
			                        GetExternalState() == pgcs_pgcStill;}
		BOOL IsPaused(void) {return GetExternalState() == pgcs_paused;}
		virtual BOOL IsPlayingForward(void);
		BOOL IsMultiAngleScene(void) {return isMultiAngleScene;}

		Error StartPGCPlayback(DVDOBS * vobs, DVDGenericPGCI * pgci, WORD program, BOOL skipPreCommands = FALSE, BOOL toPause = FALSE);
		Error StartPGCPlaybackWithTime(DVDOBS * vobs, DVDGenericPGCI * pgci, DVDTime timeOffset, BOOL skipPreCommands = FALSE, BOOL toPause = FALSE);
		Error StartPGCPlaybackWithParentalLevel(DVDGenericPGCI * pgci);
		Error StopPGCPlayback(void);
		Error	AccessRestricted(DVDGenericPGCI * pgci, DVDTime time, BOOL & restricted);

		virtual Error Exit(Error err = GNR_OK) {GNRAISE_OK;}

		Error StartForwardScan(WORD speed);
		Error StartBackwardScan(WORD speed);
		Error StartReversePlayback(void);
		Error StartTrickPlayback(void);
		Error StopScan(BOOL toPause);
		WORD	GetScanSpeed(void) { return scanSpeed; }
		Error Pause(void);
		Error Resume(void);

		Error	GetSPRM(int index, WORD & val);
		Error SetSPRM(int index, WORD val);

		Error GoProgram(WORD pgn);
		Error GoCell(WORD cell);
		Error GoPrevProgram(void);
		Error GoNextProgram(void);
		Error GoTopProgram(void);
		Error GoPrevCell(void);
		Error GoNextCell(void);
		Error GoTopCell(void);
		Error GoTopProgramChain(void);
		Error GoTailProgramChain(void);
		virtual Error GoProgramChain(WORD pgcn) = 0;

		Error ResumeFromSystemSpace(DVDOBS * vobs, DVDGenericPGCI * pgci);
		virtual Error SaveResumeInformation(void);

		DisplayPresentationMode GetCurrentDisplayMode(void) {return currentPresentationMode;}
		Error SetDisplayMode(DisplayPresentationMode mode);
		Error SetAudioStreamSelectionPreferences(DWORD flags);

		void	CurrentBitrateSet(DWORD currentBitrate)
			{
			if (currentBitrate != this->currentBitrate)
				SendEvent(DNE_BITRATE_CHANGE, currentBitrate);
			this->currentBitrate = currentBitrate;
			}
		DWORD	CurrentBitrate() { return currentBitrate; }

		virtual PMLHandling PMLChangeAllowed(void);
		Error ParentalLevelSelect(WORD level);
		Error SetEventHandler(DWORD event, DNEEventHandler handler, void * userData);
	};

#pragma warning(default : 4250)

#endif
