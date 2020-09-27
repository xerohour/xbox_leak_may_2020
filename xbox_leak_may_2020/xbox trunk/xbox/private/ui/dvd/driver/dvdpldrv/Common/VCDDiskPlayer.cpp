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

//////////////////////////////////////////////////////////////////////
//
//  VCDDiskPlayerClass
//
//////////////////////////////////////////////////////////////////////

#include "VCDDiskPlayer.h"

//
//  VCDDiskPlayerClass Constructor
//

VCDDiskPlayerClass::VCDDiskPlayerClass(WinPortServer * server, UnitSet units)
	: CDDiskPlayerClass() //this one HAS TO be called before constructors that pass &eventDispatcherInstance
	, VCDSelectionListSequencer(server, units, &eventDispatcherInstance)
	, EventSender(&eventDispatcherInstance)
	, ERSBreakpointControl(&eventDispatcherInstance)
	{
	search = NULL;
	tracks = NULL;
	psd = NULL;
	entries = NULL;
	info = NULL;
	vcdfs = NULL;
	}

//
//  Destructor
//

VCDDiskPlayerClass::~VCDDiskPlayerClass(void)
	{
	Exit();
	delete search;
	delete tracks;
	delete psd;
	delete entries;
	delete info;
	if (vcdfs)
		{
		vcdfs->SetEventDispatcher(NULL);
		vcdfs->Release();
		}
	}

//
//  Initialize
//

Error VCDDiskPlayerClass::Init(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType, VCDFileSystem * vcdfs)
	{

	if (vcdfs)
		vcdfs->SetEventDispatcher(&eventDispatcherInstance);

	GNREASSERT(CDDiskPlayerClass::Init(server, profile, diskType, vcdfs));

	//
	//  Build file system
	//

	this->vcdfs = vcdfs;

	//
	//  Create entries (mandatory)
	//

	entries = new VCDEntries();
	if (!entries)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT(entries->Initialize(vcdfs, diskType));

	//
	//  Create info (mandatory) and PSD(X) (optional)
	//

	info = new VCDInfo();
	if (!info)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	GNREASSERT(info->Initialize(vcdfs));

	if (info->HasPSDX())
		{
		psd = new VCDPSDX();
		if (!psd)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);

		if (IS_ERROR(psd->InitializeExtended(vcdfs)))
			{
			delete psd;
			psd = NULL;
			}
		}
	else
		{
		psd = new VCDPSD();
		if (!psd)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);

		if (IS_ERROR(psd->Initialize(vcdfs)))
			{
			delete psd;
			psd = NULL;
			}
		}

	//
	//  Create tracks
	//

	tracks = new VCDTracks();
	if (tracks)
		{
		if(IS_ERROR(tracks->Initialize(vcdfs)))
			{
			delete tracks;
			tracks = NULL;
			}
		}

	//
	//  Create search
	//

	search = new VCDSearch();
	if (search)
		{
		if(IS_ERROR(search->Initialize(vcdfs)))
			{
			delete search;
			search = NULL;
			}
		}

	return VCDSelectionListSequencer::Initialize(vcdfs, info, entries, tracks, search, psd);
	}

//
//  Get disk type
//

Error VCDDiskPlayerClass::GetDiskType(DVDDiskType & type)
	{
	return CDDiskPlayerClass::GetDiskType(type);
	}

//
//  Internal: Get current playback mode
//

Error VCDDiskPlayerClass::InternalGetMode(DVDPlayerMode & mode)
	{
	mode = VCDSelectionListSequencer::InternalGetMode();
	GNRAISE_OK;
	}

//
//  Return current player mode
//


Error VCDDiskPlayerClass::GetMode(DVDPlayerMode & mode)
	{
	GNREASSERT(InternalGetMode(mode));

	if (mode != DPM_STOPPED && player->IsPreempted())
		{
		Exit(GNR_LOST_DECODER);

		mode = DPM_STOPPED;
		}

	GNRAISE_OK;
	}

//
//  Get extended player state
//

