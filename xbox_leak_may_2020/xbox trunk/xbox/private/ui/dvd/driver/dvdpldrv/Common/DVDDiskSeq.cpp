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
//  DVD Disk Sequencer Implementation
//
//////////////////////////////////////////////////////////////////////

#include "DVDDiskSeq.h"
#include "Library/Common/vddebug.h"

//
//  Constructor
//

DVDDiskSequencer::DVDDiskSequencer(WinPortServer * server, UnitSet units, DVDDiskPlayerFactory * factory, EventDispatcher* pEventDispatcher)
	: DVDTitleSequencer(server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	, ERSBreakpointControl(pEventDispatcher)
	{
//#if DVD_AUDIO
//	dvdDisk = new DVDAudioDisk();
//#else
//	dvdDisk = new DVDVideoDisk();
//#endif
	factory->CreateDVDDisk(dvdDisk);
	srpt = NULL;
	vtsn = 0;
	}

//
//  Destructor
//

DVDDiskSequencer::~DVDDiskSequencer(void)
	{
	if (srpt)
		{
		delete srpt;
		srpt = NULL;
		}

	if (dvdDisk)
		{
		delete dvdDisk;
		dvdDisk = NULL;
		}
	}

//
//  Initialize
//

Error DVDDiskSequencer::Init(DVDFileSystem * dvdfs, WORD parentalCountryCode)
	{
	DVDMGMVOBS * mgmVobs;

	if (!dvdDisk)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	GNREASSERT(dvdDisk->Init(dvdfs, parentalCountryCode));
	mgm = dvdDisk->GetMGM();
	GNREASSERT(DVDTitleSequencer::Init(dvdfs));

	resumeAvail = FALSE;
	TransferDiskKey();
	GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
	vobs = mgmVobs;
	titleSet = NULL;
	menuCacheValid = FALSE;

	GNREASSERT(mgm->GetSRPT(srpt));

	GNRAISE_OK;
	}

//
//  Test if disk is encrypted
//

Error DVDDiskSequencer::DiskIsEncrypted(BOOL & isEnc)
	{
	DVDHeaderFile * headerFile;

	GNREASSERT(mgm->GetHeaderFile(headerFile));

	return headerFile->IsEncrypted(isEnc);
	}

//
//  Transfer disk key
//

Error DVDDiskSequencer::TransferDiskKey(void)
	{
	DVDHeaderFile * headerFile;
	BOOL isEnc;

	GNREASSERT(mgm->GetHeaderFile(headerFile));
	GNREASSERT(headerFile->IsEncrypted(isEnc));

	if (isEnc)
		{
		return player->TransferDiskKey(headerFile);
		}
	else
		GNRAISE_OK;
	}

//
//  Select PGCIT with selected language code
//

Error DVDDiskSequencer::SelectPGCIT(DVDPGCIUT pgciut, DVDPGCIT & pgcit)
	{
	WORD languageCode;
	WORD i, num;
	WORD lc;

	GNREASSERT(GetSPRM(0, languageCode));
	GNREASSERT(pgciut.GetNumberOfLanguageUnits(num));

	for (i=1; i<=num; i++)
		{
		GNREASSERT(pgciut.GetLanguageCode(i, lc));
		if (languageCode == lc)
			{
			return pgciut.GetPGCIT(i, pgcit);
			}
		}

	return pgciut.GetPGCIT(1, pgcit);
	}

//
//  VOBU started -> Check Valid UOP change
//

Error DVDDiskSequencer::VOBUStarted(DVDVOBU * vobu)
	{
	DWORD uops;

	GNREASSERT(GetUOPs(uops));

	if (uops != last.uops)
		{
		SendEvent(DNE_VALID_UOP_CHANGE, uops);
		last.uops = uops;
		}

	return DVDTitleSequencer::VOBUStarted(vobu);
	}

//
//  Check title/part of title change
//

Error DVDDiskSequencer::CheckTitleChange(void)
	{
	DVDLocation	loc;
	WORD			angles;
	BYTE			audio;
	DWORD			subPicture;

	GNREASSERT(DVDDiskSequencer::GetCurrentLocation(loc));
	if (loc.title != last.title)
		{
		SendEvent(DNE_TITLE_CHANGE, loc.title);

		last.title = loc.title;
		last.partOfTitle = loc.partOfTitle;

		//
		//  Check for number of angle, audio streams or subpicture streams change
		//

		GNREASSERT(srpt->GetNumberOfAngles(loc.title, angles));
		GNREASSERT(GetAvailStreams(audio, subPicture));
		if (angles != last.angles || audio != last.audioStreams || subPicture != last.subPictureStreams)
			SendEvent(DNE_STREAMS_CHANGE, 0);

		}
	else if (loc.partOfTitle != last.partOfTitle)
		{
		SendEvent(DNE_PART_OF_TITLE_CHANGE, loc.partOfTitle);
		last.partOfTitle = loc.partOfTitle;
		}

	GNRAISE_OK;
	}

//
//  PGCSequencer state has changed -> check for mode change
//

void DVDDiskSequencer::StateChanged(void)
	{
	DVDPlayerMode newMode = InternalGetMode();

	if (newMode != last.mode)
		{
		SendEvent(DNE_PLAYBACK_MODE_CHANGE, newMode);
		last.mode = newMode;
		}
	}

//
//  Check breakpoints
//

BOOL DVDDiskSequencer::BreakpointReached(DVDTime lowerLimit, DVDTime upperLimit, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	DVDLocation loc;

	if (!IS_ERROR(DVDDiskSequencer::GetCurrentLocation(loc)))
		return ERSBreakpointControl::BreakpointReached(loc.title, lowerLimit, upperLimit, dir, bp);
	else
		return FALSE;
	}

//
//  Check breakpoints, but do not activate
//  Start:	Lower limit of search interval (in milliseconds)
//  Length:	Length interval (in milliseconds)
//  Lowest:	If TRUE find breakpoint with lowest time, else find highest
//

BOOL DVDDiskSequencer::BreakpointContained(int start, int length, BOOL lowest, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	DVDLocation loc;
	int frameRate;

	if (!IS_ERROR(DVDDiskSequencer::GetCurrentLocation(loc)))
		{
		frameRate = loc.titleTime.FrameRate();
		return ERSBreakpointControl::BreakpointContained(loc.title, DVDTime(start, 1, frameRate),
																		 DVDTime(start + length, 1, frameRate), lowest, dir, bp);
		}
	else
		return FALSE;
	}

//
//  Check EndOfTitle breakpoints
//

BOOL DVDDiskSequencer::EndOfPGCBreakpointReached(ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	DVDLocation loc;

	if (!IS_ERROR(DVDDiskSequencer::GetCurrentLocation(loc)))
		return ERSBreakpointControl::EndOfTitleBreakpointReached(loc.title, dir, bp);
	else
		return FALSE;
	}

//
//  Test on End-Of-Program Breakpoint
//

BOOL DVDDiskSequencer::EndOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	WORD pttn;
	WORD vtsTitle;
	WORD title;

	if (!IS_ERROR(pgcit.GetTitle(pgcn, vtsTitle)))
		{
		if (!IS_ERROR(ptt.GetPartAndTitle(pgcn, pgn, vtsTitle, pttn)))
			{
			if (!IS_ERROR(srpt->GetTitleOfTSTitle(vtsn, vtsTitle, title)))
				{
				return ERSBreakpointControl::EndOfPTTBreakpointReached(title, pttn, dir, bp);
				}
			}
		}

	return FALSE;
	}

