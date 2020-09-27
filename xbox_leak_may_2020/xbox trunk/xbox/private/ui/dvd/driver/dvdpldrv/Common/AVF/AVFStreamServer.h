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

// FILE:       Driver/Dvdpldrv/Common/AVF/AVFStreamServer.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    3-SEP-2000
//
// PURPOSE:    Generic stream server for audio and video files.
//
// HISTORY:

#ifndef AVFSTREAMSERVER_H
#define AVFSTREAMSERVER_H

class AVFStreamServer;

#include "Library/Common/Prelude.h"
#include "Library/Common/WinPorts.h"
#include "Library/Common/KrnlSync.h"
#include "Driver/Dvdpldrv/Common/Breakpoint.h"
#include "Driver/Dvdpldrv/Common/DVDTime.h"
#include "Library/Files/GenericFile.h"
#include "Driver/Dvdpldrv/Common/AVF/AVFile.h"
#include "Library/Lowlevel/Timer.h"

#ifndef _DEBUG_SHELL
#include "Driver/Dvdpldrv/Common/AVF/AVFPlayer.h"
#endif
#ifdef _DEBUG_SHELL
#define _DEBUG_LIST
#include "Library/Support/Shell/ShellToolKit.h"
#else
#include <stdio.h>
#endif // _DEBUG_SHELL

// #define _DEBUG_BUFFER_PERFORMANCE_TEST
#ifdef _DEBUG_BUFFER_PERFORMANCE_TEST
typedef struct
	{
	int	framesRead;
	int	framesQueued;
	int	bytesSent;
	DWORD	framesSent;
	DWORD	startMilliTicks;
	DWORD	endMilliTicks;
	} DebugRefillStats;
#define DEBUG_MAX_REFILLS_PER_TRANSFER	20 // for sizing only
#define DEBUG_MAX_LOOPS_PER_PRINT		50 // see AVFileManager::GetNumberOfRefillsPerTransfer()
#endif

//
//  Defines
//

#define AVF_MAX_TRANSFER_NUM	5
#define CDDA_SIGNAL_TIME_NUM	2		// Number of supported signal times

#define AVF_CALLBACK_DISTANCE	24000		// 1 sec at 192kBpS
#define AVF_CALLBACK_POSITION_UNDEFINED	0xffffffff
#define AVF_MAX_CONSECUTIVE_IDENTICAL_ERRORS		2 // nudge after this many errors on same frame

#define DEFAULT_MIN_AUDIO_SCAN_BITRATE	192
	// when bitReservoirInUse == FALSE and scanIsAudible the above comes into play

////////////////////////////////////////////////////////////////////
//
//  AVFBreakLocation class
//
////////////////////////////////////////////////////////////////////

class AVFStillLocation
	{
	public:
		DWORD		milliSec;		// time of the still loc
		BOOL		valid;			// Position is valid
		BOOL		upcoming;		// We are reaching still loc
		DWORD		callbackPos;	// Callback at that position
		DWORD		deferMilliSec;		// Defer location for cancelled breakpoint
		BOOL		buffered;		// required data has been sent to the player

		AVFStillLocation(void) { valid = FALSE; upcoming = FALSE; buffered = FALSE; }
		Error SetStillLocation(DWORD ms);
		void SetStillLocationUpcoming(void);
		void SetStillLocationBuffered(void) { buffered = TRUE; }
		// void ResetAtStillLocation(void) { buffered = FALSE; upcoming = FALSE; }
		void SetStillPosition(DWORD pos) { callbackPos = pos; }
		DWORD GetStillPosition(void) { return callbackPos; }
		BOOL IsStillLocationValid(void) { return valid; }
		BOOL IsStillLocationBuffered(void) { return buffered; }
		BOOL IsStillLocationUpcoming(void);
		Error ClearStillLocation(void);
		void Invalidate(void) { valid = FALSE; upcoming = FALSE; buffered = FALSE; }
	};

////////////////////////////////////////////////////////////////////
//
//  AVFStreamServer Class
//
////////////////////////////////////////////////////////////////////

