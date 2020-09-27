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
//  VCD Play List Sequencer
//
////////////////////////////////////////////////////////////////////

#include "VCDPlaySeq.h"
#include "Library/Common/vddebug.h"


////////////////////////////////////////////////////////////////////
//
//  Macro Definitions
//
////////////////////////////////////////////////////////////////////

#define SAVE_UOPS		DWORD oldUOPs; DWORD newUOPs; GNREASSERT(GetUOPs(oldUOPs));
#define CHECK_UOPS	{ GNREASSERT(GetUOPs(newUOPs)); if (oldUOPs != newUOPs) SendEvent(DNE_VALID_UOP_CHANGE, newUOPs); }

////////////////////////////////////////////////////////////////////
//
//  Function Definitions
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDPlayListSequencer::VCDPlayListSequencer(WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: VCDStreamSequencer(server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	, ERSBreakpointControl(pEventDispatcher)
	{
	list = NULL;
	file = NULL;
	state = pls_stopped;
	scanSpeed = 0;

	currentTrack = 0;
	currentSegment = 0;
	numberOfTitles = 0;
	currentDurationInMilliSecs = 0;

	playingAudioTrack = FALSE;
	}

//
//  Destructor
//

VCDPlayListSequencer::~VCDPlayListSequencer(void)
	{
	delete file;
	delete list;
	}

//
//  Initialize
//

Error VCDPlayListSequencer::Initialize(VCDFileSystem * vcdfs, VCDInfo * info,	VCDEntries * entries,
													VCDTracks * tracks, VCDSearch * search)
	{
	GNREASSERT(VCDStreamSequencer::Init());

	this->info = info;
	this->entries = entries;
	this->tracks = tracks;
	this->search = search;

	GNREASSERT(SetAttributes(entries->IsSVCDStandard()));

	GNRAISE_OK;
	}

//
//  Message Function
//

void VCDPlayListSequencer::Message(WPARAM wParam, LPARAM lParam)
	{
	switch (wParam)
		{
		case STILL_TIME_MSG:
			StillPhaseCompleted();
			break;
		case PLAYBACK_TIME_MSG:
			// nothing to do
			break;
		case BREAKPOINT_MSG:
			{
			ERSBreakpoint	bp;
			DWORD				loc;
			int				frameRate = player->GetCurrentFrameRate();

			loc = ((CurrentLocation() / CD_FRAME_SIZE) * frameRate) / CD_FRAME_RATE;
			if (BreakpointReached(CurrentPlayItem() - 1, DVDTime(0, 0, 0, loc, frameRate),
				DVDTime(0, 0, 0, loc + 15, frameRate), IsPlayingForward() ? EPD_FORWARD : EPD_BACKWARD, bp))
				{
				bpReached = BPS_NORMAL;
				ExecuteBreakpoint(bp.id);
				}
			break;
			}
		default:
			// Call base class messaging.
			VCDStreamSequencer::Message (wParam, lParam);
			break;
		}
	}

//
//  Start A Still Phase
//

Error VCDPlayListSequencer::StartStillPhase(WORD delay)
	{
	if (delay == 0)
		GNREASSERT(StillPhaseCompleted());
	else
		{
		if (delay < 255)
			ScheduleTimer(STILL_TIME_MSG, (DWORD)delay * 1000);

		SendEvent(DNE_STILL_ON, delay);
		}

	GNRAISE_OK;
	}

//
//  Cancel Still Phase
//

Error VCDPlayListSequencer::CancelStillPhase(void)
	{
	CancelTimer();
	if (state == pls_still || state == pls_autopause)
		SendEvent(DNE_STILL_OFF, 0);

	GNRAISE_OK;
	}

//
//  Stop Still Phase
//

Error VCDPlayListSequencer::StopStillPhase(void)
	{
	Error err = GNR_OK;

	switch (bpReached)
		{
		case BPS_NONE:
			CancelTimer();

			if (state == pls_still || state == pls_autopause)
				GNREASSERT(StillPhaseCompleted());
			break;
		case BPS_NORMAL:
			player->Resume();
			bpReached = BPS_NONE;
			SetPlaybackMode(pls_playing);
			break;
		case BPS_EOT:
			bpReached = BPS_NONE;
			SetPlaybackMode(pls_playing);
			if (IS_ERROR(err = AdvancePlayItem()))
				SetPlaybackMode(pls_stopped);
			break;
		}

	if (!IS_ERROR(err))
		SendEvent(DNE_STILL_OFF, 0);
	GNRAISE(err);
	}

//
//  Transfer Completed
//

void VCDPlayListSequencer::TransferCompleted(void)
	{
	if (scanSpeed > 0)
		SetScanSpeed(0);

	if (repeatCount && !scanSpeed && file)
		{
		//
		// the value 0xFFFF for the variable 'repeatCount' means an infinity loop.
		// so do not decrement repeat count in this case!
		//

		if (repeatCount != 0xFFFF)
			repeatCount--;
		TransferStream(file, 0, 0, !repeatCount);
		}
	}

//
//  Playback Completed
//

void VCDPlayListSequencer::PlaybackCompleted(void)
	{
	ERSBreakpoint bp;

	if (scanSpeed < 0)
		{
		//
		//  We reached beginning of item in backward scan -> start forward scan
		//

		StopScan(FALSE);
		}
	else
		{
		//
		//  Test if we reached End-Of-Title breakpoint
		//

		if (EndOfTitleBreakpointReached(CurrentPlayItem() - 1, IsPlayingForward() ? EPD_FORWARD : EPD_BACKWARD, bp))
			bpReached = BPS_EOT;

		if (bpReached == BPS_EOT)
			ExecuteBreakpoint(bp.id);
		else
			bp.flags = 0;

		//
		//  Start still phase if we didn't reach pause breakpoint
		//

		if (!BP_TOSTILL(bp.flags))
			{
			if (waitTime)
				{
				SetPlaybackMode(pls_still);

				StartStillPhase(waitTime);
				}
			else
				{
				AdvancePlayItem();
				}
			}
		}
	}

//
//  Auto Pause Reached
//

void VCDPlayListSequencer::AutoPauseReached(void)
	{
	if (list)
		{
		SetPlaybackMode(pls_autopause);
		StartStillPhase(list->AutoPauseWaitTime());
		}
	else
		ContinueAutoPause();
	}

//
//  Stop Play Item
//

Error VCDPlayListSequencer::StopPlayItem(void)
	{
	GNREASSERT(AbortTransfer());
	player->Resume();

	GNRAISE_OK;
	}

//
//  Advance To Next Play Item
//

Error VCDPlayListSequencer::AdvancePlayItem(void)
	{
	Error err = GoNextItem();

	if (err == GNR_OBJECT_NOT_FOUND)
		{
		err = PlayListCompleted();
		if (IS_ERROR(err))
			SetPlaybackMode(pls_stopped);
		}
	else if (err == GNR_ITEM_NOT_FOUND)
		{
		err = PlayListCompleted(err);
		SetPlaybackMode(pls_stopped);
		}

	GNRAISE(err);
	}

//
//	Function:
//		FindLastPresentationTime
//	Parameter:
//		file: pointer to a VCDDataFile
//	Return Value:
//		last presentation time stamp in the given track (equals track playing time)
// Description:
//		Search for the last sector of the given track and extracts the PTS that is equal
//		to the track duration.
//		Usually there should be a file TRACKS.SVD on the disc that contains the duration
//		for each track. Unfortunately some illegal discs don't conform to the spec and
//		therefore don't provide the TRACKS.SVD file. For these discs use this function to
//		get the duration.
//
static DWORD FindLastPresentationTime(VCDDataFile * file)
	{
	DriveBlock db;
	RequestHandle rh;
	DWORD sector;
	BOOL found;
	BOOL readError;
	WORD numSearch;
	BYTE pts_dts_flags;
	DWORD pts, scr;
	Error err;

	if (file)
		{
		found = readError = FALSE;
		numSearch = 0;

		//
		// determine the number of sectors this track contains
		//

		if (IS_ERROR(file->GetNumberOfBlocks(sector)))
			sector = 0;

		while (!found && !readError)
			{
			//
			// read data
			//

			if (!IS_ERROR(err = file->LockBlocks(sector, 1, &db, DAF_NONE, &rh)))
				{
				//
				// look for a valid PTS
				//

				pts_dts_flags = db.data[45] & 0xc0;
				if (pts_dts_flags == 0xc0 || pts_dts_flags == 0x80)
					{
					//
					// extract the PTS
					//

					pts = (((DWORD)(db.data[47] & 0x0e) >> 1) << 29) |
							(((DWORD)(db.data[48]       )     ) << 21) |
							(((DWORD)(db.data[49] & 0xfe) >> 1) << 14) |
							(((DWORD)(db.data[50]       )     ) <<  6) |
							(((DWORD)(db.data[51] & 0xfc) >> 2));

					//
					// convert into milliseconds and round up
					//
					pts = pts/45 + 1000;

					found = TRUE;
					}
				else if (db.data[24] == 0x00 && db.data[25] == 0x00 && db.data[26] == 0x01 && db.data[27] == 0xba)
					{
					//
					// if no valid PTS can be found extract the SCR
					//

					scr = ((((DWORD)(db.data[28] & 0x38)	>> 1)	| ((DWORD)(db.data[28] & 0x03))) << 27) |
							(((DWORD) (db.data[29]		 )		 )													<< 19) |
							((((DWORD)(db.data[30] & 0xf8)	>> 1) | ((DWORD)(db.data[30] & 0x03))) << 12) |
							(((DWORD) (db.data[31]		 )		 )													<<  4) |
							(((DWORD) (db.data[32] & 0xf8)	>> 4));

					scr = scr/45 + 1000;
					pts = scr;

					found = TRUE;
					}
				}

			file->UnlockBlocks(sector, 1, &db, DAF_NONE, &rh);

			if (!found)
				{
				sector--;
				numSearch++;
				}

			//
			// 140 sectors means approximately 1 seconds (highest bitrate 2.6 Mbps)
			//
			if (numSearch >= 140)
				{
				DP("*** READ ERROR ***");
				pts = 0;
				readError = TRUE;
				}
			}

		return pts;
		}
	else
		return 0;
	}

//
//  Find Previous Sequence Header
//

static DWORD FindPrevSequenceHeader(VCDDataFile * file, DWORD blk)
	{
	RequestHandle rh;
	DriveBlock blocks[16];
	int		num, lockedBlocks;
	int		i;
	BYTE		*p;
	BOOL		found;
	DWORD		numBlocks;

	//
	// Do backward search for seq header, beginning with successor block
	//

	found = FALSE;
	blk++;

	file->GetNumberOfBlocks(numBlocks);
	if (blk > numBlocks) blk = numBlocks;

	num = 0;
	while (!found && blk > 0)
		{
		//
		//  Search 16 blocks wise
		//

		if (blk > 16)
			num = 16;
		else
			num = blk;

		lockedBlocks = num;
		blk-=num;
		if (!IS_ERROR(file->LockBlocks(blk, num, blocks, DAT_LOCK_AND_READ, &rh)))
			{
			//
			//  Now find pic header in blocks
			//

			while (!found && num)
				{
				num--;
				p = blocks[num].data;
				if (p[18] == 0x62)
					{
					i = 0;
					while (i < 2044 && (p[i] != 0x00 || p[i+1] != 0x00 || p[i+2] != 0x01 || (p[i+3] != 0xb3 && p[i+3] != 0xb8)))
						i++;
					found = i < 2044;
					}
				}
			}
		file->UnlockBlocks(blk, lockedBlocks, blocks, DAT_UNLOCK_CLEAN, &rh);
		}
	blk += num;

	return blk;
	}

//
//  Open A Segment
//

Error VCDPlayListSequencer::OpenSegment(WORD num)
	{
	DVDDiskType diskType;
	DiskItemName name;
	GenericDiskItem * gdi;
	BOOL isPAL;

	GNREASSERT(GetDiskType(diskType));

	SegPlayItemContent spic(diskType == DDT_VIDEO_CD);;

	if (diskType == DDT_VIDEO_CD)
		name = "segment\\item" + KernelString(num, 4) + ".dat";
	else if (diskType == DDT_SUPER_VIDEO_CD)
		name = "segment\\item" + KernelString(num, 4) + ".mpg";
	else
		GNRAISE(GNR_OBJECT_INVALID);

	GNREASSERT(vcdfs->OpenItem(name, FAT_READ, gdi));
	file = (VCDDataFile*)gdi;

	spic = info->GetSegPlayItemContent(num);

	GNREASSERT(file->IsPALSequence(isPAL));
	GNREASSERT(player->SetStreamAttributes(isPAL || spic.IsPALVideo(), diskType == DDT_VIDEO_CD));

	currentSegment = num;

	GNRAISE_OK;
	}

//
//  Open A Track
//

Error VCDPlayListSequencer::OpenTrack(WORD num)
	{
	DVDDiskType diskType;
	Error err;
	DiskItemName name;
	GenericDiskItem * gdi;
	BOOL isPAL;

	file = NULL;

	//
	//  Test on AV track
	//

	GNREASSERT(GetDiskType(diskType));
	if (diskType == DDT_VIDEO_CD)
		{
		name = "mpegav\\avseq" + KernelString((WORD)(num-1), 2) + ".dat";
		if (IS_ERROR(err = vcdfs->OpenItem(name, FAT_READ, gdi)))
			{
			//
			//  Test on AV track with alternative name
			//

			name = "mpegav\\music" + KernelString((WORD)(num-1), 2) + ".dat";
			err = vcdfs->OpenItem(name, FAT_READ, gdi);
			}
		}
	else if (diskType == DDT_SUPER_VIDEO_CD)
		{
		//
		// the MPEG-2 audio/video tracks can be found under the directory \\mpeg2\
		//	(like described in the SVCD system specification Version 1.0, May, 1999).
		//
		// some s-vcds have a directory \\mepgav\ instead of \\mpeg2\
		// so we have to support this too

		name = "mpeg2\\avseq" + KernelString((WORD)(num-1), 2) + ".mpg";
		if (IS_ERROR(err = vcdfs->OpenItem(name, FAT_READ, gdi)))
			{
			name = "mpegav\\avseq" + KernelString((WORD)(num-1), 2) + ".mpg";
			err = vcdfs->OpenItem(name, FAT_READ, gdi);
			}
		}
	else
		GNRAISE(GNR_OBJECT_INVALID);

	if (!IS_ERROR(err))
		{
		//
		//  We found an AV track
		//

		file = (VCDDataFile*)gdi;
		GNREASSERT(file->IsPALSequence(isPAL));
		GNREASSERT(player->SetStreamAttributes(isPAL || info->IsPALTrack(num), diskType == DDT_VIDEO_CD));
		currentTrack = num;
		if (diskType == DDT_SUPER_VIDEO_CD)
			{
			if (!tracks)
				currentDurationInMilliSecs = FindLastPresentationTime(file);
			else
				currentDurationInMilliSecs = 0;
			}
		}
	else
		list = NULL;

	GNRAISE(err);

	}

//
//  Restart Track At A Sector
//

Error VCDPlayListSequencer::RestartTrackAt(DWORD startSector, BOOL toPause)
	{
	if ((state == pls_playing || state == pls_scanning || state == pls_still) && file && currentTrack)
		{
		StopPlayItem();
		SetPlaybackMode(pls_playing);
		startSector = FindPrevSequenceHeader(file, startSector);
		GNREASSERT(TransferStream(file, startSector, 0, !repeatCount, 0, FALSE, toPause));
		}

	GNRAISE_OK;
	}

//
//  Restart Track At A Play Item
//

Error VCDPlayListSequencer::RestartTrackAtItem(WORD item)
	{
	if (state == pls_playing && file && item >= 100 && currentTrack && entries->GetEntryTrack(item - 99) == currentTrack)
		{
		StopPlayItem();

		GNREASSERT(TransferStream(file, file->SectorToBlock(entries->GetEntrySector(item - 99)), 0, !repeatCount));
		}

	GNRAISE_OK;
	}

//
//  Start Forward Scan
//

Error VCDPlayListSequencer::StartForwardScan(WORD scanSpeed)
	{
	DWORD startSector;
	SAVE_UOPS;

	SetScanSpeed(scanSpeed);
	startSector = CurrentLocation() / CD_FRAME_SIZE;
	StopPlayItem();
	startSector = FindPrevSequenceHeader(file, startSector);
	SetPlaybackMode(pls_scanning);
	GNREASSERT(TransferStream(file, startSector, 0, !repeatCount, this->scanSpeed));

	CHECK_UOPS;
	GNRAISE_OK;
	}

//
//  Start Backward Scan
//

Error VCDPlayListSequencer::StartBackwardScan(WORD scanSpeed)
	{
	DWORD startSector;
	SAVE_UOPS;

	SetScanSpeed(-(int)(scanSpeed));
	startSector = CurrentLocation() / CD_FRAME_SIZE;
	StopPlayItem();
	startSector = FindPrevSequenceHeader(file, startSector);
	SetPlaybackMode(pls_scanning);
	GNREASSERT(TransferStream(file, startSector, 0, !repeatCount, this->scanSpeed));

	CHECK_UOPS;
	GNRAISE_OK;
	}

//
//  Stop Scan
//

Error VCDPlayListSequencer::StopScan(BOOL toPause)
	{
	DWORD startSector;
	SAVE_UOPS;

	if (scanSpeed)
		{
		SetScanSpeed(0);
		startSector = CurrentLocation() / CD_FRAME_SIZE;
		StopPlayItem();
		startSector = FindPrevSequenceHeader(file, startSector);
		SetPlaybackMode(pls_playing);

		if (toPause) Pause();

		GNREASSERT(TransferStream(file, startSector, 0, !repeatCount));
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);

	CHECK_UOPS;
	GNRAISE_OK;
	}

//
// Time search (only called when playing an S-VCD)
//

DWORD VCDPlayListSequencer::TimeSearch(DVDTime time)
	{
	DWORD sector;

	StopPlayItem();
	SetPlaybackMode(pls_playing);

	sector = FindTimeSearchSector(file, time.Millisecs());

	return sector;
	}

//
//  Start Track At A Sector
//

Error VCDPlayListSequencer::StartTrackAt(WORD track, DWORD startSector, BOOL toPause)
	{
	repeatCount = 0;
	waitTime = 0;
	SetScanSpeed(0);
	playAllTracks = TRUE;
	currentTrack = track;

	GNREASSERT(OpenTrack(track));
	SetPlaybackMode(pls_playing);
	if (startSector) startSector = FindPrevSequenceHeader(file, startSector);

	GNREASSERT(TransferStream(file, startSector, 0, TRUE));
	if (toPause)
		Pause();

	GNRAISE_OK;
	}

//
//  Start Play Item
//  Repeat is equal to 0 except for play items started from a selection list
//

Error VCDPlayListSequencer::StartPlayItem(WORD item, WORD waitTime, WORD repeat, DWORD numSectors, BOOL toPause)
	{
	DVDDiskType diskType;
	SAVE_UOPS;

	GNREASSERT(GetDiskType(diskType));

	SegPlayItemContent spic(diskType == DDT_VIDEO_CD);

	if (waitTime > 0xfe)
		waitTime = 0xff;
	//
	// A 'repeat'-value of 0xffff means an infinity loop.
	// It has the same meaning as waitTime = 0xff and repeat = 0x0.
	// The problem is that a wait time of zero and a repeat value of 0xffff leads
	// to recursive procedure calls. After a while this leads to a deadlock.
	// To avoid this we set the wait time to 0xff and repeat to 0x0.
	// Martin (MST) 08/24/2000
	//
	if (repeat == 0xffff)
		{
		waitTime = 0xff;
		repeat = 0x0;
		}

	DP("Start Play Item Item %d Wait %d Repeat %d NumSectors %d", item, waitTime, repeat, numSectors);

	this->repeatCount = repeat;
	this->waitTime = waitTime;

	playAllTracks = FALSE;
	bpReached = BPS_NONE;
	SetScanSpeed(0);

	if (IsIdle())
		{
		if (item < 2)
			{
			PlaybackCompleted();
			}
		else if (item < 100)
			{
			GNREASSERT(OpenTrack(item));
			SetPlaybackMode(pls_playing);
			GNREASSERT(TransferStream(file, 0, numSectors, !repeat, 0, FALSE, toPause, FALSE));
			}
		else if (item < 600)
			{
			GNREASSERT(OpenTrack(entries->GetEntryTrack(item-99)));
			SetPlaybackMode(pls_playing);
			GNREASSERT(TransferStream(file, file->SectorToBlock(entries->GetEntrySector(item-99)), numSectors, !repeat, 0, FALSE, toPause));
			}
		else if (item < 1000)
			{
			GNRAISE(GNR_RANGE_VIOLATION);
			}
		else if (item < 2980)
			{
			GNREASSERT(OpenSegment(item-999));
			SetPlaybackMode(pls_playing);
			spic = info->GetSegPlayItemContent(item-999);
			GNREASSERT(TransferStream(file, 0, numSectors, !repeat, 0, spic.IsHiResStill(), toPause, FALSE));
			}
		else
			{
			GNRAISE(GNR_RANGE_VIOLATION);
			}
		}
	else
		GNRAISE(GNR_OBJECT_IN_USE);

	//
	//  Check for changes and send events
	//

	CHECK_UOPS;
	SendEvent(DNE_TITLE_CHANGE, item - 1);
	GNRAISE_OK;
	}

//
//  Go to previous play item
//

Error VCDPlayListSequencer::GoPrevItem(void)
	{
	if (list)
		{
		if (currentItem > 1)
			{
			bpReached = BPS_NONE;
			StopPlayItem();
			SetPlaybackMode(pls_idle);
			currentItem--;
			return StartPlayItem(list->PlayItemNumber(currentItem), list->PlayItemWaitTime(), 0, list->PlayingTime());
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	else if (playAllTracks)
		{
		WORD part, entry;

		part = TranslateBlockToPart(currentTrack, CurrentBlock());
		if (part)
			{
			entry = TranslatePartToEntry(currentTrack, part - 1);
			if (entry >= 100)
				return RestartTrackAtItem(entry);
			}

		if (currentTrack > 2)
			currentTrack--;
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		bpReached = BPS_NONE;
		StopPlayItem();
		SetPlaybackMode(pls_idle);
		return StartTrackAt(currentTrack, 0);
		}

	//
	//  No such thing as a previous play item
	//

	currentItem = 0;
	PlayItemCompleted();

	GNRAISE_OK;
	}

//
//  Go to next play item
//

Error VCDPlayListSequencer::GoNextItem(void)
	{
	if (list)
		{
		if (currentItem < list->NumberOfItems())
			{
			bpReached = BPS_NONE;
			StopPlayItem();
			SetPlaybackMode(pls_idle);
			currentItem++;
			return StartPlayItem(list->PlayItemNumber(currentItem), list->PlayItemWaitTime(), 0, list->PlayingTime());
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	else if (playAllTracks)
		{
		WORD part, entry;

		part = TranslateBlockToPart(currentTrack, CurrentBlock());	// Get currently played part
		if (part)
			{
			entry = TranslatePartToEntry(currentTrack, part + 1);
			if (entry >= 100)
				return RestartTrackAtItem(entry);
			}

		if (currentTrack <= NumberOfTitles())
			currentTrack++;
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);

		bpReached = BPS_NONE;
		StopPlayItem();
		SetPlaybackMode(pls_idle);
		return StartTrackAt(currentTrack, 0);
		}

	//
	//  No such thing as a next play item (e.g. single play item from selection list)
	//

	currentItem = 0;
	PlayItemCompleted();

	GNRAISE_OK;
	}

//
//  Still Phase Completed
//

Error VCDPlayListSequencer::StillPhaseCompleted(void)
	{
	if (state == pls_still)
		{
		GNREASSERT(AdvancePlayItem());
		}
	else if (state == pls_autopause)
		{
		SetPlaybackMode(pls_playing);
		GNREASSERT(ContinueAutoPause());
		}

	GNRAISE_OK;
	}

//
//  Start Play List
//

Error VCDPlayListSequencer::StartPlayList(VCDPlayList * list, DWORD flags)
	{
	if (IsIdle())
		{
		this->list = list;

		currentItem = 1;

		GNREASSERT(StartPlayItem(list->PlayItemNumber(currentItem), list->PlayItemWaitTime(), 0,
					  list->PlayingTime(), (flags & DDPSPF_TOPAUSE) != 0));

		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_IN_USE);
	}

//
//  Abort Play List
//

Error VCDPlayListSequencer::AbortPlayList(void)
	{
	if (!IsIdle())
		{
		SetPlaybackMode(pls_stopped);
		GNREASSERT(StopPlayItem());
		CancelStillPhase();

		currentTrack = 0;
		currentSegment = 0;

		if (file)
			{
			delete file;
			file = NULL;
			}

		list = NULL;
		file = NULL;
		}

	GNRAISE_OK;
	}

//
//  Complete Play Item
//

Error VCDPlayListSequencer::CompletePlayItem(void)
	{
	repeatCount = 0;
	waitTime = 0;

	switch (state)
		{
		case pls_stopped:
		case pls_idle:
			PlayItemCompleted();
			break;
		case pls_playing:
			GNREASSERT(CompleteStream());
			break;
		case pls_still:
			CancelStillPhase();
			GNREASSERT(AdvancePlayItem());
			break;
		case pls_autopause:
			GNREASSERT(CompleteStream());
			CancelStillPhase();
			SetPlaybackMode(pls_playing);
			GNREASSERT(ContinueAutoPause());
			break;
		}

	GNRAISE_OK;
	}

//
//  Get Current Play Item
//

WORD VCDPlayListSequencer::CurrentPlayItem(void)
	{
	if (currentSegment)
		return currentSegment + 999;
	else if (currentTrack)
		return currentTrack;
	else
		return 0;
	}

//
//  Get Current Block
//

DWORD VCDPlayListSequencer::CurrentBlock(void)
	{
	DWORD block = CurrentLocation() / CD_FRAME_SIZE;

	if (file)
		return file->AbsoluteBlockAddress(block);
	else
		return 0;
	}

//
//  Freeze current state
//

Error VCDPlayListSequencer::Freeze(VCDPLSState * buffer)
	{
	//
	//  Save data
	//
	buffer->transferCount = CurrentLocation();
	buffer->repeatCount = repeatCount;
	buffer->waitTime = waitTime;
	buffer->currentItem = currentItem;
	buffer->playAllTracks = playAllTracks;
	buffer->currentTrack = currentTrack;

	GNRAISE_OK;
	}

//
//  Restore previously saved state
//

Error VCDPlayListSequencer::Defrost(VCDPLSState * buffer, VCDPlayList * list, DWORD flags)
	{
	DVDDiskType			diskType;
	WORD					item;
	DWORD					currentSector;

	GNREASSERT(GetDiskType(diskType));

	SegPlayItemContent spic(diskType == DDT_VIDEO_CD);

	//
	//  Restore data
	//

	if (buffer->playAllTracks)
		{
		playAllTracks = TRUE;
		this->list = NULL;
		SetScanSpeed(0);
		numSectors = 0;
		SetPlaybackMode(pls_playing);
		currentTrack = buffer->currentTrack;
		GNREASSERT(OpenTrack(currentTrack));

		if (file)
			currentSector = FindPrevSequenceHeader(file, buffer->transferCount / CD_FRAME_SIZE);
		else
			currentSector = 0;

		GNREASSERT(RestartTrackAt(currentSector, (flags & DDPSPF_TOPAUSE) != 0));
		}
	else
		{
		playAllTracks = FALSE;
		this->list = list;
		repeatCount = buffer->repeatCount;
		waitTime = buffer->waitTime;
		currentItem = buffer->currentItem;
		item = list->PlayItemNumber(currentItem);
		SetScanSpeed(0);
		numSectors = 0;

		//
		//  Restart playback
		//

		if (item < 2)
			{
			PlaybackCompleted();
			}
		else if (item < 100)
			{
			GNREASSERT(OpenTrack(item));

			if (file)
				currentSector = FindPrevSequenceHeader(file, buffer->transferCount / CD_FRAME_SIZE);
			else
				currentSector = 0;

			SetPlaybackMode(pls_playing);
			GNREASSERT(TransferStream(file, currentSector, 0, !buffer->repeatCount, scanSpeed,
											  spic.IsHiResStill(), (flags & DDPSPF_TOPAUSE) != 0));
			}
		else if (item < 600)
			{
			GNREASSERT(OpenTrack(entries->GetEntryTrack(item - 99)));

			if (file)
				currentSector = FindPrevSequenceHeader(file, buffer->transferCount / CD_FRAME_SIZE);
			else
				currentSector = 0;

			SetPlaybackMode(pls_playing);
			GNREASSERT(TransferStream(file, currentSector, list->PlayingTime(), !buffer->repeatCount, scanSpeed,
											  spic.IsHiResStill(), (flags & DDPSPF_TOPAUSE) != 0));
			}
		else if (item < 1000)
			{
			GNRAISE(GNR_RANGE_VIOLATION);
			}
		else if (item < 2980)
			{
			GNREASSERT(OpenSegment(item - 999));
			SetPlaybackMode(pls_playing);
			spic = info->GetSegPlayItemContent(item - 999);
			GNREASSERT(TransferStream(file, 0, 0, !buffer->repeatCount, scanSpeed, spic.IsHiResStill(), (flags & DDPSPF_TOPAUSE) != 0));
			}
		else
			{
			GNRAISE(GNR_RANGE_VIOLATION);
			}
		}

	GNRAISE_OK;
	}


//
//  Pause Playback
//

Error VCDPlayListSequencer::Pause(void)
	{
	GNREASSERT(player->Pause());
	SetPlaybackMode(pls_paused);
	GNRAISE_OK;
	}

//
//  Resume Playback
//

Error VCDPlayListSequencer::Resume(void)
	{
	//
	// Resume playback
	//

	GNREASSERT(player->Resume());
	SetPlaybackMode(pls_playing);
	SetScanSpeed(0);
	GNRAISE_OK;
	}

//
//  Enter Stop State
//

Error VCDPlayListSequencer::EnterStopState(void)
	{
	SetPlaybackMode(pls_stopped);

	GNRAISE_OK;
	}

//
//  Pause due to breakpoint
//

void VCDPlayListSequencer::StartBreakpointStillCallback(void)
	{
	StartStillPhase(255);
	SetPlaybackMode(pls_still);
	player->Pause();
	}

//
//  Internal Get Mode
//

DVDPlayerMode VCDPlayListSequencer::InternalGetMode(void)
	{
	if (IsStillPhase())
		return DPM_STILL;
	else if (IsPlaying())
		return DPM_PLAYING;
	else if (IsPaused())
		return DPM_PAUSED;
	else if (IsScanning())
		return DPM_SCANNING;
	else
		return DPM_STOPPED;
	}

//
//  Number Of Titles
//

WORD VCDPlayListSequencer::NumberOfTitles(void)
	{
	if (numberOfTitles)
		return numberOfTitles;
	else
		{
		int i, t, m;

		m = entries->EntriesUsed();
		numberOfTitles = 1;
		for(i = 1; i <= m; i++)
			{
			t = entries->GetEntryTrack(i);
			if (t > numberOfTitles) numberOfTitles = (WORD)t;
			}
		numberOfTitles--;

		return numberOfTitles;
		}
	}

//
//  Translate Item (Track) and Part into Entry of ENTRIES.VCD
//

WORD VCDPlayListSequencer::TranslatePartToEntry(WORD track, WORD part)
	{
	int i, t, m;

	m = entries->EntriesUsed();
	for (i = 1; i <= m; i++)
		{
		t = entries->GetEntryTrack(i);
		if (t == track)
			{
			part--;
			if (!part) return i + 99;
			}
		}

	return track;
	}

//
//  Translate Item (Track) and Block into Part (Entry in ENTRIES.VCD)
//

WORD VCDPlayListSequencer::TranslateBlockToPart(WORD track, DWORD block)
	{
	int i, t, m;
	DWORD blk;
	WORD num = 0;

	m = entries->EntriesUsed();
	for (i = 1; i <= m; i++)
		{
		t = entries->GetEntryTrack(i);
		if (t == track)
			{
			blk = entries->GetEntrySector(i);
			if (blk <= block) num++;
			}
		}

	return num;
	}

//
//  Test if we are playing forward
//

BOOL VCDPlayListSequencer::IsPlayingForward(void)
	{
	return !(state == pls_scanning && scanSpeed < 0);
	}

//
//  Return current duration (in blocks/sectors) (called when using a VCD)
//

Error VCDPlayListSequencer::CurrentDuration(DWORD & dur)
	{
	if (state != pls_still)
		return VCDStreamSequencer::CurrentDuration(dur);
	else
		{
		if (waitTime != 0xff)
			dur = waitTime * CD_FRAME_RATE * CD_FRAME_SIZE;
		else
			dur = 0;
		GNRAISE_OK;
		}
	}

//
// Return current duration in seconds (only called when using a Super-VCD)
//

Error VCDPlayListSequencer::CurrentPlayingTime(DWORD & time)
	{
	if (tracks)
		{
		if (currentSegment)
			{
			GNREASSERT(CurrentDuration(time));
			time /= (75 * CD_FRAME_SIZE);
			}
		else if (currentTrack)
			return tracks->TrackPlayingTime(currentTrack-1);
		else
			time = 0;
		}
	else
		{
		// SPECIAL CASE: S-VCD without the file tracks.vcd
		return currentDurationInMilliSecs/1000;
		}

	GNRAISE_OK;
	}

//
//  Get available streams
//

Error VCDPlayListSequencer::GetAvailStreams(BYTE & audio, DWORD & subPicture)
	{
	BOOL isDualAudio;

	if (tracks)
		{
		if (currentTrack)
			{
			isDualAudio = tracks->IsDualAudioStream(currentTrack-1);
				{
				audio = 0x0;
				for(int strm = 0; strm < 2; strm++)
					{
					audio |= MKFLAG(strm);
					}
				}
			}
		else
			audio = MKFLAG(0);
		}
	else
		audio = MKFLAG(0);

	subPicture = 0x00000000;

	GNRAISE_OK;
	}

//
//  Get current audio stream
//

WORD VCDPlayListSequencer::GetCurrentAudioStream(void)
	{
	return player->GetAudioStreamChannel();
	}

Error VCDPlayListSequencer::GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format)
	{
	format.languageCode = 0xffff;
	format.languageExtension = 0xffff;
	format.codingMode = DAM_MPEG1;			// VCD --> MPEG-1, S-VCD --> MPEG-1 or MPEG-2
	format.bitsPerSample = 16;
	format.samplesPerSecond = 44100;

	if (currentTrack && tracks)
		{
		if (tracks->IsMultiChannelAudioTrack(currentTrack))
			format.channels = 5+1;
		else
			format.channels = 2;
		}
	else
		format.channels = 2;

	GNRAISE_OK;
	}

//
//  Audio stream change
//

Error VCDPlayListSequencer::AudioStreamChange(WORD stream)
	{
	return player->SetAudioStreamChannel(stream);
	}