//
//  Test on Begin-Of-Program Breakpoint
//

BOOL DVDDiskSequencer::BeginOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp)
	{
	WORD pttn;
	WORD vtsTitle;
	WORD title;

	if (!IS_ERROR(pgcit.GetTitle(pgcn, vtsTitle)))
		{
		if (!IS_ERROR(ptt.GetPartAndTitle(pgcn, pgn, vtsTitle, pttn)))
			{
			if (!IS_ERROR(srpt->GetTitleOfTSTitle(vtsn, vtsTitle, title)))
				{
				//DP("PGCN %d, PGN %d, TTN %d PTTN %d", pgcn, pgn, title, pttn);
				return ERSBreakpointControl::BeginOfPTTBreakpointReached(title, pttn, dir, bp);
				}
			}
		}

	return FALSE;
	}

//
//  Get current playback location
//

Error DVDDiskSequencer::GetCurrentLocation(DVDLocation & loc)
	{
	GNREASSERT(DVDTitleSequencer::GetCurrentLocation(loc));

	loc.videoTitleSet = vtsn;

	if (GetDomain() == TT_DOM)
		GNREASSERT(srpt->GetTitleOfTSTitle(loc.videoTitleSet, loc.vtsTitle, loc.title));
	else
		loc.title = 0;

	loc.titleTime = loc.pgcTime;

	GNRAISE_OK;
	}

