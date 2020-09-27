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
//  VCD File Classes Implemetation
//
////////////////////////////////////////////////////////////////////

#include "VCDFile.h"
#include "VCDHeader.h"
#include "Library\Common\VDDebug.h"

////////////////////////////////////////////////////////////////////
//
//  VCD File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDFile::VCDFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher)
	: GenericFile(vcdfs)
	, EventSender(pEventDispatcher)
	{
	this->baseFS = baseFS;
	file = NULL;
	}

//
//  Destructor
//

VCDFile::~VCDFile(void)
	{
	if (file)
		{
		file->Close();
		delete file;
		file = NULL;
		}
	}

//
//  Open
//

Error VCDFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericDiskItem * gdi;

	GNREASSERT(GenericFile::Open(gfsi, accessType));
	GNREASSERT(baseFS->OpenItem(gfsi, accessType, gdi));
	file = (GenericFile*)gdi;
	GNREASSERT(file->GetStartBlock(startBlock));
	GNREASSERT(file->GetSize(size));
	GNRAISE_OK;
	}

//
//  Close
//

Error VCDFile::Close(void)
	{
	if (file)
		{
		GNREASSERT(file->Close());
		delete file;
		file = NULL;
		}

	GNRAISE_OK;
	}

//
//  Get name
//

Error VCDFile::GetName(DiskItemName & name)
	{
	return file->GetName(name);
	}

//
//  Get path name
//

Error VCDFile::GetPathName(DiskItemName & name)
	{
	return file->GetPathName(name);
	}

//
//  Get size
//

Error VCDFile::GetSize(KernelInt64 & size)
	{
	return file->GetSize(size);
	}

//
//  Get iterator
//

Error VCDFile::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get copy management mode
//

Error VCDFile::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	gcmi = GCMI_COPY_FORBIDDEN;
	GNRAISE_OK;
	}

//
//  Seek block
//

Error VCDFile::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	return file->SeekBlock(block, flags, rh);
	}

//
//  Lock blocks
//

Error VCDFile::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	Error err = file->LockBlocks(block, num, blocks, (flags & ~GD_COMMAND_MASK) | DAT_LOCK_AND_READ, rh);
	if (err == GNR_FILE_READ_ERROR)
		SendEvent(DNE_READ_ERROR, 0);
	return err;
	}

//
// Unlock blocks
//

Error VCDFile::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	return file->UnlockBlocks(block, num, blocks, (flags & ~GD_COMMAND_MASK) | DAT_UNLOCK_CLEAN, rh);
	}

//
//  Read bytes
//

Error VCDFile::ReadBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh)
	{
	return file->ReadBytes(pos, num, buffer, flags, rh);
	}

//
//  Write bytes
//

Error VCDFile::WriteBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Flush
//

Error VCDFile::Flush(void)
	{
	return file->Flush();
	}

//
//  Terminate request
//

Error VCDFile::TerminateRequest(RequestHandle * rh)
	{
	return file->TerminateRequest(rh);
	}

////////////////////////////////////////////////////////////////////
//
//  VCD Header File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDHeaderFile::VCDHeaderFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher)
	: VCDFile(vcdfs, baseFS, pEventDispatcher)
	{
	}

//
//  Destructor
//

VCDHeaderFile::~VCDHeaderFile(void)
	{
	}

//
//  Open file
//

Error VCDHeaderFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GNREASSERT(VCDFile::Open(gfsi, accessType));
	useCount = 1;
	GNRAISE_OK;
	}

//
//  Build unique key
//

Error VCDHeaderFile::BuildUniqueKey(void)
	{
	VDAutoMutex mutex(&lock);
	RequestHandle rh;
	DriveBlock db;
	DWORD block, lastBlock;
	DWORD i, bytesToRead;
	Error err;

	memset(uniqueKey.key, 0, 8);
	if (size <= 0) GNRAISE_OK;

	lastBlock = ((size-1) / baseFS->GetHeaderDataSize()).ToDWORD();
	bytesToRead = size.ToDWORD() & 0xffff; //truncate because we don't wanna waste time reading too big files, anyway
	for (block=0; block <= lastBlock; block++)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &db, baseFS->GetHeaderSectorType() | DAF_CACHED, &rh)))
			{
			for (i=0; i<baseFS->GetHeaderDataSize() && bytesToRead; i++)
				{
				uniqueKey.key[0] += db.data[baseFS->GetHeaderHeaderSize() + i];
				bytesToRead--;
				uniqueKey.key[1] += uniqueKey.key[0] + 0x32;
				uniqueKey.key[2] += uniqueKey.key[1] + 0x47;
				uniqueKey.key[3] += uniqueKey.key[2] + 0x21;
				uniqueKey.key[4] += uniqueKey.key[3] + 0x46;
				uniqueKey.key[5] += uniqueKey.key[4] + 0x57;
				uniqueKey.key[6] += uniqueKey.key[5] + 0x94;
				uniqueKey.key[7] += uniqueKey.key[6] + 0x25;
				}
			}

		UnlockBlocks(block, 1, &db, baseFS->GetHeaderSectorType() | DAF_CACHED, &rh);
		GNREASSERT(err);
		}

	GNRAISE_OK;
	}

