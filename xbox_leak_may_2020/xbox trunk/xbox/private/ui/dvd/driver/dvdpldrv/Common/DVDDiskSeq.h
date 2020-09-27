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
//  DVD Disk Sequencer Class
//
////////////////////////////////////////////////////////////////////

#ifndef DVDDISKSEQ_H
#define DVDDISKSEQ_H

#include "DVDTitleSeq.h"
#include "DVDAVDisk.h"

#pragma warning(disable : 4250)

class DVDDiskSequencer : public DVDTitleSequencer
	{
	protected:
		DVDMGM	* mgm;
		DVDTS		* titleSet;
		DVDOBS	* vobs;
		DVDDisk	* dvdDisk;
		DVDSRPT	* srpt;

		WORD			vtsn;
		WORD			resumeVTSN;

		BOOL			menuCacheValid;
		BOOL			menuAvail[VMT_NO_MENU];

		struct								// Structure for change checks
			{
			WORD				title;			// for title change check
			WORD				partOfTitle;	// For PartOfTitle change check
			DWORD				uops;
			WORD				angles;
			WORD				audioStreams;
			DWORD				subPictureStreams;
			DVDPlayerMode	mode;
			} last;

		virtual	Error	VOBUStarted(DVDVOBU * vobu);
		virtual	void	StateChanged(void);
		virtual	Error	CheckTitleChange(void);
		virtual	BOOL	BreakpointReached(DVDTime lowerLimit, DVDTime upperLimit, ERSPlaybackDir dir, ERSBreakpoint & bp);
		virtual  BOOL	BreakpointContained(int start, int length, BOOL lowest, ERSPlaybackDir dir, ERSBreakpoint & bp);
		virtual	BOOL	EndOfPGCBreakpointReached(ERSPlaybackDir dir, ERSBreakpoint & bp);

		virtual	BOOL	EndOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp);
		virtual	BOOL	BeginOfPGBreakpointReached(WORD pgn, ERSPlaybackDir dir, ERSBreakpoint & bp);

		Error InternalGoSystemSpaceProgramChain(WORD pgcn, int domainID, int menuID);

		//
		//  State freezing stuff
		//

		class DVDDSFreezeState
			{
			public:
				WORD					vtsn;
				WORD					resumeVTSN;
				DVDDomain			domain;

				DVDNCFreezeState	ncFreezeState;
				DVDTSFreezeState	tsFreezeState;
			};

		Error Freeze (DVDDSFreezeState & state);
		Error Defrost(DVDDSFreezeState & state, DWORD flags);

	public:
		DVDDiskSequencer (WinPortServer * server, UnitSet units, DVDDiskPlayerFactory * factory, EventDispatcher* pEventDispatcher);
		virtual ~DVDDiskSequencer(void);

		Error Init(DVDFileSystem * dvdfs, WORD parentalCountryCode);

   	Error TransferDiskKey(void);

		Error SelectPGCIT(DVDPGCIUT pgciut, DVDPGCIT & pgcit);

	   Error GetCurrentLocation(DVDLocation & loc);
		Error GetCurrentDuration(DVDLocation & loc);

		Error GetSPRM(int index, WORD & sprm);

		Error MenuAvail(VTSMenuType menu, BOOL & avail);

   	Error StartPresentation(DWORD flags);

		Error Exit(Error err = GNR_OK);

   	Error GoTitle(WORD title, DWORD flags);
   	Error GoPartOfTitle(WORD title, WORD ptt);
   	Error GoVideoTitleSetTitle(WORD title, BOOL toPause = FALSE);
   	Error GoVideoTitleSetPartOfTitle(WORD title, WORD ptt);
		Error GoTrack(WORD track, WORD titleGroupNumber);
		Error GoAudioTitleSetTrack(int track);
		Error TimePlay(WORD title, DVDTime time);
		Error AccessRestricted(WORD title, DVDTime time, BOOL & restricted);

		Error ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time);

		Error GoSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID);

   	Error GoSystemSpaceTitleSet(WORD titleSet, WORD title, WORD domainID, WORD menuID);

   	Error CallSystemSpaceProgramChain(WORD pgcn, WORD domainID, WORD menuID, WORD resumeCell);

		Error ResumeFromSystemSpace(void);
		Error SaveResumeInformation(void);

		Error GetUOPs(DWORD & uops);
		Error GetTitleSearchUOPs(WORD title, DWORD & uops);
		Error TitleSearchUOPPermitted(WORD title, int uops, BOOL & permitted);
		Error GetVideoStreamAttributes(VideoStreamFormat & videoStream);

		virtual DVDPlayerMode InternalGetMode(void);
		Error DiskIsEncrypted(BOOL & enc);

		Error SetBreakpoint(WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);
	};

#pragma warning(default : 4250)

#endif