//
//  Get current duration
//

Error DVDDiskSequencer::GetCurrentDuration(DVDLocation & loc)
	{
	GNREASSERT(DVDTitleSequencer::GetCurrentDuration(loc));
	GNREASSERT(mgm->GetNumberOfTS(loc.videoTitleSet));

	if (GetDomain() == TT_DOM)
		GNREASSERT(srpt->GetNumberOfTitles(loc.title));
	else
		loc.title = 0;

	loc.titleTime = loc.pgcTime;

	GNRAISE_OK;
	}

//
//  Test if menu is available
//

Error DVDDiskSequencer::MenuAvail(VTSMenuType menuType, BOOL & avail)
	{
	DVDPGCIUT mpgciut;
	DVDPGCIT pgcit;
	BOOL hasMPGCIUT;
	WORD menu;
	VTSMenuType	mt;

	if (!menuCacheValid)
		{
		for(mt = VMT_TITLE_MENU; mt < VMT_NO_MENU; mt = (VTSMenuType)(mt + 1))
			{
			menuAvail[mt] = FALSE;

			if (mt == VMT_TITLE_MENU)
				{
				GNREASSERT(mgm->HasMPGCIUT(hasMPGCIUT));
				if (hasMPGCIUT)
					{
					GNREASSERT(mgm->GetMPGCIUT(mpgciut));
					GNREASSERT(SelectPGCIT(mpgciut, pgcit));
					GNREASSERT(pgcit.FindMenu(mt, menu));
					menuAvail[mt] = menu != 0;
					}
				}
			else if (titleSet || vtsn)
				{
				if (!titleSet)
					GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));

				if (titleSet)
					{
					GNREASSERT(titleSet->HasMPGCIUT(hasMPGCIUT));
					if (hasMPGCIUT)
						{
						GNREASSERT(titleSet->GetMPGCIUT(mpgciut));
						GNREASSERT(SelectPGCIT(mpgciut, pgcit));
						GNREASSERT(pgcit.FindMenu(mt, menu));
						menuAvail[mt] = menu != 0;
						}
					}
				}
			}

		menuCacheValid = TRUE;
		}

	avail = menuAvail[menuType];

	GNRAISE_OK;
	}

//
//  Start presentation
//

