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
//  DVDDiskPlayerClass
//
//////////////////////////////////////////////////////////////////////

#include "DVDDiskPlayer.h"

#define MKLANG(c, d)	(((WORD)c << 8) | (WORD)d)

//
//  Constructor
//

DVDDiskPlayerClass::DVDDiskPlayerClass(WinPortServer * server, UnitSet units, DVDDiskPlayerFactory * factory)
	: CDDiskPlayerClass() //this one HAS TO be called before constructors that pass &eventDispatcherInstance
	, WinPort(server)
	, DVDDiskSequencer(server, units, factory, &eventDispatcherInstance)
	, EventSender(&eventDispatcherInstance)
	, ERSBreakpointControl(&eventDispatcherInstance)
	{
	dvdfs = NULL;
	}

//
//  Destructor
//

DVDDiskPlayerClass::~DVDDiskPlayerClass(void)
	{
	Exit();
	if (dvdfs)
		{
		dvdfs->SetEventDispatcher(NULL);
		dvdfs->Release();
		}
	}

//
//  Initialize
//

Error DVDDiskPlayerClass::Init(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType, DVDFileSystem * dvdfs, BYTE region)
	{
	BOOL enc;

	if (dvdfs)
		dvdfs->SetEventDispatcher(&eventDispatcherInstance);

	//
	//  Initialize base classes
	//

	GNREASSERT(CDDiskPlayerClass::Init(server, profile, diskType, dvdfs));
	GNREASSERT(DVDDiskSequencer::Init(dvdfs, MKLANG('e', 'n')));

	//
	//  Initialize *PRMs
	//

	ClearGPRMs();

	SetSPRM(0, MKLANG('e', 'n'));
	SetSPRM(1, 15);
	SetSPRM(2, 63);
	SetSPRM(9, 0);
	SetSPRM(10, 0);
	SetSPRM(13, 15);
	SetSPRM(16, MKLANG('e', 'n'));
	SetSPRM(18, MKLANG('e', 'n'));
	SetSPRM(20, region ^ 0xff);

	stillPhase = FALSE;

	//
	//  Check if disk is encrypted and perform key exchange if so
	//

	GNREASSERT(dvdDisk->GetDiskFileSystem()->DVDIsEncrypted(enc));
	if (enc)
		GNREASSERT(TransferDiskKey());

	else
		{
		// try for the encrypted but not marked disc(CliffHanger/CableGuy WS)
		DVDHeaderFile * headerFile;
		Error err;

		err = mgm->GetHeaderFile(headerFile);
		if (!IS_ERROR(err))
			{
			err = player->TransferDiskKey(headerFile);
			}
		}

	//
	//  Store the file system. This must not be done in case of errors, otherwise it will be deleted
	//  twice and the system will crash.
	//

	this->dvdfs = dvdfs;
	GNRAISE_OK;
	}

//
//  Test if DVD is encrypted
//

Error DVDDiskPlayerClass::DiskIsEncrypted(BOOL & enc)
	{
	return DVDDiskSequencer::DiskIsEncrypted(enc);
	}

//
//  Pause playback
//

Error DVDDiskPlayerClass::PausePlayback(void)
	{
	if (!stillPhase && GetDomain() != STOP_DOM)
		{
		if (IsScanning())
			return DVDDiskSequencer::StopScan(TRUE);
		else
			return DVDDiskSequencer::Pause();
		}
	else
		GNRAISE_OK;
	}

//
//  Resume playback
//

Error DVDDiskPlayerClass::ResumePlayback(void)
	{
	dvdDisk->GetDiskFileSystem()->SpinUpDrive();
	return DVDDiskSequencer::Resume();
	}