class AVFStreamServer : protected WinPort, virtual protected ERSBreakpointControl
	{
	protected:
		AVFileManager			avFile;		// the currently-playing file handler
		AVFPlayer		* player;
		GenericProfile * profile;

		DVDPlayerMode	mode;							// The mode the player is currently in (Please use Set Method to change)
		VDLocalMutex	serverLock;					// Save the stream server

		//
		//  Playback related variables
		//

		AVFFrame							*pCurrentFrame;
		AVFFrameLocator				*pCurrentFrameLocator;
		AVFFrameSegmentNode			*pCurrentFrameSegment;

		DWORD			currentFrameNum;				// the frame currently being sent
		DWORD			lastScanFrameBuffered;		// the lowest frame number in frame buffer
		DWORD			currentSegmentNum;			// each frame may consist of multiple segments
		DWORD			currentSegmentSendOffset;	// offset within the frame segment
		BOOL			needToDequeueNextFrame;		// need to dequeue next frame in TransferBuffer()
		BOOL			cancelled;						// Set to TRUE by CancelData, prevents callback from using old data


		BOOL	isFinalBuffer;					// Set if buffer is the last to play
		BOOL	completeDataCalled;			// Ensures there are no end-of-file hangs
		BOOL	isMovingBackward;				// backward play or backward scan modes only
		BOOL	isBeginningOfScan;			// comes into play when isFinalBuffer is TRUE, at begin of scan
		BOOL	isBeginningOfResume;			// comes into play when isFinalBuffer is TRUE, at begin of resume
		BOOL	totalBufferedBytes;			// used during scan modes
		BOOL	abortScan;						// for handling odd error conditions during scan
		BOOL	scanIsAudible;					// from profile settings
		WORD	scanSpeed;						// scanSpeed = 1 => double speed playback
		DWORD	elapsedSeconds;				// needed for GetCurrentLocation()
		DWORD	elapsedMillisec;				// needed for GetCurrentLocation()
		DWORD minFreeReverseScanBuffer;		// no disk reads will occur unless room for this many frames
		DWORD reverseScanFrameSequenceLen;	// this many frames will be played sequentially
		DWORD forwardScanFrameSequenceLen;	// this many frames will be played sequentially
		DWORD maxRefillsPerTransfer;			// set by filter, see GetNumberOfRefillsPerTransfer()
		DWORD maxFramesLockedPerRefill;		// set by filter, see GetMaxFramesToLockEachRefill()
		DWORD minFramesLockedPerReverseRefill; // one half of maxFramesLockedPerRefill
		DWORD	minAudioScanBitrate;			// when bitReservoirInUse == FALSE and scanIsAudible
		DWORD	frameMilliSec;

		DWORD			errorFrameNum;
		int			errorFrameNumCount;
		void		ResetNeedToNudgeNudgeTargetFrameNum(void);	// helps get past bad disk sectors
		BOOL		NeedToNudgeTargetFrameNumOnError(DWORD num); // helps get past bad disk sectors
		DWORD	totalSentBytes; // how many bytes were sent to the decoder

#ifdef _DEBUG_BUFFER_PERFORMANCE_TEST
		DWORD	totalSentBytes; // how many bytes were sent to the decoder
		DebugRefillStats refillStats[DEBUG_MAX_LOOPS_PER_PRINT][DEBUG_MAX_REFILLS_PER_TRANSFER];
		int	debugRefillIndex, debugLoopNum;
#endif
		//
		//  Break Location Handling
		//

		GenericFileSystemIterator	*gfsiStill;	// need to save this for the StopStillPhase()
		AVFileManager	*pInfoFile;					// passed down from AVFDiskPlayer
		DWORD	timeBaseMillisec;				// Base for current playback time, for StopStillPhase()
		AVFStillLocation	stillLocation;
		AVFFilterType		fileType;

		//
		//  Breakpoint handling
		//

		ERSBreakpoint	currentBreakpoint;
		BOOL				currentBreakpointValid;

		ERSBreakpoint	lastBreakpoint;		// The last breakpoint reached
		ERSPlaybackDir	lastBreakpointDir;	// The direction in which we were playing then

		void	SetCurrentBreakpoint(DWORD time, BOOL playForward);
		virtual	void	SetBreakpointActiveCallback(ERSBreakpoint bp, BOOL excl);
		virtual	void	StartBreakpointStillCallback(void);
		virtual Error SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);
		virtual Error ClearBreakpoint(DWORD id);

		Error	SetStillLocation(DWORD time);
		Error	ClearStillLocation(void);
		Error	StartStillPhase(void);

		//
		//  Signal Time Handling
		//

		struct tagSignalTime
			{
			DWORD	milliSec;
			BOOL	valid;
			} signalTime;

		Error		SetSignalTime(DWORD milliSec);
		DWORD		GetSignalTime(void);
		Error		ClearSignalTime(void);
		BOOL		SignalTimeReached(DWORD playbackMillisec, BOOL playForward);
		void		SignalTimeCallback(void);
		BOOL		StillLocationReached(DWORD playbackMilliSec, BOOL playForward);

		//
		//  Playback functions
		//

		Error TransferBuffer(void);
		Error	RefillBuffer(void);
		void	InvalidateAllBuffers(void);

		Error	FlushBuffer(void);
		Error	CancelData(void);
		BOOL	BufferEmpty(void);
		void	SetIsMovingBackward(BOOL backwd) { isMovingBackward = backwd; }
		BOOL	IsMovingBackward(void) { return isMovingBackward; }
		BOOL	IsMovingForward(void) { return !isMovingBackward; }
		// Error GetPlaybackTime(DVDTime & time) {GNREASSERT(player->GetPlaybackTime(time)); GNRAISE_OK;}

		void SetPlaybackMode(DVDPlayerMode newMode)	// Use this to change playback mode with event sending
			{
			if (mode != newMode)
				{
				SendEvent(DNE_PLAYBACK_MODE_CHANGE, newMode);
				mode = newMode;
				}
			}

		Error SetCallbackPosition(DWORD position);
		virtual void PlaybackCompleted(void) = 0;
		DWORD	GetElapsedSeconds(void) { return elapsedSeconds; }
		DWORD	GetElapsedMillisec(void) { return elapsedMillisec; }
		void	SetElapsedSeconds(DWORD sec);
		void	SetElapsedMillisec(DWORD ms);
		DWORD GetNumberOfRefillsPerTransfer(void) { return maxRefillsPerTransfer; }
		void SetNumberOfRefillsPerTransfer(int num) { maxRefillsPerTransfer = num; }
		DWORD GetMaxFramesToLockEachRefill(void) { return maxFramesLockedPerRefill; }
		void SetMaxFramesToLockEachRefill(int num) { maxFramesLockedPerRefill = num; }
		DWORD	GetTotalBufferedBytes(void) { return totalSentBytes; } // how many bytes were sent to the decoder
		void AddToTotalBufferedBytes(DWORD num) { totalSentBytes += num; }
		void ClearTotalBufferedBytes(void) { totalSentBytes = 0; }
		// DWORD GetMinFramesToLockEachReverseRefill(void) { return minFramesLockedPerReverseRefill; }
		// void SetMinFramesToLockEachReverseRefill(int num) { minFramesLockedPerReverseRefill = num; }

	public:

		AVFStreamServer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		~AVFStreamServer(void);

		// must be called before InitializeDataStructures()
		virtual Error Init(WinPortServer * server, GenericFileSystem * avffs, GenericProfile * profile);

		//
		//  Data source handling
		//

		Error OpenFile(GenericFileSystemIterator * gfsi);
		Error CloseFile(void);

		Error Cleanup(void);

		//
		//  WinPort methods
		//

		void Message(WPARAM wParam, LPARAM dParam);

		//
		//  Playback methods
		//

		void SetInfoFile(AVFileManager * pFile);
		Error StartPlayback(GenericFileSystemIterator * gfsi,
								  BOOL pauseAtStart = FALSE, DVDTime *pTime = (DVDTime *) NULL);
		Error StopPlayback(BOOL aborted = FALSE);
		Error StartForwardScan(WORD speed);
		Error StartBackwardScan(WORD speed);
		Error StopScan(void);
		Error PausePlayback(void);
		Error ResumePlayback(void);
		virtual Error StopStillPhase(void);

		//
		//  Inquiry (inquisition) methods
		//

		BOOL IsPlaying(void);											// Paused counts as playing
		WORD GetScanSpeed(void) { return scanSpeed; }
		Error GetCurrentProgramTime(DVDTime &pgcTime);
		AVFFilterType GetFileType(void) { return fileType; }
		virtual Error GetCurrentBitrate(DWORD & bitrate);

		Error GetCodingMode(DVDAudioCodingMode &codingMode);
		Error GetSampleRate(DWORD &samplesPerSecond);
		Error GetNumberOfAudioChannels(WORD &channels);
		DWORD GetNumberOfUnplayedBytesInDecoderBuffer(void);
		DWORD GetBufferedFrameCount(void);
		DWORD GetFrameMillisec(void);
		BOOL IsFrameBufferHalfFull(void);
		BOOL IsFrameBufferQuarterFull(void);
		BOOL IsFrameBufferThreeQuartersFull(void);

		virtual DVDPlayerMode InternalGetMode(void);

		//
		// Transfer methods (called by AVFPlayer)
		//

		Error PerformTransfer(int maxTransferNum = 0); // was AVF_MAX_TRANSFER_NUM;
      void	PlayerCallback(DWORD signalPosition, BOOL calculateOnly=FALSE);
#ifdef _DEBUG
		void DebugDumpPlayerMode(void);
#endif
	};


#endif