Error VCDDiskPlayerClass::GetExtendedPlayerState(ExtendedPlayerState * appEPS)
	{
	WORD stream;
	int i;

	//
	//  Reset bitmasks
	//

	eps.valid = 0;
	eps.changed = 0;

	//
	//  Set VCD specific data
	//

	//
	//  Button Information
	//

	if (EPS_REQUEST(EPS_BUTTON))
		{
		GetButtonInformation(eps.numberOfButtons, eps.userButtonOffset, eps.buttonInfo);
		eps.selectedButton = eps.forcedlyActivatedButton = eps.numberOfUserButtons = 0;

		eps.valid |= EPS_BUTTON;
		if (EPS_MONITOR(EPS_BUTTON))
			{
			if (EPS_CHANGED(numberOfButtons) || EPS_CHANGED(userButtonOffset))
				eps.changed |= EPS_BUTTON;

			for (i=0; i<eps.numberOfButtons; i++)
				{
				if (EPS_CHANGED(buttonInfo[i]))
					{
					eps.changed |= EPS_BUTTON;
					break;
					}
				}
			}
		}

	//
	//  Get audio and subpicture stream attributes
	//

	GetAvailStreams(eps.availableAudioStreams, eps.availableSubPictureStreams);

	//
	//  Get Audio Stream Data
	//

	if (EPS_REQUEST(EPS_AUDIOSTREAMS))
		{
		GNREASSERT(GetCurrentAudioStream(stream));
		eps.currentAudioStream = (WORD)stream;

		//
		// a S-VCD can have a maximum of 2 audio streams
		//	whereas a VCD can have only 1 audio stream
		// So let the loop go two times
		//
		for (i=0; i<2; i++)
			{
			if (eps.availableAudioStreams & (1 << i))
				GetAudioStreamAttributes((WORD)i, eps.audioStream[i]);
			}

		GNREASSERT(player->GetAudioInformation(eps.mpeg2PrologicStatus, eps.mpeg2LFEStatus, eps.ac3AudioCodingMode));

		eps.valid |= EPS_AUDIOSTREAMS;

		if (EPS_MONITOR(EPS_AUDIOSTREAMS))
			{
			if (EPS_CHANGED(currentAudioStream) || EPS_CHANGED(availableAudioStreams) || EPS_CHANGED(audioStream[0]) ||
				 EPS_CHANGED(mpeg2PrologicStatus) || EPS_CHANGED(mpeg2LFEStatus) || EPS_CHANGED(ac3AudioCodingMode))
				eps.changed |= EPS_AUDIOSTREAMS;
			for (i=0; i<2; i++)
				{
				if (EPS_CHANGED(audioStream[i]))
					{
					eps.changed |= EPS_AUDIOSTREAMS;
					break;
					}
				}
			}
		}

	//
	//  Set common data
	//

	return CDDiskPlayerClass::GetExtendedPlayerState(appEPS);
	}

Error VCDDiskPlayerClass::StopStillPhase(void)
	{
	return VCDSelectionListSequencer::StopStillPhase();
	}

Error VCDDiskPlayerClass::Exit(Error err)
	{
	SendEvent(DNE_PLAYBACK_MODE_CHANGE, DPM_STOPPED);
	if (IS_ERROR(err))
		SendEvent(DNE_ERROR, err);
	return AbortActionList();
	}

Error VCDDiskPlayerClass::CallMenu(VTSMenuType menu)
	{
	if (psd)
		{
		AbortActionList();
		return StartActionList(0x0000);
		}
	else
		GNRAISE_OK;
	}

Error VCDDiskPlayerClass::PausePlayback(void)
	{
	if (IsScanning())
		return VCDSelectionListSequencer::StopScan(TRUE);
	else
		return VCDSelectionListSequencer::Pause();
	}

Error VCDDiskPlayerClass::ResumePlayback(void)
	{
	vcdfs->SpinUpDrive();
	return VCDSelectionListSequencer::Resume();
	}

Error VCDDiskPlayerClass::AdvanceFrame(void)
	{
	return player->Step();
	}

Error VCDDiskPlayerClass::AdvanceFrameBy(int n)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::SetPlaybackSpeed(WORD speed)
	{
	return player->SetPlaybackSpeed(speed);
	}

