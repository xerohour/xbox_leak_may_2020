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

////////////////////////////////////////////////////////////////////
//
//  DVD Tile Sequencer Class
//
////////////////////////////////////////////////////////////////////

#include "DVDTitleSeq.h"
#include "Library/Common/vddebug.h"

//
//  Constructor
//

DVDTitleSequencer::DVDTitleSequencer (WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: DVDPGCSequencer(server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	, ERSBreakpointControl(pEventDispatcher)
	{
	pgci = NULL;
	SetDomain(STOP_DOM);
	}

//
//  Destructor
//

DVDTitleSequencer::~DVDTitleSequencer(void)
	{
	if (pgci) delete pgci;
	}

//
//  Program Chain Completed action
//

Error DVDTitleSequencer::PGCCompleted(DVDGenericPGCI * pgci)
	{
	//
	//  Go to next PGC
	//

	GNREASSERT(pgci->GetNextProgramChain(pgcn));
	if (!pgcn)
		GNREASSERT(pgci->GetUpperProgramChain(pgcn));

	delete this->pgci;
	this->pgci = NULL;

	if (pgcn == 0xffff)
		{
		return ResumeFromSystemSpace();
		}
	else if (pgcn)
		{
		GNREASSERT(pgcit.GetPGCI(pgcn, this->pgci));
		if (this->pgci)
			{
			return StartPGCPlayback(vobs, this->pgci, 1);
			}
		else
			GNRAISE(GNR_RANGE_VIOLATION);
		}
	else
		{
		GNREASSERT(Exit());
		}

	GNRAISE_OK;
	}

//
//  Open title set
//
Error DVDTitleSequencer::CloseTitleSet(void)
	{
	if (pgci)
		{
		StopPGCPlayback();
		delete pgci;
		pgci = NULL;
		}

	GNRAISE_OK;
	}

Error DVDTitleSequencer::OpenTitleSet(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt)
	{
	DVDDataFile * file = NULL;
	Error error = GNR_OK;
	WORD vatt;
	BOOL enc;

	//
	// Stop playback and create new PGCI
	//

	if (pgci)
		{
		StopPGCPlayback();
		delete pgci;
		pgci = NULL;
		}

	this->pgcit = pgcit;
	this->ptt = ptt;

	if (!(this->vobs) || vobsUniqueKey != vobs->uniqueKey)
		{
		this->vobs = vobs;
		vobsUniqueKey = vobs->uniqueKey;

		GNREASSERT(vobs->GetDataFile(file));

		if (file)
			{
			GNREASSERT(vobs->GetVideoAttributes(vatt));

			GNREASSERT(player->SetVideoStandard(vatt));

			//
			// set digital copy managment mode ...
			//

			GenericCopyManagementInfo mode;
			error = file->GetCopyManagementInfo(mode);
			if (IS_ERROR(error))
				GNREASSERT(player->SetCopyManagementMode(GCMI_COPY_FORBIDDEN));
			else
				{
				GNREASSERT(player->SetCopyManagementMode(mode));
				}

			//
			//  Workaround ????
			//  Double key transfer
			//

			GNREASSERT(file->IsEncrypted(enc));
			if (enc)
				{
				GNREASSERT(TransferDiskKey());

				error = player->TransferTitleKey(file);

				DP("Transfer of title key result 1 %08lx", error);

				if (error != GNR_OK && error != GNR_UNIMPLEMENTED)
					{
					GNREASSERT(TransferDiskKey());
					error = player->TransferTitleKey(file);

					DP("Transfer of title key result 2 %08lx", error);

					if (error != GNR_OK && error != GNR_UNIMPLEMENTED)
						{
						GNRAISE(error);
						}
					}
				}
			else
				{
				GNREASSERT(player->CancelTitleKey());
				}
			}
		}

	GNRAISE_OK;
	}

//
//  Select audio  stream
//

Error DVDTitleSequencer::SelectAudioStream(WORD & strm)
	{
	if (GetDomain() == TT_DOM)
		{
		GNREASSERT(DVDPGCSequencer::SelectAudioStream(strm));
		}
	else
		strm = 0;

	GNRAISE_OK;
	}

//
//  Select subpicture stream
//

Error DVDTitleSequencer::SelectSPUStream(WORD & strm, BOOL & enable)
	{
	if (GetDomain() == TT_DOM)
		{
		GNREASSERT(DVDPGCSequencer::SelectSPUStream(strm, enable));
		}
	else
		{
		strm = 0x00;
		enable = TRUE;
		}

	GNRAISE_OK;
	}

//
//  Get current location in stream
//

Error DVDTitleSequencer::GetCurrentLocation(DVDLocation & loc)
	{
	GNREASSERT(DVDPGCSequencer::GetCurrentLocation(loc));

	loc.domain = GetDomain();
	loc.programChain = pgcn;

	if (GetDomain() == TT_DOM)
		{
		GNREASSERT(pgcit.GetTitle(pgcn, loc.vtsTitle));
		GNREASSERT(ptt.GetPartAndTitle(loc.programChain, loc.program, loc.vtsTitle, loc.partOfTitle));
		}
	else
		{
		loc.vtsTitle = 0;
		loc.partOfTitle = 0;
		}

	GNRAISE_OK;
	}

//
//  Get duration of current playback data
//

Error DVDTitleSequencer::GetCurrentDuration(DVDLocation & loc)
	{
	WORD title;

	GNREASSERT(DVDPGCSequencer::GetCurrentDuration(loc));

	loc.domain = GetDomain();

	if (GetDomain() == STOP_DOM)
		{
		loc.programChain = 0;
		}
	else if (GetDomain() != FP_DOM)
		{
		GNREASSERT(pgcit.GetNumberOfPGC(loc.programChain));

		if (GetDomain() == TT_DOM)
			{
			GNREASSERT(ptt.GetNumberOfTitleUnits(loc.vtsTitle));
			GNREASSERT(pgcit.GetTitle(pgcn, title));
			GNREASSERT(ptt.GetNumberOfPartOfTitle(title, loc.partOfTitle));
			}
		}
	else
		loc.programChain = 1;

	GNRAISE_OK;
	}

//
//  Get the available streams
//

Error DVDTitleSequencer::GetAvailStreams(BYTE & audioStreams, DWORD & subPictureStreams)
	{
	WORD num, strm, ctrl;
	DWORD sctrl;

	if (GetDomain() == TT_DOM)
		{
		audioStreams = 0;
		subPictureStreams = 0;

		GNREASSERT(vobs->GetNumberOfAudioStreams(num));
		for(strm = 0; strm <num; strm++)
			{
			GNREASSERT(pgci->GetAudioStreamControl(strm, ctrl));
			if (XTBF(15, ctrl)) audioStreams |= MKFLAG(strm);
			}

		GNREASSERT(vobs->GetNumberOfSubPictureStreams(num));
		for(strm = 0; strm <num; strm++)
			{
			GNREASSERT(pgci->GetSubPictureStreamControl(strm, sctrl));
			if (XTBF(31, sctrl)) subPictureStreams |= MKFLAG(strm);
			}

		GNRAISE_OK;
		}
	else if (GetDomain() == STOP_DOM)
		{
		audioStreams = 0x00;
		subPictureStreams = 0x00000000;

		GNRAISE_OK;
		}
	else
		{
		audioStreams = 0x01;
		subPictureStreams = 0x00000001;

		GNRAISE_OK;
		}
	}

//
//  Go to previous program
//

Error DVDTitleSequencer::GoPrevProgram(void)
	{
	Error err;

	if (pgci)
		{
		if (IS_ERROR(err = DVDPGCSequencer::GoPrevProgram()))
			{
			if (err == GNR_RANGE_VIOLATION)
				{
				return GoPrevProgramChain();
				}
			else
				GNRAISE(err);
			}
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Check if access to a certain position (via TimePlay) is allowed
//

Error DVDTitleSequencer::AccessRestricted(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt, WORD vttn, DVDTime time, BOOL & restricted)
	{
	DVDGenericPGCI * pgci;
	Error err;
	WORD ptl;
	WORD pgcn;
	WORD pgn;
	WORD pgcitID;
	WORD ptlID;
	PGCBlockMode mode;
	PGCBlockType type;
	BOOL hasIDs;

	GNREASSERT(ptt.GetPartOfTitlePGC(vttn, 1, pgcn, pgn));

	GNREASSERT(GetSPRM(13, ptl));
	GNREASSERT(pgcit.GetParentalID(pgcn, pgcitID));
	GNREASSERT(vobs->HasParentalIDs(hasIDs));
	if (ptl != 15 &&  pgcitID && hasIDs)
		{
		GNREASSERT(vobs->GetParentalID(ptl, ptlID));

		for (;;)
			{
			GNREASSERT(pgcit.GetParentalID(pgcn, pgcitID));
			if (pgcitID & ptlID)
				break;

			GNREASSERT(pgcit.GetBlockType(pgcn, type));
			GNREASSERT(pgcit.GetBlockMode(pgcn, mode));
			if (type != PBT_PARENTAL_BLOCK || mode == PBM_LAST_PGC_IN_BLOCK)
				break;

			pgcn++;
			}
		}

	GNREASSERT(pgcit.GetPGCI(pgcn, pgci));
	err = DVDPGCSequencer::AccessRestricted(pgci, time, restricted);
	delete pgci;
	GNRAISE(err);
	}

//
//  Select program chain
//

Error DVDTitleSequencer::SelectProgramChain(void)
	{
	WORD ptl;
	WORD pgcitID;
	PGCBlockMode mode;
	PGCBlockType type;
	WORD ptlID;
	BOOL hasIDs;

	GNREASSERT(GetSPRM(13, ptl));
	GNREASSERT(pgcit.GetParentalID(pgcn, pgcitID));
	GNREASSERT(vobs->HasParentalIDs(hasIDs));
	if (ptl != 15 && pgcitID && hasIDs)
		{
		GNREASSERT(vobs->GetParentalID(ptl, ptlID));

		for(;;)
			{
			GNREASSERT(pgcit.GetParentalID(pgcn, pgcitID));
			if (pgcitID & ptlID)
				return pgcit.GetPGCI(pgcn, pgci);

			GNREASSERT(pgcit.GetBlockType(pgcn, type));
			GNREASSERT(pgcit.GetBlockMode(pgcn, mode));
			if (type != PBT_PARENTAL_BLOCK || mode == PBM_LAST_PGC_IN_BLOCK)
				GNRAISE(GNR_PARENTAL_LEVEL_TOO_LOW);

			pgcn++;
			}
		}
	else
		return pgcit.GetPGCI(pgcn, pgci);
	}

//
//  Play PGCI
//

Error DVDTitleSequencer::PlayPGCI(DVDGenericPGCI * pgci, BOOL toPause)
	{
	DP("Playing PGCI %08lx for %08lx", pgci, this->pgci);

	if (!pgci)
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);

	if (this->pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete this->pgci;
		}

	this->pgci = pgci;

	DP("Starting PGC Playback");

	return StartPGCPlayback(vobs, pgci, 1, FALSE, toPause);
	}

//
//  Go to a certain program chain
//

Error DVDTitleSequencer::GoProgramChain(WORD pgcn)
	{
	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;
		pgci = NULL;
		}

	this->pgcn = pgcn;

	GNREASSERT(SelectProgramChain());
	return StartPGCPlayback(vobs, pgci, 1);
	}

//
//  Go to part of title
//

Error DVDTitleSequencer::GoPartOfTitle(WORD pttn)
	{
	WORD pgn, title;

	if (pgci && GetDomain() == TT_DOM)
		{
		GNREASSERT(pgcit.GetTitle(pgcn, title));
		if (title)
			{
			GNREASSERT(StopPGCPlayback());
			delete pgci;

			pgci = NULL;
			GNREASSERT(ptt.GetPartOfTitlePGC(title, pttn, pgcn, pgn));

			GNREASSERT(SelectProgramChain());
			return StartPGCPlayback(vobs, pgci, pgn, TRUE);
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go to previous program chain
//

Error DVDTitleSequencer::GoPrevProgramChain(void)
	{
	WORD w;

	if (pgci)
		{
		GNREASSERT(pgci->GetPreviousProgramChain(w));
		if (w)
			{
			GNREASSERT(StopPGCPlayback());

			delete pgci;

			pgcn = w;

			GNREASSERT(SelectProgramChain());
			return StartPGCPlayback(vobs, pgci, 1, FALSE);
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go to next program chain
//

Error DVDTitleSequencer::GoNextProgramChain(void)
	{
	WORD w;

	if (pgci)
		{
		GNREASSERT(pgci->GetNextProgramChain(w));
		if (w)
			{
			GNREASSERT(StopPGCPlayback());
			delete pgci;

			pgcn = w;

			GNREASSERT(SelectProgramChain());
			return StartPGCPlayback(vobs, pgci, 1, FALSE);
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go to upper program chain
//

Error DVDTitleSequencer::GoUpProgramChain(void)
	{
	WORD w;

	if (pgci)
		{
		GNREASSERT(pgci->GetUpperProgramChain(w));
		if (w == 0xffff)
			{
			return ResumeFromSystemSpace();
			}
		else if (w)
			{
			GNREASSERT(StopPGCPlayback());
			delete pgci;

			pgcn = w;

			GNREASSERT(SelectProgramChain());
			return StartPGCPlayback(vobs, pgci, 1, FALSE);
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Go to certain title
//

Error DVDTitleSequencer::GoVideoTitleSetTitle(WORD title, BOOL toPause)
	{
	WORD pgn;

	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;

		pgci = NULL;
		}

	GNREASSERT(SetSPRM(9, 0));

	GNREASSERT(ptt.GetPartOfTitlePGC(title, 1, pgcn, pgn));

	GNREASSERT(SelectProgramChain());
	return StartPGCPlayback(vobs, pgci, 1, FALSE, toPause);
	}

//
//  Go to certain part of title
//

Error DVDTitleSequencer::GoVideoTitleSetPartOfTitle(WORD title, WORD ptt)
	{
	WORD pgn;

	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;

		pgci = NULL;
		}

	GNREASSERT(SetSPRM(9, 0));

	GNREASSERT(this->ptt.GetPartOfTitlePGC(title, ptt, pgcn, pgn));

	GNREASSERT(SelectProgramChain());
	return StartPGCPlayback(vobs, pgci, pgn, FALSE);
	}

//
//	Go to certain track (DVD-Audio only)
//

Error DVDTitleSequencer::GoAudioTitleSetTrack(int track)
	{
	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;

		pgci = NULL;
		}

	// always set Program Chain Number = 1
	// not sure if this works (MST, 12/27/00)
	pgcn = 1;

	GNREASSERT(SelectProgramChain());
	//return StartPGCPlayback(vobs, pgci, 1, TRUE, FALSE);

	GNRAISE_OK;
	}

//
//  Perform time search
//

Error DVDTitleSequencer::TimeSearch(DVDTime time)
	{
	BOOL restricted;

	if (pgci)
		{
		GNREASSERT(DVDPGCSequencer::AccessRestricted(pgci, time, restricted));
		if (restricted)
			GNRAISE(GNR_OPERATION_PROHIBITED);

		GNREASSERT(StopPGCPlayback());
		GNRAISE(StartPGCPlaybackWithTime(vobs, pgci, time, TRUE, FALSE));
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Stop playback
//

Error DVDTitleSequencer::Exit(Error err)
	{
	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;

		pgci = NULL;
		pgcn = 0;
		}

	GNRAISE_OK;
	}

//
//  Perform time play
//

Error DVDTitleSequencer::TimePlay(WORD title, DVDTime time)
	{
	WORD pgn;

	if (pgci)
		{
		GNREASSERT(StopPGCPlayback());
		delete pgci;

		pgci = NULL;
		}
	GNREASSERT(SetSPRM(9, 0));

	GNREASSERT(this->ptt.GetPartOfTitlePGC(title, 1, pgcn, pgn));

	GNREASSERT(SelectProgramChain());
	GNRAISE(StartPGCPlaybackWithTime(vobs, pgci, time));
	}

//
//  Extended Play
//

Error DVDTitleSequencer::ExtendedPlay(DWORD flags, WORD title, WORD pttn, DVDTime time)
	{
	BOOL pause;
	WORD pgn;

	pause = (flags & DDPEPF_PAUSE_AT_START) != 0;

	if (flags & DDPEPF_USE_TITLE)
		{
		if (pgci)
			{
			GNREASSERT(StopPGCPlayback());
			delete pgci;

			pgci = NULL;
			}

		GNREASSERT(SetSPRM(9, 0));

		if (flags & DDPEPF_USE_PTT)
			GNREASSERT(this->ptt.GetPartOfTitlePGC(title, pttn, pgcn, pgn));
		else
			GNREASSERT(this->ptt.GetPartOfTitlePGC(title, 1, pgcn, pgn));

		GNREASSERT(SelectProgramChain());

		if (flags & DDPEPF_USE_TIME)
			GNRAISE(StartPGCPlaybackWithTime(vobs, pgci, time, FALSE, pause));
		else
			GNRAISE(StartPGCPlayback(vobs, pgci, pgn, FALSE, pause));
		}
	else
		{
		if (pgci && GetDomain() == TT_DOM)
			{
			GNREASSERT(pgcit.GetTitle(pgcn, title));
			if (title)
				{
				GNREASSERT(StopPGCPlayback());
				delete pgci;

				pgci = NULL;
				if (flags & DDPEPF_USE_PTT)
					GNREASSERT(ptt.GetPartOfTitlePGC(title, pttn, pgcn, pgn));
				else
					GNREASSERT(ptt.GetPartOfTitlePGC(title, 1, pgcn, pgn));

				GNREASSERT(SelectProgramChain());

				if (flags & DDPEPF_USE_TIME)
					GNRAISE(StartPGCPlaybackWithTime(vobs, pgci, time, TRUE, pause));
				else
					GNRAISE(StartPGCPlayback(vobs, pgci, pgn, TRUE, pause));
				}
			else
				GNRAISE(GNR_OBJECT_NOT_FOUND);
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	}

//
//  Get system parameter
//

Error DVDTitleSequencer::GetSPRM(int index, WORD & sprm)
	{
	switch(index)
		{
		case 5:
			if (GetDomain() == TT_DOM)
				return pgcit.GetTitle(pgcn, sprm);
			else if (resumeAvail)
				sprm = resumeTitle;
			else
				sprm = 1;
			break;
		case 6:
			if (GetDomain() == TT_DOM)
				sprm = pgcn;
			else if (resumeAvail)
				sprm = resumePGCN;
			else
				sprm = 1;
			break;
		case 7:
			if (GetDomain() == TT_DOM)
				{
				WORD title;
				WORD pg;

				GNREASSERT(pgcit.GetTitle(pgcn, title));
				GNREASSERT(GetCurrentPlayingProgram(pg));
				return ptt.GetPartOfTitle(title, pgcn, pg, sprm);
				}
			else if (resumeAvail)
				sprm = resumePTT;
			else
				sprm = 1;
			break;
		default:
			 return DVDPGCSequencer::GetSPRM(index, sprm);
		}
	GNRAISE_OK;
	}

//
//  Resume to previously saved state
//

Error DVDTitleSequencer::ResumeFromSystemSpace(DVDOBS * vobs, DVDPGCIT pgcit, DVDPTT ptt)
	{
	OpenTitleSet(vobs, pgcit, ptt);

	pgcn = resumePGCN;
	GNREASSERT(ptt.GetPartOfTitle(resumeTitle, pgcn, resumePG, resumePTT));
	GNREASSERT(SelectProgramChain());
	return DVDPGCSequencer::ResumeFromSystemSpace(vobs, pgci);
	}

//
//  Save state for resume
//

Error DVDTitleSequencer::SaveResumeInformation(void)
	{
	GNREASSERT(DVDPGCSequencer::SaveResumeInformation());

	resumePGCN = pgcn;
	GNREASSERT(pgcit.GetTitle(pgcn, resumeTitle));
	GNREASSERT(pgci->GetProgramOfCell(resumeCell, resumePG));
	GNREASSERT(ptt.GetPartOfTitle(resumeTitle, pgcn, resumePG, resumePTT));
	resumeAvail = TRUE;
	resumeValid = FALSE;

	GNRAISE_OK;
	}

//
//  Freeze current state
//

Error DVDTitleSequencer::Freeze(DVDTSFreezeState & state)
	{
	//
	// Save data
	//

	state.pgcn = pgcn;

	state.resumeAvail = resumeAvail;
	state.resumeValid = resumeValid;
	state.resumePGCN = resumePGCN;
	state.resumeTitle = resumeTitle;
	state.resumePG = resumePG;

	GNRAISE(DVDPGCSequencer::Freeze(state.pgcsFreezeState));
	}

//
//  Return to previously saved state
//

Error DVDTitleSequencer::Defrost(DVDTSFreezeState & state, DVDOBS * vobs, DWORD flags)
	{
	//
	// Open title set and program chain
	//

	OpenTitleSet(vobs, pgcit, ptt);  // Initializes vobs, pgcit, ptt

	pgcn = state.pgcn;
	GNREASSERT(SelectProgramChain());  // initializes PGCI

	//
	// Restore resume data
	//

	resumeAvail = state.resumeAvail;
	resumeValid = state.resumeValid;
	resumePGCN = state.resumePGCN;
	resumeTitle = state.resumeTitle;
	resumePG = state.resumePG;

	GNRAISE(DVDPGCSequencer::Defrost(state.pgcsFreezeState, vobs, pgci, GetDomain(), flags));
	}