Error DVDDiskSequencer::StartPresentation(DWORD flags)
	{
	DVDMGMVOBS	*	mgmVobs;
	DVDPGCIUT		mpgciut;
	DVDPGCIT			pgcit;
	DVDPTT			ptt;
	Error				err;
	BOOL				has;
	WORD				num;

	GNREASSERT(CloseTitleSet());

	SetDomain(STOP_DOM);

	GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
	vobs = mgmVobs;

	navpu->ClearGPRMs();

	SetSPRM(1, 15);
	GNREASSERT(GetSPRM(2, num));
	SetSPRM(2, num & 0x40 | 62);
	SetSPRM(3, 1);
	SetSPRM(8, 1);
	SetSPRM(9, 0);
	SetSPRM(10, 0);

	GNREASSERT(mgm->HasMPGCIUT(has));
	if (has)
		{
		GNREASSERT(mgm->GetMPGCIUT(mpgciut));
		GNREASSERT(SelectPGCIT(mpgciut, pgcit));
		}

	//
	// DVD-Audio may not contain any vobs
	//

	if (vobs)
		OpenTitleSet(vobs, pgcit, ptt);

	GNREASSERT(mgm->HasFirstPlayPGC(has));
	if (has && vobs)
		{
		DVDGenericPGCI * pgci;
		DP("It has first play PGC");

		SetDomain(FP_DOM);

#if DVD_AUDIO
		GNREASSERT(pgcit.GetPGCI(1, pgci));	// get PGCI start address for DVD-Audio
#else
		GNREASSERT(mgm->GetFirstPlayPGCI(pgci));
#endif
		if (IS_ERROR(err = PlayPGCI(pgci, (flags & DDPSPF_TOPAUSE) != 0)))
			Exit(err);
		GNRAISE(err);
		}
	else
		{
		DP("No first play PGC or no Manager Menu");

		GNREASSERT(srpt->GetNumberOfTitles(num));
		if (num == 1)
			{
			return GoTitle(1, flags);
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	}

//
//  Stop presentation
//

Error DVDDiskSequencer::Exit(Error err)
	{
	GNREASSERT(DVDTitleSequencer::Exit());
	resumeAvail = FALSE;
   titleSet = NULL;
	menuCacheValid = FALSE;

	SetDomain(STOP_DOM);
	SendEvent(DNE_PLAYBACK_MODE_CHANGE, DPM_STOPPED);

	GNRAISE_OK;
	}

//
//  Go to a title
//

Error DVDDiskSequencer::GoTitle(WORD title, DWORD flags)
	{
	DVDPGCIT pgcit;
	DVDPTT ptt;
	WORD vttn;
	Error err;

	GNREASSERT(CloseTitleSet());

	resumeAvail = FALSE;

	GNREASSERT(srpt->GetTitle(title, vtsn, vttn));

	GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
	menuCacheValid = FALSE;
	vobs = titleSet->GetTSOBS();

	SetDomain(TT_DOM);

	GNREASSERT(titleSet->GetPGCIT(pgcit));
	GNREASSERT(titleSet->GetPTT(ptt));
	if (!IS_ERROR(err = OpenTitleSet(vobs, pgcit, ptt)))
		{
	 	if (!IS_ERROR(err = GoVideoTitleSetTitle(vttn, (flags & DDPSPF_TOPAUSE) != 0)))
			{
			GNRAISE_OK;
			}
		}

	SetDomain(STOP_DOM);

	GNRAISE(err);
	}

//
// Go to a Track (DVD-Audio only)
//

Error DVDDiskSequencer::GoTrack(WORD track, WORD titleGroupNumber)
	{
	DVDPGCIT	pgcit;
	DVDPTT	ptt;
	WORD		atkn;
	Error		err;

	GNREASSERT(CloseTitleSet());

	resumeAvail = FALSE;

	GNREASSERT(srpt->GetTitle(track, vtsn, atkn));

	GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
	menuCacheValid = FALSE;
	vobs = titleSet->GetTSOBS();

	SetDomain(TT_GR_DOM);

	GNREASSERT(titleSet->GetPGCIT(pgcit));
	if (!IS_ERROR(err = OpenTitleSet(vobs, pgcit, ptt)))
		{
		if (!IS_ERROR(err = GoAudioTitleSetTrack(atkn)))
			{
			GNRAISE_OK;
			}
		}

	SetDomain(STOP_DOM);

	GNRAISE_OK;
	}

//
//  Start time play
//

Error DVDDiskSequencer::TimePlay(WORD title, DVDTime time)
	{
	DVDPGCIT pgcit;
	DVDPTT ptt;
	WORD vttn;
	Error err;
	BOOL restricted;

	//
	//  Make sure the position requested allows TimePlay (because of AccessRestricted flag)
	//

	GNREASSERT(AccessRestricted(title, time, restricted));
	if (restricted)
		{
//		DPF("Time play forbidden");
		GNRAISE(GNR_OPERATION_PROHIBITED);
		}

//	DPF("Time Play allowed");

	//
	//  Op's a go, let's do it
	//

	resumeAvail = FALSE;

	GNREASSERT(CloseTitleSet());

	GNREASSERT(srpt->GetTitle(title, vtsn, vttn));

	GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
	menuCacheValid = FALSE;
	vobs = titleSet->GetVTSVOBS();

	GNREASSERT(titleSet->GetPGCIT(pgcit));
	GNREASSERT(titleSet->GetPTT(ptt));

	SetDomain(TT_DOM);

	if (!IS_ERROR(err = OpenTitleSet(vobs, pgcit, ptt)))
		{
		if (!IS_ERROR(err = DVDTitleSequencer::TimePlay(vttn, time)))
			GNRAISE_OK;
		}

	SetDomain(STOP_DOM);

	GNRAISE(err);
	}

//
//  Check if access to a position is restricted
//

Error DVDDiskSequencer::AccessRestricted(WORD title, DVDTime time, BOOL & restricted)
	{
	WORD vttn;
	WORD vtsn;
	DVDTS * ts;
	Error err;
	DVDPGCIT pgcit;
	DVDPTT ptt;
	BOOL deleteIt;

	GNREASSERT(srpt->GetTitle(title, vtsn, vttn));

	GNREASSERT(dvdDisk->GetTSExclusive(vtsn, ts, deleteIt));

	vobs = ts->GetVTSVOBS();

	GNREASSERT(ts->GetPGCIT(pgcit));
	GNREASSERT(ts->GetPTT(ptt));

	err = DVDTitleSequencer::AccessRestricted(vobs, pgcit, ptt, vttn, time, restricted);
	if (deleteIt)
		delete ts;
	GNRAISE(err);
	}

//
//  Go to part of title
//

Error DVDDiskSequencer::GoPartOfTitle(WORD title, WORD ptt)
	{
	WORD vttn;
	Error err;
	DVDPGCIT pgcit;
	DVDPTT dvdPtt;

	GNREASSERT(CloseTitleSet());

	resumeAvail = FALSE;

	GNREASSERT(srpt->GetTitle(title, vtsn, vttn));

	GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
	menuCacheValid = FALSE;
	vobs = titleSet->GetVTSVOBS();

	GNREASSERT(titleSet->GetPGCIT(pgcit));
	GNREASSERT(titleSet->GetPTT(dvdPtt));

	SetDomain(TT_DOM);

	if (!IS_ERROR(err = OpenTitleSet(vobs, pgcit, dvdPtt)))
		{
		if (!IS_ERROR(err = GoVideoTitleSetPartOfTitle(vttn, ptt)))
			{
			GNRAISE_OK;
			}
		}

	SetDomain(STOP_DOM);

	GNRAISE(err);
	}

//
//  Extended play
//

Error DVDDiskSequencer::ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time)
	{
	WORD vttn;
	Error err;

	if (flags & DDPEPF_USE_TITLE)
		{
		DVDPGCIT pgcit;
		DVDPTT dvdPTT;

		navpu->ClearGPRMs();

		GNREASSERT(CloseTitleSet());

		resumeAvail = FALSE;

		GNREASSERT(srpt->GetTitle(title, vtsn, vttn));

		GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
		menuCacheValid = FALSE;
		vobs = titleSet->GetVTSVOBS();

		GNREASSERT(titleSet->GetPGCIT(pgcit));
		GNREASSERT(titleSet->GetPTT(dvdPTT));
		SetDomain(TT_DOM);

		if (!IS_ERROR(err = OpenTitleSet(vobs, pgcit, dvdPTT)))
			{
			if (!IS_ERROR(err = DVDTitleSequencer::ExtendedPlay(flags, vttn, ptt, time)))
				{
				GNRAISE_OK;
				}
			}

		SetDomain(STOP_DOM);

		GNRAISE(err);
		}
	else
		return DVDTitleSequencer::ExtendedPlay(flags, title, ptt, time);
	}

//
//  Go to video title set title
//

Error DVDDiskSequencer::GoVideoTitleSetTitle(WORD title, BOOL toPause)
	{
	if (GetDomain() != TT_DOM)
		{
		DVDPGCIT pgcit;
		DVDPTT ptt;

		GNREASSERT(CloseTitleSet());

		resumeAvail = FALSE;

		vobs = titleSet->GetVTSVOBS();


		GNREASSERT(titleSet->GetPGCIT(pgcit));
		GNREASSERT(titleSet->GetPTT(ptt));
		SetDomain(TT_DOM);

		GNREASSERT(OpenTitleSet(vobs, pgcit, ptt));
		}

	return DVDTitleSequencer::GoVideoTitleSetTitle(title, toPause);
	}

//
//  Go to video title set part of title
//

Error DVDDiskSequencer::GoVideoTitleSetPartOfTitle(WORD title, WORD ptt)
	{
	if (GetDomain() != TT_DOM)
		{
		DVDPGCIT pgcit;
		DVDPTT ptt;

		GNREASSERT(CloseTitleSet());

		resumeAvail = FALSE;

		vobs = titleSet->GetVTSVOBS();

		GNREASSERT(titleSet->GetPGCIT(pgcit));
		GNREASSERT(titleSet->GetPTT(ptt));
		SetDomain(TT_DOM);

		GNREASSERT(OpenTitleSet(vobs, pgcit, ptt));
		}

	return DVDTitleSequencer::GoVideoTitleSetPartOfTitle(title, ptt);
	}

//
//	Go to audio title set track
//

Error DVDDiskSequencer::GoAudioTitleSetTrack(int track)
	{
	if (GetDomain() != TT_GR_DOM)
		{
		DVDPGCIT pgcit;

		GNREASSERT(CloseTitleSet());

		resumeAvail = FALSE;

		vobs = titleSet->GetVTSVOBS();


		GNREASSERT(titleSet->GetPGCIT(pgcit));
		SetDomain(TT_GR_DOM);

		GNREASSERT(OpenTitleSet(vobs, pgcit, ptt));
		}

	return DVDTitleSequencer::GoAudioTitleSetTrack(track);
	}

Error DVDDiskSequencer::InternalGoSystemSpaceProgramChain(WORD pgcn, int domainID, int menuID)
	{
	DVDManagerMenuType mgmType;
	DVDMGMVOBS * mgmVobs;
	DVDPGCIUT pgciut;
	DVDPGCIT	pgcit;
	DVDGenericPGCI * pgci;
	BOOL has;
	Error err;

	switch (domainID)
		{
		case 0:
			GNREASSERT(mgm->GetMPGCIUT(pgciut));
			GNREASSERT(SelectPGCIT(pgciut, pgcit));
			GNREASSERT(mgm->HasFirstPlayPGC(has));
			if (has)
				{
				GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
				vobs = mgmVobs;
				GNREASSERT(OpenTitleSet(vobs, pgcit, DVDPTT()));
				GNREASSERT(mgm->GetFirstPlayPGCI(pgci));
				SetDomain(FP_DOM);
				GNREASSERT(PlayPGCI(pgci));
				}
			break;
		case 1:
			GNREASSERT(mgm->GetMPGCIUT(pgciut));
			GNREASSERT(SelectPGCIT(pgciut, pgcit));
			GNREASSERT(pgcit.FindMenu(VMT_TITLE_MENU, pgcn));
			if (pgcn)
				{
				GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
				vobs = mgmVobs;
				GNREASSERT(OpenTitleSet(vobs, pgcit, DVDPTT()));
				GNREASSERT(mgm->Identify(mgmType));
				if (mgmType == DMT_AMGM)
					SetDomain(AMGM_DOM);
				else
					SetDomain(VMGM_DOM);
				GNREASSERT(GoProgramChain(pgcn));
				}
			break;
		case 2:
			if (!titleSet && vtsn)
				{
				GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));		// e.g. when in STOP state	no VTS is currently available!
				menuCacheValid = FALSE;
				}

			if (titleSet)
				{
				GNREASSERT(titleSet->GetMPGCIUT(pgciut));
				GNREASSERT(SelectPGCIT(pgciut, pgcit));

				switch(menuID)
					{
					case 3:
						GNREASSERT(pgcit.FindMenu(VMT_ROOT_MENU, pgcn));
						break;
					case 4:
						GNREASSERT(pgcit.FindMenu(VMT_SUBPICTURE_MENU, pgcn));
						break;
					case 5:
						GNREASSERT(pgcit.FindMenu(VMT_AUDIO_MENU, pgcn));
						break;
					case 6:
						GNREASSERT(pgcit.FindMenu(VMT_ANGLE_MENU, pgcn));
						break;
					case 7:
						GNREASSERT(pgcit.FindMenu(VMT_PTT_MENU, pgcn));
						break;
					}

				if (pgcn)
					{
					SetDomain(VTSM_DOM);
					vobs = titleSet->GetVTSMVOBS();

					if (IS_ERROR(err = OpenTitleSet(vobs, pgcit, DVDPTT())) ||
						 IS_ERROR(err = GoProgramChain(pgcn)))
						{
						Exit(err);

						GNRAISE(err);
						}
					}
				}
			break;
		case 3:
			GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
			vobs = mgmVobs;
			GNREASSERT(mgm->GetMPGCIUT(pgciut));
			GNREASSERT(SelectPGCIT(pgciut, pgcit));
			SetDomain(VMGM_DOM);
			GNREASSERT(OpenTitleSet(vobs, pgcit, DVDPTT()));
			GNREASSERT(GoProgramChain(pgcn));
			break;
		}

	GNRAISE_OK;
	}

