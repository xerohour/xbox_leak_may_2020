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
//  CD File System Classes
//
////////////////////////////////////////////////////////////////////

#include "CDFile.h"
#include <stdio.h>

////////////////////////////////////////////////////////////////////
//
//  CD Track Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

CDTrack::CDTrack(CDFileSystem * cdfs) : GenericFile(cdfs)
	{
	this->cdfs = cdfs;
	cdi = NULL;
	}

//
//  Destructor
//

CDTrack::~CDTrack(void)
	{
	Close();
	}

//
//  Open track
//

Error CDTrack::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	if (accessType != FAT_READ)
		GNRAISE(GNR_FILE_READ_ONLY);

	GNREASSERT(GenericFile::Open(gfsi, accessType));
	GNREASSERT(((CDIterator*)gfsi)->Clone(cdi));
	startBlock = cdfs->GetStartBlock(cdi);
	trackType = cdfs->GetTrackType(cdi);

	GNRAISE_OK;
	}

//
//  Close track
//

Error CDTrack::Close(void)
	{
	delete cdi;
	cdi = NULL;

	GNRAISE_OK;
	}

//
//  Get name of track
//

Error CDTrack::GetName(DiskItemName & name)
	{
	return cdfs->GetItemName(cdi, name);
	}

//
//  Get path to track
//

Error CDTrack::GetPathName(DiskItemName & name)
	{
	return cdfs->GetPathName(cdi, name);
	}

//
//  Get track size
//

Error CDTrack::GetSize(KernelInt64 & size)
	{
	return cdfs->GetItemSize(cdi, size);
	}

//
//  Get iterator
//

Error CDTrack::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return cdi->Clone(gfsi);
	}

//
//  Build unique key
//

Error CDTrack::BuildUniqueKey(void)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get copy management mode
//

Error CDTrack::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	gcmi = GCMI_COPY_FORBIDDEN;	// ???
	GNRAISE_OK;
	}

//
//  Seek block
//

Error CDTrack::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(cdfs->SeekBlock(startBlock + block, flags, rh));
	}

//
//  Lock blocks
//

Error CDTrack::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	if (trackType == CTT_AUDIO)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | DST_CDDA;
	else if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | cdfs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | cdfs->GetDataSectorType();

	GNRAISE(cdfs->LockBlocks(startBlock + block, num, blocks, flags, rh));
	}

//
//  Unlock blocks
//

Error CDTrack::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	if (trackType == CTT_AUDIO)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | DST_CDDA;
	else if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | cdfs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | cdfs->GetDataSectorType();

	GNRAISE(cdfs->UnlockBlocks(startBlock + block, num, blocks, flags, rh));
	}

////////////////////////////////////////////////////////////////////
//
//  CD Session Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

//
//  Constructor
//

CDSession::CDSession(CDFileSystem * cdfs) : GenericDirectory(cdfs)
	{
	this->cdfs = cdfs;
	cdi = NULL;
	}

//
//  Copy constructor
//

CDSession::CDSession(const CDSession & cds) : GenericDirectory(cds)
	{
	cdi = new CDIterator(*(cds.cdi));
	}

//
//  Destructor
//

CDSession::~CDSession(void)
	{
	Close();
	}

//
//  Open directory
//

Error CDSession::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GNREASSERT(gfsi->Clone((GenericFileSystemIterator*&)cdi));
	GNRAISE_OK;
	}

//
//  Close directory
//

Error CDSession::Close(void)
	{
	delete cdi;
	cdi = NULL;

	GNRAISE_OK;
	}

//
//  Get name
//

Error CDSession::GetName(DiskItemName & name)
	{
	return cdfs->GetItemName(cdi, name);
	}

//
//  Get path name
//

Error CDSession::GetPathName(DiskItemName & name)
	{
	return cdfs->GetPathName(cdi, name);
	}

//
//  Get iterator pointing to this dir
//

Error CDSession::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return cdi->Clone(gfsi);
	}

//
//  Create item
//

Error CDSession::CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Delete item
//

Error CDSession::DeleteItem(GenericFileSystemIterator * gfsi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Get number of items in dir
//

Error CDSession::GetNumberOfItems(DWORD & num)
	{
	return cdfs->GetNumberOfItems(cdi, num);
	}

//
//  Find item
//

Error CDSession::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * where;
	Error err;

	GNREASSERT(GetIterator(where));
	err = cdfs->FindItem(name, type, flags, where, gfsi);
	delete where;
	GNRAISE(err);
	}

#endif

////////////////////////////////////////////////////////////////////
//
//  CD Iterator Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

CDIterator::CDIterator(CDFileSystem * cdfs) : GenericFileSystemIterator(cdfs)
	{
	this->cdfs = cdfs;
	session = 0;	// Let's point to root dir
	track = 0;
	}

