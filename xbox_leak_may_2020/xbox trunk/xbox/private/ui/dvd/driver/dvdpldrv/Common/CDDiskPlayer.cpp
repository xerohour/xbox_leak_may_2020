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
//  CDDiskPlayerClass
//
//////////////////////////////////////////////////////////////////////

#include "CDDiskPlayer.h"
#include "library/common/vddebug.h"

//
//  Constructor
//

CDDiskPlayerClass::CDDiskPlayerClass(void)
	: eventDispatcherInstance()
	, EventSender(&eventDispatcherInstance)
	, ERSBreakpointControl(&eventDispatcherInstance)
	{
	diskType = DDT_NONE;
	}

//
//  Destructor
//

CDDiskPlayerClass::~CDDiskPlayerClass(void)
	{
	}

//
//  Init
//

Error CDDiskPlayerClass::Init(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType, GenericFileSystem * gfs)
	{
	this->diskType = diskType;
	GNRAISE_OK;
	}

//
//  Cleanup
//

Error CDDiskPlayerClass::Cleanup(void)
	{
	return Exit();
	}

//
//  Get Disk Type
//

Error CDDiskPlayerClass::GetDiskType(DVDDiskType & type)
	{
	type = diskType;
	GNRAISE_OK;
	}

//
//  Check if disk is encrypted
//

Error CDDiskPlayerClass::DiskIsEncrypted(BOOL & enc)
	{
	enc = FALSE;
	GNRAISE_OK;
	}

//
//  Get Extended Player State
//

Error CDDiskPlayerClass::GetExtendedPlayerState(ExtendedPlayerState * appEPS)
	{
	int i;

	//
	//  Player info
	//

	if (EPS_REQUEST(EPS_PLAYERINFO))
		{
		GNREASSERT(GetMode(eps.playerMode));
		GNREASSERT(GetPlaybackSpeed(eps.playbackSpeed));
		GNREASSERT(GetScanSpeed(eps.scanSpeed));
		GNREASSERT(GetUOPs(eps.forbiddenUOPs));
		GNREASSERT(IsPlayingForward(eps.playingForward));

		eps.valid |= EPS_PLAYERINFO;
		if (EPS_MONITOR(EPS_PLAYERINFO))
			{
			if (EPS_CHANGED(playerMode) || EPS_CHANGED(playbackSpeed) || EPS_CHANGED(scanSpeed) ||
				 EPS_CHANGED(forbiddenUOPs) || EPS_CHANGED(playingForward))
				eps.changed |= EPS_PLAYERINFO;
			}
		}

	//
	//  Misc. Info
	//

	if (EPS_REQUEST(EPS_MISC))
		{
		GNREASSERT(GetDiskType(eps.diskType));
		GNREASSERT(GetCurrentDisplayMode(eps.displayMode));
		GNREASSERT(GetCurrentVideoStandard(eps.videoStandard));
		GNREASSERT(GetCurrentBitrate(eps.currentBitRate));
		GNREASSERT(DiskIsEncrypted(eps.diskIsEncrypted));

		eps.valid |= EPS_MISC;
		if (EPS_MONITOR(EPS_MISC))
			if (EPS_CHANGED(diskType) || EPS_CHANGED(displayMode) || EPS_CHANGED(videoStandard) ||
				 EPS_CHANGED(currentBitRate) || EPS_CHANGED(diskIsEncrypted))
				eps.changed |= EPS_MISC;
		}

	//
	//  Title Information
	//

	GetCurrentLocation(eps.location);  // Always do this for internal use

	if (EPS_REQUEST(EPS_TITLE))
		{
		GetCurrentDuration(eps.duration);

		eps.valid |= EPS_TITLE;
		if (EPS_MONITOR(EPS_TITLE))
			{
			if (NotEqual(eps.location, appEPS->location) || NotEqual(eps.duration, appEPS->duration))
				eps.changed |= EPS_TITLE;
			}
		}

	//
	//  Menu Information
	//

	if (EPS_REQUEST(EPS_MENU))
		{
		GNREASSERT(GetMenuLanguage(eps.menuLanguage));
		GNREASSERT(MenuAvail(VMT_TITLE_MENU, eps.menuAvailable[VMT_TITLE_MENU]));
		GNREASSERT(MenuAvail(VMT_ROOT_MENU, eps.menuAvailable[VMT_ROOT_MENU]));
		GNREASSERT(MenuAvail(VMT_SUBPICTURE_MENU, eps.menuAvailable[VMT_ROOT_MENU]));
		GNREASSERT(MenuAvail(VMT_AUDIO_MENU, eps.menuAvailable[VMT_AUDIO_MENU]));
		GNREASSERT(MenuAvail(VMT_ANGLE_MENU, eps.menuAvailable[VMT_ANGLE_MENU]));
		GNREASSERT(MenuAvail(VMT_PTT_MENU, eps.menuAvailable[VMT_PTT_MENU]));
		eps.menuAvailable[VMT_NO_MENU] = FALSE;

		eps.valid |= EPS_MENU;
		if (EPS_MONITOR(EPS_MENU))
			if (EPS_CHANGED(menuLanguage))
				eps.changed |= EPS_MENU;
			else
				{
				for (i=0; i<VMT_NO_MENU; i++)
					if (EPS_CHANGED(menuAvailable[i]))
						{
						eps.changed |= EPS_MENU;
						break;
						}
				}
		}

	//
	//  Copy data to application buffer
	//

	eps.size = appEPS->size;
	eps.request = appEPS->request;
	eps.monitor = appEPS->monitor;
	memcpy(appEPS, &eps, min(sizeof(ExtendedPlayerState), appEPS->size));

	//
	//  So far, so good ...
	//

	GNRAISE_OK;
	}