Error DVDDiskSequencer::GoSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID)
	{
	if (GetDomain() == TT_DOM)
		{
		GNREASSERT(SaveResumeInformation());
		resumeValid = FALSE;
		}

	return InternalGoSystemSpaceProgramChain(pgcn, domainID, menuID);
	}

Error DVDDiskSequencer::GoSystemSpaceTitleSet(WORD ts, WORD title, WORD domainID, WORD menuID)
	{
	WORD numVTS;

	GNREASSERT(mgm->GetNumberOfTS(numVTS));

	if (ts == 0) ts = vtsn;

	if (ts > numVTS) ts = numVTS;
	else if (ts < 1) ts = 1;

	if (ts && !(titleSet && vtsn == ts))
		{
		GNREASSERT(CloseTitleSet());

		vtsn = ts;
		GNREASSERT(dvdDisk->GetTS(ts, titleSet));
		menuCacheValid = FALSE;
		}

	return InternalGoSystemSpaceProgramChain(0, domainID, menuID);
	}

Error DVDDiskSequencer::CallSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID, WORD resumeCell)
	{
	if (GetDomain() == TT_DOM)
		{
		GNREASSERT(SaveResumeInformation());
		resumeValid = TRUE;
		}

	if (resumeCell)
		{
		this->resumeCell = resumeCell;
		resumeVOBU = 0;
		}

	return InternalGoSystemSpaceProgramChain(pgcn, domainID, menuID);
	}