//
//  Copy constructor
//

CDIterator::CDIterator(const CDIterator & cdi) : GenericFileSystemIterator(cdi)
	{
	cdfs = cdi.cdfs;
	session = cdi.session;
	track = cdi.track;
	}

//
//  Clone function
//

Error CDIterator::Clone(CDIterator * & cdi) const
	{
	cdi = new CDIterator(cdfs);
	if (cdi)
		{
		cdi->session = session;
		cdi->track = track;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Clone function
//

Error CDIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	CDIterator * cdi;

	GNREASSERT(Clone(cdi));
	gfsi = cdi;
	GNRAISE_OK;
	}

//
//  Destructor
//

CDIterator::~CDIterator(void)
	{
	}

//
//  Comparison function
//

BOOL CDIterator::Equals(GenericFileSystemIterator * gfsi)
	{
	return (session == ((CDIterator*)gfsi)->session) &&
			 (track   == ((CDIterator*)gfsi)->track);
	}

//
//  Go to first item in directory
//

Error	CDIterator::GoToFirstItem(void)
	{
	return cdfs->GoToFirstItem(this);
	}

//
//  Go to next item in directory
//

Error	CDIterator::GoToNextItem(void)
	{
	return cdfs->GoToNextItem(this);
	}

//
//  Enter subdirectory
//

Error	CDIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	return cdfs->GoToSubDir(this, gfsi);
	}

//
//  Go to parent directory
//

Error	CDIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	return cdfs->GoToParentDir(this, gfsi);
	}

//
//  Get current directory
//

Error	CDIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	return cdfs->GetDir(this, gd);
	}

//
//  Get path name of current directory
//

Error CDIterator::GetPathName(DiskItemName & name)
	{
	return cdfs->GetPathName(this, name);
	}

//
//  Get type of item pointed to
//

Error CDIterator::GetItemType(DiskItemType & type)
	{
	return cdfs->GetItemType(this, type);
	}

//
//  Get name of item pointed to
//

Error CDIterator::GetItemName(DiskItemName & name)
	{
	return cdfs->GetItemName(this, name);
	}

//
//  Get size of item pointed to
//

Error CDIterator::GetItemSize(KernelInt64 & size)
	{
	return cdfs->GetItemSize(this, size);
	}

//
//  Get the number of items in the directory the iterator is in
//

Error CDIterator::GetNumberOfItems(DWORD & num)
	{
	return cdfs->GetNumberOfItems(this, num);
	}

//
//  Open item pointed to
//

Error	CDIterator::OpenItem(DWORD fileAccessType, GenericDiskItem * & gdi)
	{
	return cdfs->OpenItem(this, fileAccessType, gdi);
	}

////////////////////////////////////////////////////////////////////
//
//  CD File System Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

CDFileSystem::CDFileSystem(void) : GenericFileSystem()
	{
	numSessions = 0;
	sessions = NULL;
	cdvdVolume = NULL;
	fileSystemType = FS_CD;
	}

//
//  Destructor
//

CDFileSystem::~CDFileSystem(void)
	{
	int i;

	//
	//  Delete TOC data
	//

	for (i=0; i<numSessions; i++)
		delete[] sessions[i].toc;

	delete[] sessions;
	}

//
//  Initialize
//

Error CDFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	RequestHandle rh;
	CDSessionType sessionType = CST_NONE;
	PhysicalDiskType type;
	Error err = GNR_OK, herr;
	int i;

	//
	//  Check if we can use this volume
	//

	GNREASSERT(volume->GetDiskType(type));

	switch (type)
		{
		case PHDT_CDDA:
			volumeName = "Audio CD";
			break;
		case PHDT_CDROM:
			volumeName = "CDROM";
			break;
		case PHDT_CDI:
			volumeName = "CDI";
			break;
		case PHDT_CDROM_XA:
			volumeName = "CDROM XA";
			break;
		default:
			GNRAISE(GNR_VOLUME_INVALID);
		}

	GNREASSERT(GenericFileSystem::Init(mountPoint, volume, profile));
	cdvdVolume = (CDVDVolume*)volume;

	//
	//  Initialize TOCs
	//

	GNREASSERT((cdvdVolume)->GetNumberOfSessions(numSessions));
	if (!numSessions)
		GNRAISE(GNR_VOLUME_INVALID);

	sessions = new CDSessionInfo[numSessions];
	for (i=0; i<numSessions; i++)
		{
		if (IS_ERROR(herr = cdvdVolume->ReadCDTOC(i+1, sessionType, sessions[i].toc, sessions[i].numTracks, &rh)))
			{
			err = herr;
			sessions[i].toc = NULL;
			sessions[i].numTracks = 0;
			}
		}

	//
	//  Initialize misc
	//

	volumeSetName = volumeName;
	volumeSetIndex = 1;
	volumeSetSize = 1;

	GNRAISE(err);
	}