//
//  Get UOPs
//

Error CDDiskPlayerClass::GetUOPs(DWORD & uops)
	{
	DVDPlayerMode mode;
	BOOL avail;

	uops = 0;
	GNREASSERT(MenuAvail(VMT_TITLE_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_TITLE;
	GNREASSERT(MenuAvail(VMT_ROOT_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_ROOT;
	GNREASSERT(MenuAvail(VMT_SUBPICTURE_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_SUB_PICTURE;
	GNREASSERT(MenuAvail(VMT_AUDIO_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_AUDIO;
	GNREASSERT(MenuAvail(VMT_ANGLE_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_ANGLE;
	GNREASSERT(MenuAvail(VMT_PTT_MENU, avail));
	if (!avail)
		uops |= UOP_MENU_CALL_PTT;

	GNREASSERT(InternalGetMode(mode));
	switch (mode)
		{
		case DPM_STOPPED:
			uops |= UOP_STOP | UOP_GO_UP | UOP_TIME_PTT_SEARCH | UOP_PREV_TOP_PG_SEARCH |
					  UOP_NEXT_PG_SEARCH | UOP_FORWARD_SCAN | UOP_BACKWARD_SCAN | UOP_SCAN_OFF |
					  UOP_RESUME | UOP_BUTTON | UOP_STILL_OFF | UOP_PAUSE_ON | UOP_PAUSE_OFF;
			break;
		case DPM_STILL:
			uops |= UOP_FORWARD_SCAN | UOP_BACKWARD_SCAN | UOP_SCAN_OFF | UOP_PAUSE_ON | UOP_PAUSE_OFF;
			break;
		case DPM_PAUSED:
			uops |= UOP_BUTTON | UOP_STILL_OFF | UOP_SCAN_OFF;
			break;
		case DPM_SCANNING:
			uops |= UOP_PAUSE_OFF | UOP_STILL_OFF;
			break;
		case DPM_PLAYING:
			uops |= UOP_PAUSE_OFF | UOP_STILL_OFF | UOP_SCAN_OFF;
			break;
		case DPM_REVERSEPLAY:
			uops |= UOP_PAUSE_OFF | UOP_STILL_OFF;
			break;
		}

	GNREASSERT(HasPositionalButtons(avail));
	if (!avail)
		uops |= UOP_BUTTON;

	GNRAISE_OK;
	}

//
//  Get Title Search UOPs
//

Error CDDiskPlayerClass::GetTitleSearchUOPs (WORD title, DWORD & uops)
	{
	GNRAISE(GetUOPs(uops));
	}

//
//  Check if UOP is permitted for title search
//

Error CDDiskPlayerClass::TitleSearchUOPPermitted (WORD title, int uops, BOOL & permitted)
	{
	GNRAISE(UOPPermitted(uops, permitted));
	}

//
//	Set Playback Control for VCDs
//

Error CDDiskPlayerClass::SetPlaybackControl(BOOL playbackControl)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Get AVF File Info
//

Error CDDiskPlayerClass::GetAVFileInfo(GenericFileSystemIterator *gfsi, AVFInfo *pInfo)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Play File (same as Start Presentation, but with a Generic File System Iterator as an argument)
//

Error	CDDiskPlayerClass::PlayFile(GenericFileSystemIterator *gfsi, DWORD flags)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

Error CDDiskPlayerClass::SetAudioStreamSelectionPreferences(DWORD flags)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  You can enable/disable the GetExtendedPlayerState call with this method.
//  Normally it should be permitted. If you need to disable it then overwrite
//  this method with the parent class.
//

Error CDDiskPlayerClass::EPSPermitted(BOOL & permitted)
	{
	permitted = TRUE;

	GNRAISE_OK;
	}