Error DVDDiskPlayerClass::AdvanceFrame(void)
	{
	if (!stillPhase && GetDomain() != STOP_DOM)
		{
		return player->Step();
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

Error DVDDiskPlayerClass::AdvanceFrameBy(int n)
	{
	if (!stillPhase && GetDomain() != STOP_DOM)
		{
		return player->StepBy(n);
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);
	}

Error DVDDiskPlayerClass::SetPlaybackSpeed(WORD speed)
	{
	return player->SetPlaybackSpeed(speed);
	}

Error DVDDiskPlayerClass::IsPlayingForward(BOOL & forward)
	{
	forward = DVDPGCSequencer::IsPlayingForward();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::GetPlaybackSpeed(WORD & speed)
	{
	speed = player->GetPlaybackSpeed();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::ClearGPRMs(void)
	{
	return navpu->ClearGPRMs();
	}

Error DVDDiskPlayerClass::GetAudioStreamAttributes(WORD stream, DVDAudioStreamFormat & attributes)
	{
	if (GetDomain() == STOP_DOM)
		{
		attributes.languageCode = 0;
		attributes.languageExtension = 0;
		attributes.codingMode = DAM_AC3;
		attributes.bitsPerSample = 16;
		attributes.samplesPerSecond = 48000;
		attributes.channels = 2;
		attributes.applicationMode = DAAM_UNDEFINED;
		attributes.channelAssignment = 0;
		attributes.mcIntro = 0;
		attributes.soloDuetMode = DKM_UNDEFINED;

		GNRAISE_OK;
		}
	else
		return vobs->GetAudioStreamAttributes(stream, attributes);
	}

Error DVDDiskPlayerClass::GetSubPictureStreamAttributes(WORD stream, DVDSubPictureStreamFormat & attributes)
	{
	if (GetDomain() == STOP_DOM)
		{
		attributes.languageCode = 0;

		GNRAISE_OK;
		}
	else
		return vobs->GetSubPictureStreamAttributes(stream, attributes);
	}

//
//  Message processing for still and playback phase
//

void DVDDiskPlayerClass::Message(WPARAM wParam, LPARAM dParam)
	{
	switch (wParam)
		{
		case STILL_TIME_MSG:
			if (stillPhase)
				{
				stillPhase = FALSE;
				StillPhaseCompleted();
				StateChanged();
				}
			break;
		}
	}

//
//  Still Phase Handling
//

Error DVDDiskPlayerClass::StartStillPhase(WORD delay)
	{
	if (delay == 0)
		GNREASSERT(StillPhaseCompleted());
	else
		{
		DVDTitleSequencer::StartStillPhase(delay);
		stillPhase = TRUE;
		if (delay < 255)
			ScheduleTimer(STILL_TIME_MSG, (DWORD)delay * 1000);
		SendEvent(DNE_STILL_ON, delay);
		}

	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::StopStillPhase(void)
	{
	Error err = GNR_OK;

	if (stillPhase)
		{
		CancelTimer();
		stillPhase = FALSE;
		err = StillPhaseCompleted();
		if (!IS_ERROR(err))
			SendEvent(DNE_STILL_OFF, 0);
		}

	GNRAISE(err);
	}

Error DVDDiskPlayerClass::CancelStillPhase(void)
	{
	if (stillPhase)
		{
		CancelTimer();
		stillPhase = FALSE;
		SendEvent(DNE_STILL_OFF, 0);
		}

	GNRAISE_OK;
	}


Error DVDDiskPlayerClass::Exit(Error err)
	{
	if (IS_ERROR(err))
		SendEvent(DNE_ERROR, err);
	GNREASSERT(DVDDiskSequencer::Exit(err));

	GNRAISE_OK;
	}

//
//  Internal: Get current playback mode
//

Error DVDDiskPlayerClass::InternalGetMode(DVDPlayerMode & mode)
	{
	mode = DVDDiskSequencer::InternalGetMode();
	GNRAISE_OK;
	}

//
//  Get Mode
//

Error DVDDiskPlayerClass::GetMode(DVDPlayerMode & mode)
	{
	GNREASSERT(InternalGetMode(mode));

	GNRAISE_OK;
   }

//
//  Get Extendend Player State
//

Error DVDDiskPlayerClass::GetExtendedPlayerState(ExtendedPlayerState * appEPS)
	{
	DVDLocation loc;
	int i;
	WORD stream;

	//
	//  Reset bitmasks and get general data
	//

	eps.valid = 0;
	eps.changed = 0;
	GetCurrentLocation(loc);

	//
	//  Get audio and subpicture stream attributes
	//

	GNREASSERT(GetAvailStreams(eps.availableAudioStreams, eps.availableSubPictureStreams));

	if (EPS_REQUEST(EPS_AUDIOSTREAMS))
		{
		GNREASSERT(GetCurrentAudioStream(stream));
		eps.currentAudioStream = (WORD)stream;

		for (i=0; i<8; i++)
			{
			if (eps.availableAudioStreams & (1 << i))
				GetAudioStreamAttributes((WORD)i, eps.audioStream[i]);
			}

		GNREASSERT(player->GetAudioInformation(eps.mpeg2PrologicStatus, eps.mpeg2LFEStatus, eps.ac3AudioCodingMode));

		eps.valid |= EPS_AUDIOSTREAMS;

		if (EPS_MONITOR(EPS_AUDIOSTREAMS))
			{
			if (EPS_CHANGED(currentAudioStream) || EPS_CHANGED(availableAudioStreams) || EPS_CHANGED(mpeg2PrologicStatus) ||
				 EPS_CHANGED(mpeg2LFEStatus) || EPS_CHANGED(ac3AudioCodingMode))
				eps.changed |= EPS_AUDIOSTREAMS;
			for (i=0; i<8; i++)
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
	//  Get subpicture stream attributes
	//

	if (EPS_REQUEST(EPS_SUBPICTURESTREAMS))
		{
		GNREASSERT(GetCurrentSubPictureStream(stream));
		eps.currentSubPictureStream = (WORD)stream;

		for (i=0; i<32; i++)
			{
			if (eps.availableSubPictureStreams & (1 << i))
				GetSubPictureStreamAttributes((WORD)i, eps.subPictureStream[i]);
			}

		eps.valid |= EPS_SUBPICTURESTREAMS;
		if (EPS_MONITOR(EPS_SUBPICTURESTREAMS))
			{
			if (EPS_CHANGED(currentSubPictureStream) || EPS_CHANGED(availableSubPictureStreams))
				eps.changed |= EPS_SUBPICTURESTREAMS;
			for (i=0; i<32; i++)
				{
				if (EPS_CHANGED(subPictureStream[i]))
					{
					eps.changed |= EPS_SUBPICTURESTREAMS;
					break;
					}
				}
			}
		}

	//
	//  Angle Information
	//

	if (EPS_REQUEST(EPS_ANGLE))
		{
		GNREASSERT(GetNumberOfAngles(loc.title, eps.numberOfAngles));
		GNREASSERT(GetCurrentAngle(eps.currentAngle));
		eps.isMultiAngleScene = IsMultiAngleScene();
		eps.playingAngle = GetPlayingAngle(eps.isMultiAngleScene);

		eps.valid |= EPS_ANGLE;
		if (EPS_MONITOR(EPS_ANGLE))
			{
			if (EPS_CHANGED(numberOfAngles) || EPS_CHANGED(currentAngle) ||
				 EPS_CHANGED(isMultiAngleScene) || EPS_CHANGED(playingAngle))
				eps.changed |= EPS_ANGLE;
			}
		}

	//
	//  Parental Information
	//

	if (EPS_REQUEST(EPS_PARENTAL))
		{
		GNREASSERT(GetSPRM(12, eps.parentalCountry));
		GNREASSERT(GetSPRM(13, eps.parentalLevel));

		eps.valid |= EPS_PARENTAL;
		if (EPS_MONITOR(EPS_PARENTAL))
			{
			if (EPS_CHANGED(parentalCountry) || EPS_CHANGED(parentalLevel))
				eps.changed |= EPS_PARENTAL;
			}
		}

	//
	//  Button Information
	//

	if (EPS_REQUEST(EPS_BUTTON))
		{
		GetButtonGeneralInfo(eps.numberOfButtons, eps.selectedButton, eps.forcedlyActivatedButton,
									eps.userButtonOffset, eps.numberOfUserButtons);

		for (i=0; i<36; i++)
			GetButtonInfo(i, eps.buttonInfo[i].x, eps.buttonInfo[i].y, eps.buttonInfo[i].w, eps.buttonInfo[i].h,
								  eps.buttonInfo[i].upperButton, eps.buttonInfo[i].lowerButton,
								  eps.buttonInfo[i].leftButton, eps.buttonInfo[i].rightButton,
								  eps.buttonInfo[i].autoAction);

		eps.valid |= EPS_BUTTON;
		if (EPS_MONITOR(EPS_BUTTON))
			{
			if (EPS_CHANGED(numberOfButtons) || EPS_CHANGED(selectedButton) || EPS_CHANGED(forcedlyActivatedButton) ||
				 EPS_CHANGED(userButtonOffset) || EPS_CHANGED(numberOfUserButtons))
				eps.changed |= EPS_BUTTON;

			for (i=0; i<36; i++)
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
	//  Set video stream info
	//

	if (EPS_REQUEST(EPS_VIDEOSTREAM))
		{
		GetVideoStreamAttributes(eps.videoStream);
		eps.valid |= EPS_VIDEOSTREAM;

		if (EPS_MONITOR(EPS_VIDEOSTREAM))
			{
			if (EPS_CHANGED(videoStream))
				 eps.changed |= EPS_VIDEOSTREAM;
			}
		}

	//
	//  Set common data
	//

	return CDDiskPlayerClass::GetExtendedPlayerState(appEPS);
	}

//
//  CallMenu
//

Error DVDDiskPlayerClass::CallMenu(VTSMenuType menu)
	{
	WORD menuID;
	WORD domainID;
	BOOL avail;
	Error error = GNR_OK;

	GNREASSERT(MenuAvail(menu, avail));
	if (avail)
		{
		if (GetDomain() == STOP_DOM) ClearGPRMs();

		switch (menu)
			{
			case VMT_TITLE_MENU:
				domainID = 1;
				menuID = 1;
				break;
			case VMT_ROOT_MENU:
				domainID = 2;
				menuID = 3;
				break;
			case VMT_SUBPICTURE_MENU:
				domainID = 2;
				menuID = 4;
				break;
			case VMT_AUDIO_MENU:
				domainID = 2;
				menuID = 5;
				break;
			case VMT_ANGLE_MENU:
				domainID = 2;
				menuID = 6;
				break;
			case VMT_PTT_MENU:
				domainID = 2;
				menuID = 7;
				break;
			default:
				GNRAISE_OK;
			}

		if (GetDomain() == TT_DOM)
			error = CallSystemSpaceProgramChain(0, domainID, menuID, 0);
		else
			error = GoSystemSpaceProgramChain(0, domainID, menuID);

		if (error == GNR_PARENTAL_LEVEL_TOO_LOW)
			Exit(error);
		GNRAISE(error);
		}
	else
		GNRAISE(GNR_OPERATION_PROHIBITED);

	}


Error DVDDiskPlayerClass::StartPresentation(DWORD flags)
	{
	return DVDDiskSequencer::StartPresentation(flags);
	}

Error DVDDiskPlayerClass::GetCurrentLocation(DVDLocation & location)
	{
	return DVDDiskSequencer::GetCurrentLocation(location);
	}

Error DVDDiskPlayerClass::GetCurrentDuration(DVDLocation & location)
	{
	return DVDDiskSequencer::GetCurrentDuration(location);
	}

Error DVDDiskPlayerClass::GetTitleDuration(WORD title, DVDTime & duration)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error DVDDiskPlayerClass::MenuAvail(VTSMenuType menu, BOOL & avail)
	{
	return DVDDiskSequencer::MenuAvail(menu, avail);
	}

Error DVDDiskPlayerClass::GetUOPs(DWORD & uops)
	{
	DWORD huops;

	GNREASSERT(CDDiskPlayerClass::GetUOPs(uops));
	GNREASSERT(DVDDiskSequencer::GetUOPs(huops));
	uops |= huops;
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::UOPPermitted(int uops, BOOL & permitted)
	{
	return DVDDiskSequencer::UOPPermitted(uops, permitted);
	}

Error DVDDiskPlayerClass::GetTitleSearchUOPs(WORD title, DWORD & uops)
	{
	return DVDDiskSequencer::GetTitleSearchUOPs(title, uops);
	}

Error DVDDiskPlayerClass::TitleSearchUOPPermitted(WORD title, int uops, BOOL & permitted)
	{
	return DVDDiskSequencer::TitleSearchUOPPermitted(title, uops, permitted);
	}

Error DVDDiskPlayerClass::NumberOfTitles(WORD & num)
	{
	return srpt->GetNumberOfTitles(num);
	}

Error DVDDiskPlayerClass::NumberOfPartOfTitle(WORD title, WORD & num)
	{
	return srpt->GetNumberOfPartOfTitle(title, num);
	}

Error DVDDiskPlayerClass::GetAvailStreams(BYTE & audio, DWORD & subPicture)
	{
	return DVDDiskSequencer::GetAvailStreams(audio, subPicture);
	}

Error DVDDiskPlayerClass::GetCurrentAudioStream(WORD & stream)
	{
	return GetSPRM(1, stream);
	}

Error DVDDiskPlayerClass::GetCurrentSubPictureStream(WORD & stream)
	{
	GNREASSERT(GetSPRM(2, stream));
	stream &= 0x3f;
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::IsCurrentSubPictureEnabled(BOOL & enabled)
	{
	WORD sprm;

	GNREASSERT(GetSPRM(2, sprm));
	enabled = (sprm & 0x40) != 0;
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::GetNumberOfAngles(WORD title, WORD & num)
	{
	return srpt->GetNumberOfAngles(title, num);
	}

Error DVDDiskPlayerClass::GetCurrentAngle(WORD & angle)
	{
	return GetSPRM(3, angle);
	}

Error DVDDiskPlayerClass::TitlePlay(WORD title, DWORD flags)
	{
	ClearGPRMs();
	return DVDDiskSequencer::GoTitle(title, flags);
	}

Error DVDDiskPlayerClass::PTTPlay(WORD title, WORD part)
	{
	ClearGPRMs();
	return DVDDiskSequencer::GoPartOfTitle(title, part);
	}

Error DVDDiskPlayerClass::GoTitle(WORD title, DWORD flags)
	{
	return DVDDiskSequencer::GoTitle(title, flags);
	}

Error DVDDiskPlayerClass::GoPartOfTitle(WORD title, WORD part)
	{
	return DVDDiskSequencer::GoPartOfTitle(title, part);
	}

Error DVDDiskPlayerClass::GoTrack(WORD track, WORD titleGroupNumber)
	{
	return DVDDiskSequencer::GoTrack(track, titleGroupNumber);
	}

Error DVDDiskPlayerClass::ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time)
	{
	return DVDDiskSequencer::ExtendedPlay(flags, title, ptt, time);
	}

Error DVDDiskPlayerClass::TimePlay(WORD title, DVDTime time)
	{
	ClearGPRMs();
	return DVDDiskSequencer::TimePlay(title, time);
	}

Error DVDDiskPlayerClass::GoUpProgramChain(void)
	{
	return DVDDiskSequencer::GoUpProgramChain();
	}

Error DVDDiskPlayerClass::TimeSearch(DVDTime time)
	{
	return DVDDiskSequencer::TimeSearch(time);
	}

Error DVDDiskPlayerClass::PTTSearch(WORD part)
	{
	return DVDTitleSequencer::GoPartOfTitle(part);
	}

Error DVDDiskPlayerClass::GoPrevProgram(void)
	{
	return DVDDiskSequencer::GoPrevProgram();
	}

Error DVDDiskPlayerClass::GoTopProgram(void)
	{
	return DVDDiskSequencer::GoTopProgram();
	}

Error DVDDiskPlayerClass::GoNextProgram(void)
	{
	return DVDDiskSequencer::GoNextProgram();
	}

Error DVDDiskPlayerClass::StartForwardScan(WORD speed)
	{
	return DVDDiskSequencer::StartForwardScan(speed);
	}

Error DVDDiskPlayerClass::StartBackwardScan(WORD speed)
	{
	return DVDDiskSequencer::StartBackwardScan(speed);
	}

Error DVDDiskPlayerClass::GetScanSpeed(WORD & speed)
	{
	speed = DVDDiskSequencer::GetScanSpeed();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::StartTrickplay(void)
	{
	return DVDDiskSequencer::StartTrickPlayback();
	}

Error DVDDiskPlayerClass::StopScan(void)
	{
	return DVDDiskSequencer::StopScan(FALSE);
	}

Error DVDDiskPlayerClass::StartReversePlayback(void)
	{
	return DVDDiskSequencer::StartReversePlayback();
	}

Error DVDDiskPlayerClass::ResumeFromSystemSpace(void)
	{
	return DVDDiskSequencer::ResumeFromSystemSpace();
	}

Error DVDDiskPlayerClass::ButtonUp(void)
	{
	return DVDDiskSequencer::ButtonUp();
	}

Error DVDDiskPlayerClass::ButtonDown(void)
	{
	return DVDDiskSequencer::ButtonDown();
	}

Error DVDDiskPlayerClass::ButtonLeft(void)
	{
	return DVDDiskSequencer::ButtonLeft();
	}

Error DVDDiskPlayerClass::ButtonRight(void)
	{
	return DVDDiskSequencer::ButtonRight();
	}

Error DVDDiskPlayerClass::ButtonActivate(void)
	{
	return DVDDiskSequencer::ButtonActivate();
	}

Error DVDDiskPlayerClass::ButtonSelectAt(WORD x, WORD y)
	{
	return DVDDiskSequencer::ButtonSelectAt(x, y);
	}

Error DVDDiskPlayerClass::ButtonSelectAndActivate(WORD button)
	{
	return DVDDiskSequencer::ButtonSelectAndActivate(button);
	}

Error DVDDiskPlayerClass::ButtonSelectAtAndActivate(WORD x, WORD y)
	{
	return DVDDiskSequencer::ButtonSelectAtAndActivate(x, y);
	}

Error DVDDiskPlayerClass::IsButtonAt(WORD x, WORD y, BOOL & isButton)
	{
	isButton = DVDDiskSequencer::IsButtonAt(x, y);
	GNRAISE_OK;
	}


Error DVDDiskPlayerClass::HasPositionalButtons(BOOL & hasButtons)
	{
	hasButtons = DVDDiskSequencer::HasPositionalButtons();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::MenuLanguageSelect(WORD language)
	{
	return SetSPRM(0, language);
	}

Error DVDDiskPlayerClass::GetMenuLanguage(WORD & language)
	{
	return GetSPRM(0, language);
	}

Error DVDDiskPlayerClass::AudioStreamChange(WORD stream)
	{
	return SetSPRM(1, stream);
	}

Error DVDDiskPlayerClass::SubPictureStreamChange(WORD stream, BOOL enable)
	{
	return SetSPRM(2, stream | (enable ? 0x40 : 0x00));
	}

Error DVDDiskPlayerClass::AngleChange(WORD angle)
	{
	return SetSPRM(3, angle);
	}

Error DVDDiskPlayerClass::ParentalLevelSelect(WORD level)
	{
	return DVDDiskSequencer::ParentalLevelSelect(level);
	}

Error DVDDiskPlayerClass::ParentalCountrySelect(WORD country)
	{
	return SetSPRM(12, country);
	}

Error DVDDiskPlayerClass::GetCurrentDisplayMode(DisplayPresentationMode & mode)
	{
	mode = DVDDiskSequencer::GetCurrentDisplayMode();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::SetDisplayMode(DisplayPresentationMode mode)
	{
	return DVDDiskSequencer::SetDisplayMode(mode);
	}

Error DVDDiskPlayerClass::GetCurrentVideoStandard(VideoStandard & standard)
	{
	standard = player->GetCurrentVideoStandard();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::SelectInitialLanguage(WORD audioLanguage, WORD audioExtension, WORD subPictureLanguage, WORD subPictureExtension)
	{
	SetSPRM(16, audioLanguage);
	SetSPRM(17, audioExtension);
	SetSPRM(18, subPictureLanguage);
	SetSPRM(19, subPictureExtension);

	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::TransferDiskKey(void)
	{
	return DVDDiskSequencer::TransferDiskKey();
	}

Error DVDDiskPlayerClass::GetCurrentBitrate(DWORD & bitrate)
	{
	bitrate = CurrentBitrate();
	GNRAISE_OK;
	}

Error DVDDiskPlayerClass::GetCurrentButtonState(WORD & minButton, WORD & numButtons, WORD & currentButton)
	{
	return DVDDiskSequencer::GetCurrentButtonState(minButton, numButtons, currentButton);
	}

//
//  Freeze current player state
//

Error DVDDiskPlayerClass::Freeze(DDPFreezeState * state, DWORD & size)
	{
	DVDHeaderFile * hfile;

	//
	//  Check buffer size
	//

	if (size < sizeof(DVDFreezeState))
		{
		size = sizeof(DVDFreezeState);
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	size = sizeof(DVDFreezeState);

	//
	// Build unique key
	//

	GNREASSERT(mgm->GetHeaderFile(hfile));
	GNREASSERT(hfile->GetUniqueKey(state->uniqueKey));
	state->diskType = diskType;

	//
	// Save data of base classes
	//

	DVDDiskSequencer::Freeze(((DVDFreezeState*)state)->dsFreezeState);
	GNREASSERT(GetSPRM(13, state->parentalLevel));
	GNRAISE(GetSPRM(12, state->parentalCountry));
	}

//
//  Restore player state
//

Error DVDDiskPlayerClass::Defrost(DDPFreezeState * state, DWORD & size, DWORD flags)
	{
	DVDHeaderFile * hfile;
	Error err = GNR_OK;
	BYTE uniqueKey[8];
	int i;

	//
	// Test parameters
	//

	if (!state)
		GNRAISE(GNR_INVALID_PARAMETERS);

	//
	// Test buffer size
	//

	if (size < sizeof(DVDFreezeState))
		{
		size = sizeof(DVDFreezeState);
		GNRAISE(GNR_OBJECT_INVALID);
		}
	size = sizeof(DVDFreezeState);

	//
	// Test unique key
	//

	GNREASSERT(mgm->GetHeaderFile(hfile));
	GNREASSERT(hfile->GetUniqueKey(uniqueKey));
	for (i=0; i<8; i++)
		{
		if (((DVDFreezeState*)state)->uniqueKey[i] != uniqueKey[i])
			GNRAISE(GNR_INVALID_UNIQUE_KEY);
		}

	//
	// Restore data
	//

	if (!XTBF(DDPSPF_ID_MATCH, flags))
		{
		if (IS_ERROR(err = DVDDiskSequencer::Defrost(((DVDFreezeState*)state)->dsFreezeState, flags)))
			Exit(err);
		}

	GNRAISE(err);
	}

//
//  Set Breakpoint
//

Error DVDDiskPlayerClass::SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id)
	{
	if (title == 0)
		{
		DVDLocation loc;

		GetCurrentLocation(loc);
		title = loc.title;
		}

	return DVDDiskSequencer::SetBreakpoint(title, ptt, time, flags, id);
	}

//
//  Clear Breakpoint
//

Error DVDDiskPlayerClass::ClearBreakpoint(DWORD id)
	{
	return DVDDiskSequencer::ClearBreakpoint(id);
	}

Error DVDDiskPlayerClass::SetAudioStreamSelectionPreferences(DWORD flags)
	{
	return DVDDiskSequencer::SetAudioStreamSelectionPreferences(flags);
	}