Error DVDDiskSequencer::GetUOPs(DWORD & uops)
	{
	GNREASSERT(DVDTitleSequencer::GetUOPs(uops));

	if (!resumeAvail || !resumeValid) uops |= UOP_RESUME;

	GNRAISE_OK;
	}

Error DVDDiskSequencer::GetTitleSearchUOPs(WORD title, DWORD & uops)
	{
	if (title)
		return srpt->GetUOPS(title, uops);
	else if (GetDomain() == TT_DOM)
		{
		DVDLocation loc;

		DVDTitleSequencer::GetCurrentLocation(loc);

		loc.videoTitleSet = vtsn;
		srpt->GetTitleOfTSTitle(loc.videoTitleSet, loc.vtsTitle, loc.title);

		return srpt->GetUOPS(loc.title, uops);
		}
	else
		{
		uops = 3;
		GNRAISE_OK;
		}
	}

Error DVDDiskSequencer::TitleSearchUOPPermitted(WORD title, int uops, BOOL & permitted)
	{
	DWORD tsuops;

	GNREASSERT(GetTitleSearchUOPs(title, tsuops));
	permitted = !XTBF(uops, tsuops);
	GNRAISE_OK;
	}

//
//  Get video attributes
//

Error DVDDiskSequencer::GetVideoStreamAttributes(VideoStreamFormat & videoStream)
	{
	if (vobs)
		return vobs->GetVideoAttributes(videoStream);
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Get System Parameter
//

Error DVDDiskSequencer::GetSPRM(int index, WORD & sprm)
	{
	WORD w;

	switch(index)
		{
		case 4:
			if (GetDomain() == TT_DOM)
				{
				//DVDVTTSRPT srpt;

				//GNREASSERT(mgm->GetSRPT(srpt));
				GNREASSERT(GetSPRM(5, w));
				return srpt->GetTitleOfTSTitle(vtsn, w, sprm);
				}
			else if (resumeAvail)
				{
				//DVDVTTSRPT srpt;

				//GNREASSERT(mgm->GetSRPT(srpt));
				GNREASSERT(GetSPRM(5, w));
				return srpt->GetTitleOfTSTitle(vtsn, w, sprm);
				}
			else
				{
				sprm = 1;
				GNRAISE_OK;
				}
			break;
		default:
			return DVDTitleSequencer::GetSPRM(index, sprm);
		}
	}

//
//  Resume from system space
//

Error DVDDiskSequencer::ResumeFromSystemSpace(void)
	{
	Error err;

	if (resumeAvail && resumeValid)
		{
		DVDPGCIT pgcit;
		DVDPTT ptt;

		GNREASSERT(CloseTitleSet());

		resumeAvail = FALSE;
		resumeValid = FALSE;

		SetDomain(TT_DOM);

		vtsn = resumeVTSN;

		GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
		menuCacheValid = FALSE;
		vobs = titleSet->GetVTSVOBS();

		SetDomain(TT_DOM);

		GNREASSERT(titleSet->GetPTT(ptt));
		GNREASSERT(titleSet->GetPGCIT(pgcit));
		if (!IS_ERROR(err = DVDTitleSequencer::ResumeFromSystemSpace(vobs, pgcit, ptt)))
			{
			GNRAISE_OK;
			}

		SetDomain(STOP_DOM);

		GNRAISE(err);
		}
	else if (!resumeAvail)
		{
		Error err = GNR_OK;
		Exit(err);
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NO_RESUME_INFORMATION);
	}

//
//  Save resume information
//

Error DVDDiskSequencer::SaveResumeInformation(void)
	{
	resumeVTSN = vtsn;

	return DVDTitleSequencer::SaveResumeInformation();
	}

//
//  Freeze current state
//

Error DVDDiskSequencer::Freeze(DVDDSFreezeState & state)
	{
	//
	// Do not freeze in FirstPlay domain
	//

	if (GetDomain() == FP_DOM)
		GNRAISE(GNR_OPERATION_PROHIBITED);

	//
	// Save data
	//

	state.vtsn = vtsn;
	state.resumeVTSN = resumeVTSN;
	state.domain = GetDomain();  // From DVDTitleSequencer

	GNREASSERT(DVDNavigationControl::Freeze(state.ncFreezeState));
	GNRAISE(DVDTitleSequencer::Freeze(state.tsFreezeState));
	}

//
//  Return to previously saved state
//

Error DVDDiskSequencer::Defrost(DVDDSFreezeState & state, DWORD flags)//, WORD parentalLevel)
	{
	DVDPGCIUT pgciut;
	DVDMGMVOBS * mgmVobs;

	//
	// Check if we eventually have FirstPlay domain (should never happen)
	//

	if (state.domain == FP_DOM)
		GNRAISE(GNR_OPERATION_PROHIBITED);

	//
	// Restore data
	//

	GNREASSERT(CloseTitleSet());

	GNREASSERT(DVDNavigationControl::Defrost(state.ncFreezeState));  // For SPRMs

	vtsn = state.vtsn;
	resumeVTSN = state.resumeVTSN;

	//
	// Restore domain, VTS and VOBS
	//

	SetDomain(state.domain);
	switch (GetDomain())  // FP_DOM can be omitted because of earlier check
		{
		case VMGM_DOM:
			GNREASSERT(mgm->GetMGMVOBS(mgmVobs));
			vobs = mgmVobs;
			GNREASSERT(mgm->GetMPGCIUT(pgciut));
			GNREASSERT(SelectPGCIT(pgciut, pgcit));
			ptt = DVDPTT();
			titleSet = NULL;
			menuCacheValid = FALSE;
			break;
		case VTSM_DOM:
			GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
			menuCacheValid = FALSE;
			vobs = titleSet->GetVTSMVOBS();
			GNREASSERT(titleSet->GetMPGCIUT(pgciut));
			GNREASSERT(SelectPGCIT(pgciut, pgcit));
			ptt = DVDPTT();
			break;
		case TT_DOM:
			GNREASSERT(dvdDisk->GetTS(vtsn, titleSet));
			menuCacheValid = FALSE;
			vobs = titleSet->GetVTSVOBS();
			GNREASSERT(titleSet->GetPGCIT(pgcit));
			GNREASSERT(titleSet->GetPTT(ptt));
			break;
		case STOP_DOM:
			titleSet = NULL;
			menuCacheValid = FALSE;
			vobs = NULL;
			break;
		}

	//
	// Defrost base class
	//

	if (GetDomain() != STOP_DOM)
		GNREASSERT(DVDTitleSequencer::Defrost(state.tsFreezeState, vobs, flags));

	//
	//  Restart playback
	//

	GNRAISE_OK;
	}

//
//  Get Playback Mode
//

DVDPlayerMode DVDDiskSequencer::InternalGetMode(void)
	{
	if (GetDomain() == STOP_DOM)
		return DPM_STOPPED;
	else if (IsStill())
		return DPM_STILL;
	else if (IsPaused())
		return DPM_PAUSED;
	else if (IsScanning())
		return DPM_SCANNING;
	else if (IsReversePlayback())
		return DPM_REVERSEPLAY;
	else if (IsTrickPlay())
		return DPM_TRICKPLAY;
	else
		return DPM_PLAYING;
	}

//
//  Set Breakpoint
//

Error DVDDiskSequencer::SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id)
	{
	BOOL iospt;

	GNREASSERT(srpt->IsOneSequentialPGCTitle(title, iospt));
	if (!iospt)
		GNRAISE(GNR_OBJECT_INVALID);

	return DVDTitleSequencer::SetBreakpoint(title, ptt, time, flags, id);
	}