//
// Initialize header- and data-related variables.
// Base class doesn't do good enough job because it bases decision about sector
// type relying on phisical disk type, which in case of audio may not be a good
// criteria (audio could be found mixed with data on CDROM and other disk types)
//
Error	CDFileSystem::InitHeaderAndData(GenericVolume * volume)
	{
	DWORD dummy;

	headerSectorType = DST_CDDA;
	dataSectorType = DST_CDDA;
	GNREASSERT(volume->GetBlockInfo(headerSectorType, dummy, headerHeaderSize, headerDataSize));
	GNREASSERT(volume->GetBlockInfo(dataSectorType, dummy, dataHeaderSize, dataDataSize));

	GNRAISE_OK;
	}


#define CDDB_UNIQUE_CD_ID 1

//
//  Build unique volume key
//

Error CDFileSystem::BuildUniqueKey(void)
	{
	int i;
	int j;
	DWORD val;

#if CDDB_UNIQUE_CD_ID

	// Unique CD key compatible with www.freedb.org.
	// To confirm the algorithma is up-to-date, check out www.freedb.org and/or www.cddb.com
	//TODO: How to handle multiple sessions? To base key on all the sessions may have more chances
	//to be unique, but it may not be very useful for CDDB.

	char szTemp[9];
	int cddbTrackSum, cddbTotalSum, totalTracks, totalCDLengthSec;
	DWORD dwDiscId;

	cddbTotalSum = 0; totalTracks = 0; totalCDLengthSec = 0;
	for (i=0; i<numSessions; i++)
		{
		int endOfCDSec;

		for (j=0; j<sessions[i].numTracks; j++)
			{
			cddbTrackSum = 0;
			val = sessions[i].toc[j].GetStartBlock()/75; //track positon time in seconds
			while (val > 0)
				{
				cddbTrackSum += (val%10);
				val /= 10;
				}
			cddbTotalSum += cddbTrackSum;
			val = sessions[i].toc[j].GetNumberOfBlocks();
			}

		totalTracks += sessions[i].numTracks;
		endOfCDSec = (sessions[i].toc[j-1].GetStartBlock() + sessions[i].toc[j-1].GetNumberOfBlocks())/75;
		totalCDLengthSec += endOfCDSec - sessions[i].toc[0].GetStartBlock()/75;
		}

	//Set unique disc ID value
	dwDiscId = ((cddbTotalSum % 0xff) << 24 | (totalCDLengthSec & 0xffff) << 8 | (totalTracks & 0xff));
	sprintf(szTemp, "%08x", dwDiscId);
	memcpy(uniqueKey.key, szTemp, 8);

#else

	// Our internal CD key
	int pos = 0;

	for (i=0; i<8; i++)
		uniqueKey.key[i] = 0x00;

	for (i=0; i<numSessions; i++)
		{
		for (j=0; j<sessions[i].numTracks; j++)
			{
			val = sessions[i].toc[j].GetStartBlock();
			uniqueKey.key[pos++] += (BYTE)XTBF( 0, 8, val);
			pos &= 0x7;
			uniqueKey.key[pos++] += (BYTE)XTBF( 8, 8, val);
			pos &= 0x7;
			uniqueKey.key[pos++] += (BYTE)XTBF(16, 8, val);
			pos &= 0x7;

			val = sessions[i].toc[j].GetNumberOfBlocks();
			uniqueKey.key[pos++] += (BYTE)XTBF( 0, 8, val);
			pos &= 0x7;
			uniqueKey.key[pos++] += (BYTE)XTBF( 8, 8, val);
			pos &= 0x7;
			uniqueKey.key[pos++] += (BYTE)XTBF(16, 8, val);
			pos &= 0x7;
			}
		}

#endif

	uniqueKey.valid = TRUE;
	GNRAISE_OK;
	}

//
//  Create iterator
//