//
//  Obtain file
//

void VCDHeaderFile::Obtain(void)
	{
	useCount++;
	}

//
//  Release file
//

void VCDHeaderFile::Release(void)
	{
	if (!(--useCount))
		delete this;
	}

//
//  Read byte
//

Error VCDHeaderFile::ReadByte(DWORD position, BYTE & b, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return VCDFile::ReadByte(position, b, DAF_CACHED, rh);
	}

//
//  Read word
//

Error VCDHeaderFile::ReadWord(DWORD position, WORD & w, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return VCDFile::ReadWord(position, w, DAF_CACHED, rh);
	}

//
//  Read DWord
//

Error VCDHeaderFile::ReadDWord(DWORD position, DWORD & d, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return VCDFile::ReadDWord(position, d, DAF_CACHED, rh);
	}

//
//  Read sector address
//

Error VCDHeaderFile::ReadSectorAddress(DWORD position, DWORD & address, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);
	BYTE b[3];
	BYTE min, sec, fra;

	GNREASSERT(ReadBytes(position, 3, b, DAF_CACHED, rh));
	min = (b[0] >> 4) * 10 + (b[0] & 0x0f);
	sec = (b[1] >> 4) * 10 + (b[1] & 0x0f);
	fra = (b[2] >> 4) * 10 + (b[2] & 0x0f);
	address = (min * 60 + sec) * 75 + fra;
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  VCD Data File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDDataFile::VCDDataFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher)
	: VCDFile(vcdfs, baseFS, pEventDispatcher)
	{
	}

//
//  Destructor
//

VCDDataFile::~VCDDataFile(void)
	{
	}

//
//  Open data file
//

Error VCDDataFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	RequestHandle rh;
	DriveBlock db;
	DWORD blockSize;
	BOOL found = FALSE;
	Error err;
	DWORD blk;
	DWORD i;

	GNREASSERT(VCDFile::Open(gfsi, accessType));
	GNREASSERT(gfs->GetCurrentDriveBlockSize(blockSize));

	//
	//  Compute the number of blocks in this file
	//  Note that this is a little ugly since it is a mode 2 file with blocks
	//  larger than 2K, but as it seems the file size is computed on the base of
	//  2K blocks and therefore smaller.
	//

#define ISO_BLOCK_SIZE 2048
	numBlocks = (size / ISO_BLOCK_SIZE).ToDWORD();
//	numBlocks = (size / blockSize).ToDWORD();

	//
	//  Find a header to determine whether it's PAL or not
	//

	isPAL = FALSE;
	blk = 0;

	do
		{
		blk++;
		if (!IS_ERROR(err = LockBlocks(blk, 1, &db, DAF_NONE, &rh)))
			{
			if (db.data[18] == 0x62)
				{
				found = TRUE;
				break;
				}
			}
		UnlockBlocks(blk, 1, &db, DAF_NONE, &rh);
		GNREASSERT(err);
		}
	while (blk < 16);

	//
	//  If found find header and determine PAL or not
	//

	if (found)
		{
		i = 0;
		while (i < 2044 && (db.data[i  ] != 0x00 || db.data[i+1] != 0x00 ||
			                 db.data[i+2] != 0x01 || db.data[i+3] != 0xb3)) i++;

		if (i < 2044)
			isPAL = db.data[i+6] == 0x20;

		UnlockBlocks(blk, 1, &db, DAF_NONE, &rh);
		}

	GNRAISE_OK;
	}

//
//  Build unique key
//

Error VCDDataFile::BuildUniqueKey(void)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

////////////////////////////////////////////////////////////////////
//
//  VCD Directory Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDDirectory::VCDDirectory(VCDFileSystem * vcdfs, GenericFileSystem * baseFS) : GenericDirectory(vcdfs)
	{
	this->baseFS = baseFS;
	gd = NULL;
	}

//
//  Destructor
//

VCDDirectory::~VCDDirectory(void)
	{
	}

//
//  Open
//

Error VCDDirectory::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericDiskItem * gdi;

	GNREASSERT(baseFS->OpenItem(gfsi, accessType, gdi));
	gd = (GenericDirectory*)gdi;
	GNRAISE_OK;
	}