Error VCDDiskPlayerClass::GetPlaybackSpeed(WORD & speed)
	{
	speed = player->GetPlaybackSpeed();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::IsPlayingForward(BOOL & forward)
	{
	forward = VCDPlayListSequencer::IsPlayingForward();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetAudioStreamAttributes(WORD stream, DVDAudioStreamFormat & attributes)
	{
	return VCDPlayListSequencer::GetAudioStreamAttributes(stream, attributes);
	}

Error VCDDiskPlayerClass::GetSubPictureStreamAttributes(WORD stream, DVDSubPictureStreamFormat	& attributes)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::StartPresentation(DWORD flags)
	{
	if (psd)
		return StartActionList(0x0000, flags);
	else
		return GoTitle(1, flags);
	}

//
//  Get current location
//

Error VCDDiskPlayerClass::GetCurrentLocation(DVDLocation & location)
	{
	DVDTime	time;
	DWORD loc;
	WORD item;
	DVDDiskType diskType;

	if (IsPlaying() || IsStillPhase() || IsScanning() || IsPaused())
		{
		item = CurrentPlayItem();

		if (item <= 99)
			{
			location.domain = TT_DOM;
			location.title = item - 1;
			location.partOfTitle = TranslateBlockToPart(item, CurrentBlock());
			if (!location.partOfTitle) location.partOfTitle = 1;
			}
		else
			{
			location.domain = VMGM_DOM;
			location.title = 1;
			location.partOfTitle = 1;
			}

		GNREASSERT(GetDiskType(diskType));
		if (diskType == DDT_VIDEO_CD)
			{
			loc = CurrentLocation()  / (CD_FRAME_RATE * CD_FRAME_SIZE);
			time = DVDTime(0, 0, loc, 0, player->GetCurrentFrameRate());
			}
		else if (diskType == DDT_SUPER_VIDEO_CD)
			{
			//
			// do not show any time when in still mode
			//

			if (IsStillPhase())
				time = 0;
			else
				GetPlaybackTime(time);
			}

		location.videoTitleSet = 1;
		location.vtsTitle = location.title;
		location.titleTime = time;
		location.programChain = location.title;
		location.pgcTime = time;
		location.program = location.partOfTitle;
		location.cell = location.program;
		location.cellTime = time;
		}
	else
		{
		location.domain = STOP_DOM;
		}

	GNRAISE_OK;
	}

//
//  Get current duration
//

Error VCDDiskPlayerClass::GetCurrentDuration(DVDLocation & location)
	{
	DVDTime	time;
	DWORD		dur;
	DVDDiskType diskType;

	if (IsPlaying() || IsStillPhase() || IsScanning() || IsPaused())
		{
		if (CurrentPlayItem() <= 99)
			location.domain = TT_DOM;
		else
			location.domain = VMGM_DOM;
		}
	else
		{
		location.domain = STOP_DOM;
		}

	GNREASSERT(GetDiskType(diskType));
	if (diskType == DDT_VIDEO_CD)
		{
		GNREASSERT(CurrentDuration(dur));
		dur = dur / (CD_FRAME_RATE * CD_FRAME_SIZE);
		time = DVDTime(0, 0, dur, 0, 25);
		}
	else if (diskType == DDT_SUPER_VIDEO_CD)
		{
		//
		// note that CurrentPlayingTime() returns the playing time in seconds
		//

		GNREASSERT(CurrentPlayingTime(dur));
		time = DVDTime(0, 0, dur, 0, 25);
		}

	location.videoTitleSet = 1;
	location.title = 1;
	location.vtsTitle = 1;
	location.titleTime = time;
	location.partOfTitle = 1;
	location.programChain = 1;
	location.pgcTime = time;
	location.program = 1;
	location.cell = 1;
	location.cellTime = time;

	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetTitleDuration(WORD title, DVDTime & duration)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::MenuAvail(VTSMenuType menu, BOOL & avail)
	{
	avail = (psd != NULL) && (menu == VMT_TITLE_MENU);
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetUOPs(DWORD & uops)
	{
	GNREASSERT(CDDiskPlayerClass::GetUOPs(uops));

	uops |=	UOP_MENU_CALL_ROOT
		  |	UOP_MENU_CALL_SUB_PICTURE
		  |	UOP_MENU_CALL_AUDIO
		  |	UOP_MENU_CALL_ANGLE
		  |	UOP_MENU_CALL_PTT
		  |	UOP_RESUME
		  |	UOP_SUB_PICTURE_STREAM_CHANGE
		  |	UOP_ANGLE_CHANGE
		  |	UOP_KARAOKE_MODE_CHANGE
		  |	UOP_VIDEO_MODE_CHANGE;

	if (InPlayList())
		{
		uops |= UOP_BUTTON;
		}
	else if (InSelectionList())
		{
		}
	else
		{
		uops |= UOP_BUTTON |
				 UOP_GO_UP;
		}

	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::UOPPermitted(int uop, BOOL & permitted)
	{
	DWORD uops;

	GNREASSERT(GetUOPs(uops));

	permitted = !XTBF(uop, uops);
	GNRAISE_OK;
	}

//
//  Return Number Of Titles, computed from ENTRIES.VCD
//

Error VCDDiskPlayerClass::NumberOfTitles(WORD & num)
	{
	num = VCDSelectionListSequencer::NumberOfTitles();
	GNRAISE_OK;
	}

//
//  Compute number of Part Of Title from number of entries for a track in ENTRIES.VCD
//

Error VCDDiskPlayerClass::NumberOfPartOfTitle(WORD title, WORD & num)
	{
	int i, t, m;

	m = entries->EntriesUsed();
	num = 0;
	for(i = 1; i <= m; i++)
		{
		t = entries->GetEntryTrack(i) - 1;
		if (t == title) num++;
		}

	if (num == 0) num = 1;

	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetAvailStreams(BYTE & audio, DWORD & subPicture)
	{
	return VCDSelectionListSequencer::GetAvailStreams(audio, subPicture);
	}

Error VCDDiskPlayerClass::GetCurrentAudioStream(WORD & stream)
	{
	stream = VCDSelectionListSequencer::GetCurrentAudioStream();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetCurrentSubPictureStream(WORD & stream)
	{
	stream = 0;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::IsCurrentSubPictureEnabled(BOOL & enabled)
	{
	enabled = FALSE;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetNumberOfAngles(WORD title, WORD & num)
	{
	num = 1;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetCurrentAngle(WORD & angle)
	{
	angle = 1;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GoTitle(WORD title, DWORD flags)
	{
	return VCDSelectionListSequencer::TitlePlay(title, flags);
	}

Error VCDDiskPlayerClass::ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time)
	{
	DVDLocation loc;
	WORD item;

	GetCurrentLocation(loc);

	//
	//  Start with title
	//

	if (!(flags & DDPEPF_USE_TITLE))
		title = loc.title;

	//
	//  Start part of title
	//

	if (!(flags & DDPEPF_USE_PTT))
		ptt = loc.partOfTitle;

	//
	//  Start current title at at time
	//

	if (!(flags & DDPEPF_USE_TIME))
		time = DVDTime(0, 0, 0, 0, loc.titleTime.FrameRate());

	//
	//  Now start playback
	//

	item = TranslatePartToEntry(title + 1, ptt);
	if (item)
		{
		return VCDSelectionListSequencer::TimePlay(title, time + DVDTime(0, 0, 0, entries->GetEntrySector(item),
																 time.FrameRate()),
																(flags & DDPEPF_PAUSE_AT_START) != 0);
		}
	else
		GNRAISE(GNR_INVALID_PARAMETERS);
	}

Error VCDDiskPlayerClass::GoPartOfTitle(WORD title, WORD part)
	{
	WORD item = TranslatePartToEntry(title + 1, part);

	if (item)
		{
		AbortActionList();
		GNREASSERT(StartPlayItem(item, 0));
		}

	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::TimePlay(WORD title, DVDTime time)
	{
	return VCDSelectionListSequencer::TimePlay(title, time);
	}

Error VCDDiskPlayerClass::GoUpProgramChain(void)
	{
	return GoReturnList();
	}

Error VCDDiskPlayerClass::TimeSearch(DVDTime time)
	{
	return RestartTrackAt(time.Millisecs() * 3 / 40);
	}

//
//  Go Part Of Title
//

Error VCDDiskPlayerClass::GoPartOfTitle(WORD part)
	{
	WORD item = CurrentPlayItem();

	if (item >= 2 && item <= 99)
		{
		item = TranslatePartToEntry(item, part);

		if (item)
			{
			AbortActionList();
			GNREASSERT(StartPlayItem(item, 0));
			}
		}

	GNRAISE_OK;
	}

//
//  Go Prev Program
//

Error VCDDiskPlayerClass::GoPrevProgram(void)
	{
	return PreviousFunction();
	}

//
//  Go Top Program
//

Error VCDDiskPlayerClass::GoTopProgram(void)
	{
	WORD item, part;

	if (IsPlaying() || IsScanning())
		{
		item = CurrentPlayItem();

		if (item >= 2 && item <= 99)
			{
			part = TranslateBlockToPart(item, CurrentBlock());
			if (!part) part = 1;

			item = TranslatePartToEntry(item, part);
			if (item)
				return RestartTrackAtItem(item);
			}
		}

	return GoTopList();
	}

//
//  Go Next Program
//

Error VCDDiskPlayerClass::GoNextProgram(void)
	{
	return NextFunction();
	}

Error VCDDiskPlayerClass::StartForwardScan(WORD speed)
	{
	return VCDSelectionListSequencer::StartForwardScan(speed);
	}

Error VCDDiskPlayerClass::StartBackwardScan(WORD speed)
	{
	return VCDSelectionListSequencer::StartBackwardScan(speed);
	}

Error VCDDiskPlayerClass::GetScanSpeed(WORD & speed)
	{
	speed = VCDSelectionListSequencer::GetScanSpeed();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::StartTrickplay(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::StopScan(void)
	{
	return VCDSelectionListSequencer::StopScan(FALSE);
	}

Error VCDDiskPlayerClass::StartReversePlayback(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ResumeFromSystemSpace(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonUp(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonDown(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonLeft(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonRight(void)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonSelectAt(WORD x, WORD y)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ButtonActivate(void)
	{
	return GoDefaultList();
	}

Error VCDDiskPlayerClass::ButtonSelectAndActivate(WORD button)
	{
	return GoSelectionList(button);
	}

Error VCDDiskPlayerClass::ButtonSelectAtAndActivate(WORD x, WORD y)
	{
	return GoSelectionListAt(x, y);
	}

Error VCDDiskPlayerClass::IsButtonAt(WORD x, WORD y, BOOL & isButton)
	{
	isButton = VCDSelectionListSequencer::IsButtonAt(x, y);
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::HasPositionalButtons(BOOL & hasButtons)
	{
	hasButtons = VCDSelectionListSequencer::HasPositionalButtons();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::MenuLanguageSelect(WORD language)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::GetMenuLanguage(WORD & language)
	{
	language = 0xffff;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::AudioStreamChange(WORD stream)
	{
	return VCDSelectionListSequencer::AudioStreamChange(stream);
	}

Error VCDDiskPlayerClass::SubPictureStreamChange(WORD stream, BOOL enable)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::AngleChange(WORD angle)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ParentalLevelSelect(WORD level)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::ParentalCountrySelect(WORD country)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::GetCurrentDisplayMode(DisplayPresentationMode & mode)
	{
	mode = DPM_4BY3;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::SetDisplayMode(DisplayPresentationMode mode)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::GetCurrentVideoStandard(VideoStandard & standard)
	{
	standard = player->GetCurrentVideoStandard();
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::SelectInitialLanguage(WORD audioLanguage, WORD audioExtension, WORD subPictureLanguage, WORD subPictureExtension)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error VCDDiskPlayerClass::GetCurrentBitrate(DWORD & bitrate)
	{
	bitrate = 1350000;
	GNRAISE_OK;
	}

Error VCDDiskPlayerClass::GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton)
	{
	return VCDSelectionListSequencer::GetCurrentButtonState(minButton, numButtons, currentButton);
	}

//
//  Freeze current player state
//

Error VCDDiskPlayerClass::Freeze(BYTE * buffer, DWORD & size)
	{
	//
	//  Check buffer size
	//

	if (size < sizeof(VCDDPCFreezeState))
		{
		size = sizeof(VCDDPCFreezeState);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}

	//
	// BuildUniqueKey
	//

	GNREASSERT(info->BuildUniqueKey(((VCDDPCFreezeState*)buffer)->uniqueKey));
	size = sizeof(VCDDPCFreezeState);

	//
	//  Save player state
	//

	return VCDSelectionListSequencer::Freeze(&((VCDDPCFreezeState*)buffer)->vcdSLSState);
	}

//
//  Restore player state
//

Error VCDDiskPlayerClass::Defrost(BYTE * buffer, DWORD & size, DWORD flags)
	{
	BYTE uniqueKey[8];
	int i;

	//
	// Test buffer size
	//

	if (size < sizeof(VCDDPCFreezeState))
		{
		size = sizeof(VCDDPCFreezeState);
		GNRAISE(GNR_OBJECT_INVALID);
		}
	size = sizeof(VCDDPCFreezeState);

	//
	// Check unique key
	//

	GNREASSERT(info->BuildUniqueKey(uniqueKey));
	for (i=0; i<8; i++)
		{
		if (((VCDDPCFreezeState*)buffer)->uniqueKey[i] != uniqueKey[i])
			GNRAISE(GNR_INVALID_UNIQUE_KEY);
		}

	//
	// Defrost lower level data
	//

	if (!XTBF(DDPSPF_ID_MATCH, flags))
		{
		if (IS_ERROR(VCDSelectionListSequencer::Defrost(&((VCDDPCFreezeState*)buffer)->vcdSLSState, flags)))
			{
			Exit(GNR_INVALID_NAV_INFO);
			GNRAISE(GNR_OBJECT_INVALID);
			}
		}

	GNRAISE_OK;
	}

//
//  Set Breakpoint
//

Error VCDDiskPlayerClass::SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id)
	{
	if (flags & ERS_ENDOFPTT)
		GNRAISE(GNR_OPERATION_NOT_SUPPORTED);

	if (title == 0)
		{
		DVDLocation loc;

		GetCurrentLocation(loc);
		title = loc.title;
		}

	return VCDSelectionListSequencer::SetBreakpoint(title, ptt, time, flags, id);
	}

//
//  Clear Breakpoint
//

Error VCDDiskPlayerClass::ClearBreakpoint(DWORD id)
	{
	return VCDSelectionListSequencer::ClearBreakpoint(id);
	}