Error CDFileSystem::CreateIterator(CDIterator * & cdi)
	{
	cdi = new CDIterator(this);
	if (cdi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create iterator
//

Error CDFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	gfsi = new CDIterator(this);
	if (gfsi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create file
//

Error CDFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	gf = new CDTrack(this);
	if (gf)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error CDFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
#if GFS_ENABLE_DIRECTORIES
	gd = new CDSession(this);
	if (gd)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
#else
	GNRAISE(GNR_UNIMPLEMENTED);
#endif
	}

//
//  Return start address of track
//

DWORD CDFileSystem::GetStartBlock(CDIterator * cdi)
	{
	if (cdi->session)
		return sessions[cdi->session - 1].toc[cdi->track].GetStartBlock();
	else
		return 0;
	}

//
//  Return the number of items in a dir
//

Error CDFileSystem::GetNumberOfItems(CDIterator * cdi, DWORD & num)
	{
	if (cdi->session)
		num = sessions[cdi->session].numTracks;
	else
		num = numSessions;
	GNRAISE_OK;
	}

//
// GetTrackType
//

CDTrackType	CDFileSystem::GetTrackType(CDIterator * cdi)
	{
	if (cdi->session)
		return sessions[cdi->session - 1].toc[cdi->track].GetTrackType();
	else
		return CTT_NONE;
	}

//
//  Go to first item in dir
//

Error CDFileSystem::GoToFirstItem(CDIterator * cdi)
	{
	cdi->track = 0;
	GNRAISE_OK;
	}

//
//  Go to next item in dir
//

Error	CDFileSystem::GoToNextItem(CDIterator * cdi)
	{
	if ((cdi->session && cdi->track == (sessions[cdi->session - 1].numTracks - 1)) ||		//  We are inside a session
		 (!cdi->session && cdi->track == numSessions - 1))												//  We are in root directory
		GNRAISE(GNR_ITEM_NOT_FOUND);

	cdi->track++;
	GNRAISE_OK;
	}

//
//  Enter directory
//

Error CDFileSystem::GoToSubDir(CDIterator * cdi, GenericFileSystemIterator * & gfsi)
	{
	if (cdi->session)
		{
		GNRAISE(GNR_ITEM_NOT_FOUND);
		}
	else
		{
		GNREASSERT(CreateIterator(gfsi));
		((CDIterator*)gfsi)->session = cdi->track + 1;
		((CDIterator*)gfsi)->track = 0;
		GNRAISE_OK;
		}
	}

//
//  Go to parent directory
//

Error	CDFileSystem::GoToParentDir(CDIterator * cdi, GenericFileSystemIterator * & gfsi)
	{
	if (cdi->session)
		return CreateIterator(gfsi);				// If we are inside session we go to root dir of file system
	else
		return mountPoint->Clone(gfsi);			// If we are in root dir we take iterator of parent
	}

//
//  Get current directory as object
//

Error	CDFileSystem::GetDir(CDIterator * cdi, GenericDirectory * & gd)
	{
	Error err;

	GNREASSERT(CreateDirectory(FAT_NONE, gd));
	if (IS_ERROR(err = gd->Open(cdi, DAT_NONE)))
		delete gd;

	GNRAISE(err);
	}

//
//  Get path name
//

Error CDFileSystem::GetPathName(CDIterator * cdi, DiskItemName & name)
	{
	if (cdi->session)
		name = DiskItemName(KernelString("/Session" + KernelString(cdi->session)));
	else
		name = DiskItemName("/");

	// go through mount point...
	if (this->mountPoint)
		{
		DiskItemName mpName;
		DiskItemName mpPath;
		this->mountPoint->GetPathName(mpPath);
		this->mountPoint->GetItemName(mpName);
		name = mpPath + (DiskItemName)"/" + mpName + name;
		}
	else
		{
		// no mount point? Should not be, but do not panic. Claim to be root...
		name = name;
		}


	GNRAISE_OK;
	}

//
//  Get type item pointed to
//

Error CDFileSystem::GetItemType(CDIterator * cdi, DiskItemType & type)
	{
	if (cdi->session)
		type = DIT_FILE;
	else
		type = DIT_DIR;

	GNRAISE_OK;
	}

//
//  Get name of item pointed to
//

Error CDFileSystem::GetItemName(CDIterator * cdi, DiskItemName & name)
	{
	if (cdi->session)
		name = (KernelString)"Track" + KernelString(cdi->track + 1);
	else
		name = (KernelString)("Session" + KernelString(cdi->track + 1) + (KernelString)name.GetDefaultSeparator());

	GNRAISE_OK;
	}

//
//  Get size of item pointed to
//

Error CDFileSystem::GetItemSize(CDIterator * cdi, KernelInt64 & size)
	{
	if (cdi->session)
		size = sessions[cdi->session - 1].toc[cdi->track].GetNumberOfBlocks() * blockSize;
	else
		size = 0;	// Directories (sessions in this case) don't have a size, as well as errors

	GNRAISE_OK;
	}

//
//  Get extended item information
//

Error CDFileSystem::GetExtItemInfo(GenericFileSystemIterator * gfsi, ExtItemInfo & info)
	{
	CDIterator * cdi = (CDIterator*)gfsi;

	if (cdi->session)
		{
		if (sessions[cdi->session - 1].toc[cdi->track].GetTrackType() == CTT_AUDIO)
			info.exType = EDIT_AUDIO_TRACK;
		else
			info.exType = EDIT_DATA_TRACK;
		}
	else
		info.exType = EDIT_NONE;

	GNRAISE_OK;
	}