//
//  Close
//

Error VCDDirectory::Close(void)
	{
	if (gd)
		{
		gd->Close();
		delete gd;
		gd = NULL;
		}
	GNRAISE_OK;
	}

//
//  Get name
//

Error VCDDirectory::GetName(DiskItemName & name)
	{
	return gd->GetName(name);
	}

//
//  Get path name
//

Error VCDDirectory::GetPathName(DiskItemName & name)
	{
	return gd->GetPathName(name);
	}

//
//  Get iterator
//

Error VCDDirectory::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Flush
//

Error VCDDirectory::Flush(void)
	{
	return gd->Flush();
	}

//
//  Terminate request
//

Error VCDDirectory::TerminateRequest(RequestHandle * rh)
	{
	return gd->TerminateRequest(rh);
	}

//
//  Create item
//

Error VCDDirectory::CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi)
	{
	return gd->CreateItem(diskItemType, name, gdi);
	}

//
//  Delete item
//

Error VCDDirectory::DeleteItem(GenericFileSystemIterator * gfsi)
	{
	return gd->DeleteItem(gfsi);
	}

//
//  Return number of items
//

Error VCDDirectory::GetNumberOfItems(DWORD & num)
	{
	return gd->GetNumberOfItems(num);
	}

//
//  Find item
//

Error VCDDirectory::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	return gd->FindItem(name, type, flags, gfsi);
	}

////////////////////////////////////////////////////////////////////
//
//  VCD File System Iterator Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDIterator::VCDIterator(VCDFileSystem * vcdfs, GenericFileSystemIterator * gfsi) : GenericFileSystemIterator(vcdfs)
	{
	this->gfsi = gfsi;
	}

//
//  Destructor
//

VCDIterator::~VCDIterator(void)
	{
	delete gfsi;
	}

//
//  Clone
//

Error VCDIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	VCDIterator * vcdi;

	GNREASSERT(Clone(vcdi));
	gfsi = vcdi;
	GNRAISE_OK;
	}

//
//  Clone
//

