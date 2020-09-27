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
//  CDVD Volume Class
//  Note that accesses to the drive are checked against genericDrive
//  since this might have been detached in GenericDrive
//
////////////////////////////////////////////////////////////////////

#include "CDVDVolume.h"

//
//  Constructor
//

CDVDVolume::CDVDVolume(void) : GenericVolume()
	{
	cdvdDrive = NULL;
	isEncryptedValid = FALSE;
	}

//
//  Destructor
//

CDVDVolume::~CDVDVolume(void)
	{
	if (genericDrive)
		{
		cache.Cleanup();
		}
	}

//
//  Init
//

Error CDVDVolume::Init(GenericDrive * drive, GenericProfile * profile)
	{
	VDAutoMutex mutex(&monitor);

	if (cdvdDrive)
		GNRAISE(GNR_OBJECT_IN_USE);

	GNREASSERT(GenericVolume::Init(drive, profile));
	GNREASSERT(cache.Init(drive, 64, maxBlockSize));
	this->cdvdDrive = (CDVDDrive*)drive;
	GNRAISE_OK;
	}

//
//  Get volume info
//

Error CDVDVolume::GetVolumeInfo(VolumeInfo & volumeInfo)
	{
	RequestHandle rh;
	WORD numSessions;
	WORD session;
	CDSessionType sessionType;
	CDTocEntry * toc;
	WORD tocEntryNum;
	WORD tocEntry;

	GNREASSERT(GenericVolume::GetVolumeInfo(volumeInfo));

	//
	//  Let's find out if there are audio tracks
	//

	GNREASSERT(cdvdDrive->GetNumberOfSessions(numSessions));

	for (session=1; session<=numSessions; session++)
		{
		GNREASSERT(ReadCDTOC(session, sessionType, toc, tocEntryNum, &rh));

		for (tocEntry=0; tocEntry<tocEntryNum; tocEntry++)
			{
			if (toc[tocEntry].GetTrackType() == CTT_AUDIO)
				{
				volumeInfo.attributes |= VIA_CONTAINS_AUDIO_TRACKS;
				session = numSessions;	// Terminate loop
				break;
				}
			}

		delete[] toc;
		}

	GNRAISE_OK;
	}

//
//  Lock blocks
//

Error CDVDVolume::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (GD_COMMAND(flags) != DAT_LOCK_AND_READ)
		GNRAISE(GNR_UNIMPLEMENTED);

	if (GD_FLAGS(flags) & DAF_CACHED)
		return cache.LockBlocks(block, num, blocks, flags, rh);
	else
		{
		if (genericDrive)
			GNRAISE(cdvdDrive->LockBlocks(block, num, blocks, flags, rh));
		else
			GNRAISE(GNR_NO_DRIVE);
		}
	}

//
//  Unlock blocks
//

Error CDVDVolume::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (GD_COMMAND(flags) != DAT_UNLOCK_CLEAN)
		GNRAISE(GNR_UNIMPLEMENTED);

	if (GD_FLAGS(flags) & DAF_CACHED)
		return cache.UnlockBlocks(block, num, blocks, flags, rh);
	else
		{
		if (genericDrive)
			GNRAISE(cdvdDrive->UnlockBlocks(block, num, blocks, flags, rh));
		else
			GNRAISE(GNR_NO_DRIVE);
		}
	}

//
//  Test if DVD is encrypted
//

Error CDVDVolume::DVDIsEncrypted(BOOL & enc)
	{
	VDAutoMutex mutex(&monitor);
	PhysicalDiskType type;

	if (!isEncryptedValid)
		{
		if (genericDrive)
			{
			//
			//  Check of we really have DVD. If not, then just pretend it's not encrypted.
			//

			GNREASSERT(cdvdDrive->GetDiskType(type));
			if (type == PHDT_DVD_ROM)
				GNREASSERT(cdvdDrive->DVDIsEncrypted(isEncrypted));
			else
				isEncrypted = FALSE;
			isEncryptedValid = TRUE;
			}
		else
			GNRAISE(GNR_NO_DRIVE);
		}

	enc = isEncrypted;
	GNRAISE_OK;
	}

//
//  Do authentication command
//

Error CDVDVolume::DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		{
		Error err = cdvdDrive->DoAuthenticationCommand(com, sector, key);
		// for unmarked encrypted discs
		if (com == DAC_COMPLETE_AUTHENTICATION && !IS_ERROR(err))
			isEncrypted = TRUE;
		GNRAISE(err);
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get RPC data
//

Error CDVDVolume::GetRPCData(BOOL & isRPC2, BYTE & regionCode, BYTE & availSets)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(cdvdDrive->GetRPCData(isRPC2, regionCode, availSets));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Set region code
//

Error CDVDVolume::SetRegionCode(BYTE region)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(cdvdDrive->SetRegionCode(region));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get number of sessions
//

Error CDVDVolume::GetNumberOfSessions(WORD & sessions)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(cdvdDrive->GetNumberOfSessions(sessions));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Read CD Toc
//

Error CDVDVolume::ReadCDTOC(WORD session, CDSessionType & sessionType, CDTocEntry * & toc, WORD & tocEntryNum, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(cdvdDrive->ReadCDTOC(session, sessionType, toc, tocEntryNum, rh));
	else
		GNRAISE(GNR_NO_DRIVE);
	}