Error VCDIterator::Clone(VCDIterator * & vcdi) const
	{
	GenericFileSystemIterator * help;

	GNREASSERT(gfsi->Clone(help));
	vcdi = new VCDIterator((VCDFileSystem*)gfs, help);
	if (vcdi)
		GNRAISE_OK;
	else
		{
		delete gfsi;
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	}

//
//  Go to first item
//

Error VCDIterator::GoToFirstItem(void)
	{
	return gfsi->GoToFirstItem();
	}

//
//  Go to next item
//

Error VCDIterator::GoToNextItem(void)
	{
	return gfsi->GoToNextItem();
	}

//
//  Go to sub dir
//

Error VCDIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(this->gfsi->GoToSubDir(help));
	gfsi = new VCDIterator((VCDFileSystem*)gfs, help);

	if (gfsi)
		GNRAISE_OK;
	else
		{
		delete help;
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	}

//
//  Go to parent dir
//

Error VCDIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(this->gfsi->GoToSubDir(help));
	gfsi = new VCDIterator((VCDFileSystem*)gfs, help);

	if (gfsi)
		GNRAISE_OK;
	else
		{
		delete help;
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	}

//
//  Get path name
//

Error VCDIterator::GetPathName(DiskItemName & name)
	{
	return gfsi->GetPathName(name);
	}

//
//  Get item type
//

Error VCDIterator::GetItemType(DiskItemType & type)
	{
	return gfsi->GetItemType(type);
	}

//
//  Get item name
//

Error VCDIterator::GetItemName(DiskItemName & name)
	{
	return gfsi->GetItemName(name);
	}

//
//  Get item size
//

Error VCDIterator::GetItemSize(KernelInt64 & size)
	{
	return gfsi->GetItemSize(size);
	}

//
//  Get number of items in dir
//

Error VCDIterator::GetNumberOfItems(DWORD & num)
	{
	return gfsi->GetNumberOfItems(num);
	}

//
//  Get current dir
//

Error VCDIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Open item
//

Error VCDIterator::OpenItem(DWORD accessType, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

////////////////////////////////////////////////////////////////////
//
//  VCD File System Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VCDFileSystem::VCDFileSystem(EventDispatcher* pEventDispatcher)
	: GenericFileSystem()
	, EventSender(pEventDispatcher)
	{
	baseFS = NULL;
	fileSystemType = FS_VCD;
	}

//
//  Destructor
//

VCDFileSystem::~VCDFileSystem(void)
	{
	if (baseFS)
		baseFS->Release();
	}

//
//  Initialize
//  (Note that volume is ignored)
//

Error VCDFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	GNREASSERT(((RootIterator*)mountPoint)->ObtainSubFileSystem(baseFS));
	GNREASSERT(baseFS->ObtainVolume(volume));
	Error err = GenericFileSystem::Init(mountPoint, volume, profile);
	volume->Release();
	GNRAISE(err);
	}

//
//  Create file
//

Error VCDFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	if (flags & FAT_HEADER)
		gf = new VCDHeaderFile(this, baseFS, GetEventDispatcher());
	else
		gf = new VCDDataFile(this, baseFS, GetEventDispatcher());

	if (gf)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error VCDFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
	gd = new VCDDirectory(this, baseFS);

	if (gd)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create iterator
//

Error VCDFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(baseFS->CreateIterator(help));

	gfsi = new VCDIterator(this, help);
	if (gfsi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Seek block
//

Error VCDFileSystem::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Lock blocks
//

Error VCDFileSystem::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Unlock blocks
//

Error VCDFileSystem::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Build unique key
//

Error VCDFileSystem::BuildUniqueKey(void)
	{
	VCDInfo * info = new VCDInfo();
	Error err = GNR_INVALID_UNIQUE_KEY;

	if (info)
		{
		if (!IS_ERROR(err = info->Initialize(this)))
			{
			err = info->BuildUniqueKey(uniqueKey.key);
			}

		delete info;
		}

	GNRAISE(err);
	}

//
//  Open item
//

Error VCDFileSystem::OpenItem(GenericFileSystemIterator * gfsi, DWORD accessType, GenericDiskItem * & gdi)
	{
	DiskItemType type;
	GenericFile * gf;
	GenericDirectory * gd;
	Error err;

	GNREASSERT(gfsi->GetItemType(type));
	if (type == DIT_FILE)
		{
		GNREASSERT(CreateFile(accessType, gf));
		if (IS_ERROR(err = gf->Open(((VCDIterator*)gfsi)->gfsi, accessType)))
			delete gf;
		gdi = gf;
		GNRAISE(err);
		}
	else if (type == DIT_DIR)
		{
		GNREASSERT(CreateDirectory(accessType, gd));
		if (IS_ERROR(err = gd->Open(((VCDIterator*)gfsi)->gfsi, accessType)))
			delete gd;
		gdi = gd;
		GNRAISE(err);
		}
	else
		GNRAISE(GNR_OBJECT_INVALID);
	}

//
//  Open item by name (this is here only because of the stupidity of C++)
//

Error VCDFileSystem::OpenItem(const DiskItemName & name, DWORD accessType, GenericDiskItem * & gdi)
	{
	return GenericFileSystem::OpenItem(name, accessType, gdi);
	}

//
//  Get volume name
//

Error VCDFileSystem::GetVolumeName(KernelString & name)
	{
	return baseFS->GetVolumeName(name);
	}

//
//  Get volume set name
//

Error VCDFileSystem::GetVolumeSetName(KernelString & name)
	{
	return baseFS->GetVolumeSetName(name);
	}

//
//  Get volume set index
//

Error VCDFileSystem::GetVolumeSetIndex(int & index)
	{
	return baseFS->GetVolumeSetIndex(index);
	}

//
//  Get volume set size
//

Error VCDFileSystem::GetVolumeSetSize(int & size)
	{
	return baseFS->GetVolumeSetSize(size);
	}

//
//  Set drive block size
//

Error VCDFileSystem::SetDriveBlockSize(DWORD size)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Get number of drive blocks
//

Error VCDFileSystem::GetNumberOfDriveBlocks(DWORD & num)
	{
	return baseFS->GetNumberOfDriveBlocks(num);
	}

//
//  Set number of drive blocks
//

Error VCDFileSystem::SetNumberOfDriveBlocks(DWORD num)
	{
	return baseFS->SetNumberOfDriveBlocks(num);
	}

//
//  Terminate request
//

Error VCDFileSystem::TerminateRequest(RequestHandle * rh)
	{
	return baseFS->TerminateRequest(rh);
	}

//
//  Flush
//

Error VCDFileSystem::Flush(void)
	{
	return baseFS->Flush();
	}

//
//  Get drive name
//

Error VCDFileSystem::GetDriveName(KernelString & name)
	{
	return baseFS->GetDriveName(name);
	}

//
//  Spin up drive
//

Error VCDFileSystem::SpinUpDrive(void)
	{
	return baseFS->SpinUpDrive();
	}

//
//  Spin down drive
//

Error VCDFileSystem::SpinDownDrive(void)
	{
	return baseFS->SpinDownDrive();
	}
