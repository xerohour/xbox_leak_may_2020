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
//  Generic File Classes, Implementation
//
////////////////////////////////////////////////////////////////////

#include "GenericFile.h"

////////////////////////////////////////////////////////////////////
//
//  Generic Disk Item Implementation
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericDiskItem::GenericDiskItem(GenericFileSystem * gfs)
	{
	this->gfs = gfs;
	//DP("From GenericDiskItem - FS O: ");
	gfs->Obtain();
	}

//
//  Destructor
//

GenericDiskItem::~GenericDiskItem(void)
	{
	//DP("From ~GenericDiskItem - FS R:");
	gfs->Release();
	}

//
//  Flush
//

Error GenericDiskItem::Flush(void)
	{
	GNRAISE_OK;
	}

//
//  Terminate request
//

Error GenericDiskItem::TerminateRequest(RequestHandle * rh)
	{
	return gfs->TerminateRequest(rh);
	}


void PathStringCache::Add(DiskItemName name, DWORD block)
	{
	StringCacheElement * node = new StringCacheElement(block, name);
	InsertFirst((Node*) node);
	if (Num() > size)
		{
		node = (StringCacheElement*) Last();
		RemoveLast();
		delete (StringCacheElement*) node;
		}
	}

Error PathStringCache::Get(DWORD block, DiskItemName & name)
	{
	IteratorHandle h(CreateIterator());
	StringCacheElement * node = NULL;

	bool found = false;
	while (!found && (node = ((StringCacheElement*) h.Proceed())))		// order of expressions is important!
		{
		if (node->block == block)
			found = true;
		}

	if (!found)
		return GNR_ITEM_NOT_FOUND;

	// reorder - move to front
	Remove(node);
	InsertFirst(node);

	name = node->name;

	return GNR_OK;
	}


////////////////////////////////////////////////////////////////////
//
//  Generic File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericFile::GenericFile(GenericFileSystem * gfs) : GenericDiskItem(gfs)
	{
	}

//
//  Destructor
//

GenericFile::~GenericFile(void)
	{
	}

//
//  Open file
//

Error GenericFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	if (accessType & FAT_HEADER)
		headerFile = TRUE;
	else
		headerFile = FALSE;

	startBlock = 0;
	GNRAISE_OK;
	}

//
//  Get unique key
//

Error GenericFile::GetUniqueKey(BYTE * key)
	{
	if (!uniqueKey.valid)
		{
		uniqueKey.valid = FALSE;
		GNREASSERT(BuildUniqueKey());
		}

	uniqueKey.valid = TRUE;
	memcpy(key, uniqueKey.key, 8);
	GNRAISE_OK;
	}

//
//  Get start block of file (logical, i.e. physical block is computed by adding logicalBaseBlock of file system)
//

Error GenericFile::GetStartBlock(DWORD & startBlock)
	{
	startBlock = this->startBlock;
	GNRAISE_OK;
	}

Error GenericFile::ReadByte(DWORD pos, BYTE & b, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db;
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / dataSize);
	offset = (pos % dataSize) + headerSize;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, 1, &db, flags | DAT_LOCK_AND_READ, rh)))
		b = db.data[offset];
	UnlockBlocks(block, 1, &db, flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read word
//

Error GenericFile::ReadWord(DWORD pos, WORD & w, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db[2];
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block, num = 1;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / dataSize);
	offset = (pos % dataSize) + headerSize;
	if (offset == dataSize - 1)	// We read over block boundary
		num = 2;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, num, &(db[0]), flags | DAT_LOCK_AND_READ, rh)))
		{
		if (offset == dataSize - 1)
			w = MAKEWORD(db[1].data[headerSize], db[0].data[dataSize - 1]);
		else
			w = MAKEWORD(db[0].data[offset + 1], db[0].data[offset]);
		}
	UnlockBlocks(block, num, &(db[0]), flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read dword
//

Error GenericFile::ReadDWord(DWORD pos, DWORD & d, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db[2];
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block, num = 1;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / dataSize);
	offset = (pos % dataSize) + headerSize;
	if (offset > dataSize - 4)	// We read over block boundary
		num = 2;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, num, &(db[0]), flags | DAT_LOCK_AND_READ, rh)))
		{
		switch (dataSize - offset)
			{
			case 1:
				d = MAKELONG4(db[1].data[headerSize + 2], db[headerSize + 1].data[1], db[1].data[headerSize], db[0].data[offset]);
				break;
			case 2:
				d = MAKELONG4(db[1].data[headerSize + 1], db[1].data[headerSize], db[0].data[offset + 1], db[0].data[offset]);
				break;
			case 3:
				d = MAKELONG4(db[1].data[headerSize], db[0].data[offset + 2], db[0].data[offset + 1], db[0].data[offset]);
				break;
			default:
				d = MAKELONG4(db[0].data[offset + 3], db[0].data[offset + 2], db[0].data[offset + 1], db[0].data[offset]);
				break;
			}
		}
	UnlockBlocks(block, num, &(db[0]), flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read byte
//

Error GenericFile::ReadByte(KernelInt64 pos, BYTE & b, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db;
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / (KernelInt64)dataSize).ToDWORD();
	offset = (pos % dataSize).ToDWORD() + headerSize;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, 1, &db, flags | DAT_LOCK_AND_READ, rh)))
		b = db.data[offset];
	UnlockBlocks(block, 1, &db, flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read word
//

Error GenericFile::ReadWord(KernelInt64 pos, WORD & w, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db[2];
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block, num = 1;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / (KernelInt64)dataSize).ToDWORD();
	offset = (pos % dataSize).ToDWORD() + headerSize;
	if (offset == dataSize - 1)	// We read over block boundary
		num = 2;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, num, &(db[0]), flags | DAT_LOCK_AND_READ, rh)))
		{
		if (offset == dataSize - 1)
			w = MAKEWORD(db[1].data[headerSize], db[0].data[dataSize - 1]);
		else
			w = MAKEWORD(db[0].data[offset + 1], db[0].data[offset]);
		}
	UnlockBlocks(block, num, &(db[0]), flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read dword
//

Error GenericFile::ReadDWord(KernelInt64 pos, DWORD & d, DWORD flags, RequestHandle * rh)
	{
	DriveBlock db[2];
	DWORD headerSize, dataSize;
	DWORD sectorType, offset;
	DWORD block, num = 1;
	Error err;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine position
	//

	block = (pos / (KernelInt64)dataSize).ToDWORD();
	offset = (pos % dataSize).ToDWORD() + headerSize;
	if (offset > dataSize - 4)	// We read over block boundary
		num = 2;
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data
	//

	if (!IS_ERROR(err = LockBlocks(block, num, &(db[0]), flags | DAT_LOCK_AND_READ, rh)))
		{
		switch (dataSize - offset)
			{
			case 1:
				d = MAKELONG4(db[1].data[headerSize + 2], db[headerSize + 1].data[1], db[1].data[headerSize], db[0].data[offset]);
				break;
			case 2:
				d = MAKELONG4(db[1].data[headerSize + 1], db[1].data[headerSize], db[0].data[offset + 1], db[0].data[offset]);
				break;
			case 3:
				d = MAKELONG4(db[1].data[headerSize], db[0].data[offset + 2], db[0].data[offset + 1], db[0].data[offset]);
				break;
			default:
				d = MAKELONG4(db[0].data[offset + 3], db[0].data[offset + 2], db[0].data[offset + 1], db[0].data[offset]);
				break;
			}
		}
	UnlockBlocks(block, num, &(db[0]), flags | DAT_UNLOCK_CLEAN, rh);
	GNRAISE(err);
	}

//
//  Read bytes
//  NOTE: ReadBytes relies on LockBlocks. Bad data might be read if the last block of one file
//  is not completely filled with data, i.e. the file size is not a multiple of the block size.
//

Error GenericFile::ReadBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);
	DriveBlock * db;
	DWORD firstBlock, lastBlock, numBlocks;
	DWORD blocksToRead, blocksAtOnce = 32;//27;		// This is because of the stupid ASPI limitation of 64K at most at once
	DWORD headerSize, dataSize;
	DWORD sectorType;
	DWORD bytes;
	Error err = GNR_OK;
	DWORD i;

	if (num == 0)
		GNRAISE_OK;

	//
	//  Determine sector type/data sizes
	//

	if (headerFile)
		{
		headerSize = gfs->GetHeaderHeaderSize();
		dataSize = gfs->GetHeaderDataSize();
		sectorType = gfs->GetHeaderSectorType();
		}
	else
		{
		headerSize = gfs->GetDataHeaderSize();
		dataSize = gfs->GetDataDataSize();
		sectorType = gfs->GetDataSectorType();
		}

	if (flags & FAT_RAW)
		{
		dataSize += headerSize;
		headerSize = 0;
		}

	//
	//  Determine number of blocks and allocate drive blocks
	//

	firstBlock = (pos / (KernelInt64)dataSize).ToDWORD();
	lastBlock = ((pos + num) / (KernelInt64)dataSize).ToDWORD();
	pos %= dataSize;
	numBlocks = lastBlock - firstBlock + 1;
	db = new DriveBlock[blocksAtOnce];
	if (!db)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	flags = (flags & ~GD_COMMAND_MASK & ~GD_SECTOR_TYPE_MASK) | sectorType;

	//
	//  Read data and copy it
	//

	while (numBlocks && !IS_ERROR(err))
		{
		blocksToRead = min(numBlocks, blocksAtOnce);
		err = LockBlocks(firstBlock, blocksToRead, db, flags | DAT_LOCK_AND_READ, rh);
		if (!IS_ERROR(err))
			{
			for (i=0; i<blocksToRead; i++)
				{
				bytes = (pos + num > dataSize ? dataSize - pos : num).ToDWORD();
				memcpy(buffer, db[i].data + pos.ToDWORD() + headerSize, bytes);
				buffer += bytes;
				num -= bytes;
				pos = 0;
				}

			UnlockBlocks(firstBlock, blocksToRead, db, flags | DAT_UNLOCK_CLEAN, rh);
			numBlocks -= blocksToRead;
			firstBlock += blocksToRead;
			}
		else if (err == GNR_NOT_ENOUGH_MEMORY && blocksAtOnce > 1)
			{
			blocksAtOnce >>= 1;	// If there were not enough free blocks try with less
			err = GNR_OK;
			continue;
			}
		else
			UnlockBlocks(firstBlock, blocksToRead, db, flags | DAT_UNLOCK_CLEAN, rh);
		}

	//
	//  Cleanup
	//

	delete[] db;
	GNRAISE(err);
	}

//
//  Write bytes
//

Error GenericFile::WriteBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_FILE_READ_ONLY);
	}

////////////////////////////////////////////////////////////////////
//
//  Generic Directory Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericDirectory::GenericDirectory(GenericFileSystem * gfs) : GenericDiskItem(gfs)
	{
	}

//
//  Destructor
//

GenericDirectory::~GenericDirectory(void)
	{
	}

//
//  Return size
//

Error GenericDirectory::GetSize(KernelInt64 & size)
	{
	size = 0;
	GNRAISE_OK;
	}

//
//  Find item
//

Error GenericDirectory::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * where;
	Error err;

	GNREASSERT(GetIterator(where));
	err = gfs->FindItem(name, type, flags, where, gfsi);
	delete where;
	GNRAISE(err);
	}

////////////////////////////////////////////////////////////////////
//
//  Generic File System Iterator Implementation
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericFileSystemIterator::GenericFileSystemIterator(GenericFileSystem * gfs)
	{
	this->gfs = gfs;
#if _DEBUG
	//if (gfs->GetFileSystemType() != FS_ROOT) DP("From GenericFileSystemIterator - FS O: ");
#endif
	gfs->Obtain();
	}

//
//  Destructor
//

GenericFileSystemIterator::~GenericFileSystemIterator(void)
	{
#if _DEBUG
	//if (gfs->GetFileSystemType() != FS_ROOT) DP("From ~GenericFileSystemIterator - FS R: ");
#endif
	gfs->Release();
	}

//
//  Comparison operator
//

int operator==(GenericFileSystemIterator & gfsi1, GenericFileSystemIterator & gfsi2)
	{
	if (gfsi1.gfs == gfsi2.gfs)
		return gfsi1.Equals(&gfsi2);
	else
		return 0;
	}

//
//  Get the file system of the iterator
//

Error GenericFileSystemIterator::GetFileSystem(GenericFileSystem * & gfs)
	{
	gfs = this->gfs;
	GNRAISE_OK;
	}

//
//  Get the file system this iterator points to (only valid for RootIterator)
//

Error GenericFileSystemIterator::GetSubFileSystem(GenericFileSystem * & gfs)
	{
	GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

Error GenericFileSystemIterator::GetPathName(DiskItemName & name)
	{
	return gfs->GetPathName(name, this);
	}

//
//  Get extended item information
//

Error GenericFileSystemIterator::GetExtItemInfo(ExtItemInfo & info)
	{
	return gfs->GetExtItemInfo(this, info);
	}

////////////////////////////////////////////////////////////////////
//
//  Generic File System Implementation
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericFileSystem::GenericFileSystem(void)
	{
	genericVolume = NULL;
	mountPoint = NULL;
	profile = NULL;
	volumeName = "";
	volumeSetName = "";
	volumeSetIndex = 0;
	volumeSetSize = 0;
	refCount = 0;
	fileSystemType = FS_GENERIC;
	//DP("GenericFileSystem constructor");
	}

//
//  Destructor
//

GenericFileSystem::~GenericFileSystem(void)
	{
	//DP("Delete FS type %d\n", fileSystemType);
	if (refCount != 0)
		DP("WARNING: GenericFileSystem Ref Counter was not ZERO!");

	if (genericVolume)
		{
		//DP("From ~GenericFileSystem - FS R:");
		genericVolume->Release();
		}

	delete mountPoint;
	}

//
//  Initialize (this also sets the reference counter to 1)
//

Error	GenericFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	VDAutoMutex mutex(&lock);

	//DP("Init FS type %d\n", fileSystemType);
	refCount = 1;
	if (volume)
		{
		//DP("From GenericFileSystem::Init - V O:");
		GNREASSERT(volume->Obtain());
		}
	genericVolume = volume;
	GNREASSERT(mountPoint->Clone(this->mountPoint));
	this->profile = profile;
	logicalBaseBlock = 0;

	GNREASSERT(InitHeaderAndData(volume));
	GNREASSERT(GetCurrentDriveBlockSize(blockSize));

	GNRAISE_OK;
	}

//
// Initialize header and data related variables
//
Error	GenericFileSystem::InitHeaderAndData(GenericVolume * volume)
	{
	DWORD dummy;
	PhysicalDiskType diskType;

	GNREASSERT(volume->GetDiskType(diskType));

	switch (diskType)
		{
		case PHDT_CDDA:
			headerSectorType = DST_CDDA;
			dataSectorType = DST_CDDA;
			break;
		case PHDT_CDROM:
			headerSectorType = DST_CDROM_MODE1;
			dataSectorType = DST_CDROM_MODE2;
			break;
		case PHDT_CDI:
		case PHDT_CDROM_XA:
			headerSectorType = DST_CDROM_MODE2_XA_FORM1;
			dataSectorType = DST_CDROM_MODE2_XA_FORM2;
			break;
		case PHDT_DVD_ROM:
			headerSectorType = DST_DVD_ROM;
			dataSectorType = DST_DVD_ROM;
			break;
		default:
			GNRAISE(GNR_VOLUME_INVALID);
		}

	GNREASSERT(volume->GetBlockInfo(headerSectorType, dummy, headerHeaderSize, headerDataSize));
	GNREASSERT(volume->GetBlockInfo(dataSectorType, dummy, dataHeaderSize, dataDataSize));

	GNRAISE_OK;
	}


//
//  Obtain file system
//

Error GenericFileSystem::Obtain(void)
	{
	VDAutoMutex mutex(&lock);

	refCount++;
	//DP("Obtain FS type %d, refCount %d\n", fileSystemType, refCount);
	GNRAISE_OK;
	}

//
//  Release file system
//

Error GenericFileSystem::Release(void)
	{
		{
		VDAutoMutex mutex(&lock);

		if (refCount > 0)
			refCount--;
		//DP("Release FS type %d, refCount %d\n", fileSystemType, refCount);
		}

	if (!refCount)
		delete this;
	GNRAISE_OK;
	}

//
//  Create iterator
//

Error GenericFileSystem::CreateRootIterator(GenericFileSystemIterator * & gfsi)
	{
	if (mountPoint)
		GNRAISE(mountPoint->Clone(gfsi));
	else
		GNRAISE(CreateIterator(gfsi));
	}

//
//  Create directory
//

Error GenericFileSystem::CreateRootDirectory(GenericDirectory * & gd)
	{
	if (mountPoint)
		GNRAISE(mountPoint->gfs->CreateRootDirectory(gd));
	else
		GNRAISE(CreateDirectory(FAT_NONE, gd));
	}

//
//  Get disk type
//

Error GenericFileSystem::GetDiskType(PhysicalDiskType & type)
	{
	GNRAISE(genericVolume->GetDiskType(type));
	}

//
//  Get volume name
//

Error GenericFileSystem::GetVolumeName(KernelString & name)
	{
	VDAutoMutex mutex(&lock);

	name = volumeName;
	GNRAISE_OK;
	}

//
//  Get volume set name
//

Error GenericFileSystem::GetVolumeSetName(KernelString & name)
	{
	VDAutoMutex mutex(&lock);

	name = volumeSetName;
	GNRAISE_OK;
	}

//
//  Get volume set index
//

Error GenericFileSystem::GetVolumeSetIndex(int & index)
	{
	VDAutoMutex mutex(&lock);

	index = volumeSetIndex;
	GNRAISE_OK;
	}

//
//  Get volume set size
//

Error GenericFileSystem::GetVolumeSetSize(int & size)
	{
	VDAutoMutex mutex(&lock);

	size = volumeSetSize;
	GNRAISE_OK;
	}

//
//  Get unique key
//

Error GenericFileSystem::GetUniqueKey(BYTE * key)
	{
	if (!uniqueKey.valid)
		{
		uniqueKey.valid = FALSE;
		GNREASSERT(BuildUniqueKey());
		}

	uniqueKey.valid = TRUE;
	memcpy(key, uniqueKey.key, 8);
	GNRAISE_OK;
	}

//
//  Get logical base block
//

Error GenericFileSystem::GetLogicalBaseBlock(DWORD & logicalBaseBlock)
	{
	logicalBaseBlock = this->logicalBaseBlock;
	GNRAISE_OK;
	}

//
//  Get volume
//

Error GenericFileSystem::ObtainVolume(GenericVolume * & volume)
	{
	volume = genericVolume;
	if (volume)
		{
		//DP("From ObtainVolume - V O:");
		volume->Obtain();
		}
	GNRAISE_OK;
	}


//
//  Get drive block size
//

Error GenericFileSystem::GetCurrentDriveBlockSize(DWORD & size)
	{
	DWORD minSize, maxSize, step;

	VDAutoMutex mutex(&lock);

	if (genericVolume)
		{
		Error err;

		//Which size to ask for - header or data sector? Whichever - total size is the same for both, and file system relies on that.
		if (!IS_ERROR(err = genericVolume->GetDriveBlockSize(GetDataSectorType(), size, minSize, maxSize, step)))
			GNRAISE_OK;
		else
			GNRAISE(err);
		}
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Set drive block size
//

Error GenericFileSystem::SetDriveBlockSize(DWORD size)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get number of drive blocks
//

Error GenericFileSystem::GetNumberOfDriveBlocks(DWORD & num)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		return genericVolume->GetNumberOfDriveBlocks(num);
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Set number of drive blocks
//

Error GenericFileSystem::SetNumberOfDriveBlocks(DWORD num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Seek block
//

Error GenericFileSystem::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		return genericVolume->SeekBlock(logicalBaseBlock + block, flags, rh);
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Lock blocks
//

Error GenericFileSystem::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->LockBlocks(logicalBaseBlock + block, num, blocks, flags, rh));
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Unlock blocks
//

Error GenericFileSystem::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->UnlockBlocks(logicalBaseBlock + block, num, blocks, flags, rh));
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Terminate current request
//

Error GenericFileSystem::TerminateRequest(RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->TerminateRequest(rh));
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Flush everything
//

Error GenericFileSystem::Flush(void)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->Flush());
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Get name of drive
//

Error GenericFileSystem::GetDriveName(KernelString & name)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		return genericVolume->GetDriveName(name);
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Get drive capabilities
//

#if MSNDISC
Error GenericFileSystem::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		return genericVolume->GetDriveCaps(type, caps, slots,positions,changerOpenOffset);
	else
		GNRAISE(GNR_NO_VOLUME);
	}
#else
Error GenericFileSystem::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		return genericVolume->GetDriveCaps(type, caps, slots);
	else
		GNRAISE(GNR_NO_VOLUME);
	}
#endif
//
//  Wake up the drive
//

Error GenericFileSystem::SpinUpDrive(void)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->SpinUpDrive());
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Stop the drive
//

Error GenericFileSystem::SpinDownDrive(void)
	{
	VDAutoMutex mutex(&lock);

	if (genericVolume)
		GNRAISE(genericVolume->SpinDownDrive());
	else
		GNRAISE(GNR_NO_VOLUME);
	}


//
// Get Path name of item
//
Error GenericFileSystem::GetPathName(DiskItemName & name, GenericFileSystemIterator * where)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error GenericFileSystem::GetDTSInfo(BOOL & isDTS, GenericCopyManagementInfo & gcmi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}


//
//  Find item in (sub)directory
//  GNR_OK: Item was found
//  GNR_ITEM_NOT_FOUND: Item was not found
//  GNR_...: Some error accessing the disk occurred
//

Error GenericFileSystem::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags,
											 GenericFileSystemIterator * where, GenericFileSystemIterator * & result)
	{
	GenericFileSystemIterator * where2;
	DiskItemName frontComp;
	DiskItemName tail = name;
	Error err;

	if (name.IsRelative())
		{
		//
		//  If we have a relative path name then we have to go down all the directories (path components) specified
		//

		GNREASSERT(where->Clone(where2));
		}
	else
		{
		//
		//  With an absolute path name the search must start at the root directory
		//

		GNREASSERT(CreateIterator(where2));
		flags &= ~FIF_RECURSIVE;
		}

	//
	//  Now repeat search over all path components
	//

	while (TRUE)
		{
		frontComp = tail.GetFront();
		GNREASSERT(tail.RemoveFrontComp());
		if (frontComp.Length() == 0)		// We reached the last component, so the type must match
			{
			err = InternalFindItem(tail, type, flags, where2, result);
			break;
			}
		else
			{
			if (IS_ERROR(err = InternalFindItem(frontComp, DIT_DIR, flags, where2, result)))
				break;

			delete where2;
			err = result->GoToSubDir(where2);
			delete result;
			if (IS_ERROR(err))
				{
				where2 = NULL;
				break;
				}

			flags &= ~FIF_RECURSIVE;		// The first path component may be searched recursively, but them it has to be non-recursive
			}
		}

	delete where2;
	GNRAISE(err);
	}

//
//  Internal find item
//

Error GenericFileSystem::InternalFindItem(DiskItemName & name, DiskItemType type, DWORD flags,
														GenericFileSystemIterator * where, GenericFileSystemIterator * & result)
	{
	GenericFileSystemIterator * subDir;
	DiskItemType dit;
	DiskItemName itemName;
	Error err = GNR_OK;
	DWORD dinmFlags = ((flags & FIF_CASE_SENSITIVE) ? DINM_CASE_SENSITIVE : DINM_NONE);

	GNREASSERT(where->GoToFirstItem());

	while (!IS_ERROR(err))
		{
		GNREASSERT(where->GetItemType(dit));

		//
		//  Check if type and name match
		//

		if (dit == type || type == DIT_ALL)
			{
			GNREASSERT(where->GetItemName(itemName));
			if (itemName.Matches(name, dinmFlags))
				GNRAISE(where->Clone(result));
			}

		//
		//  If it's a dir the recursively check the sub dirs (if wanted)
		//

		if ((flags & FIF_RECURSIVE) && dit == DIT_DIR)
			{
			GNREASSERT(where->GoToSubDir(subDir));
			err = InternalFindItem(name, type, dinmFlags, subDir, result);
			delete subDir;

			if (err != GNR_ITEM_NOT_FOUND)
				GNRAISE(err);
			}

		err = where->GoToNextItem();
		}

	GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Open item by name
//

Error GenericFileSystem::OpenItem(const DiskItemName & name, DWORD accessType, GenericDiskItem * & gdi)
	{
	GenericFileSystemIterator * where;
	GenericFileSystemIterator * item;
	DWORD findFlags = FIF_NONE;
	Error err;

	//
	//  Find item
	//

//	DiskItemName n = name;
//	DP("Open File %s", (char*)n);
	GNREASSERT(CreateIterator(where));

	if (accessType & FAT_RECURSIVE)
		findFlags = FIF_RECURSIVE;
	if (accessType & FAT_CASE_SENSITIVE)
		findFlags = FIF_CASE_SENSITIVE;

	err = FindItem(name, DIT_ALL, findFlags, where, item);

	if (!IS_ERROR(err))
		{
		err = item->OpenItem(accessType, gdi);
		delete item;
		}

	delete where;
	GNRAISE(err);
	}

//
//  Open item by iterator
//

Error GenericFileSystem::OpenItem(GenericFileSystemIterator * gfsi, DWORD accessType, GenericDiskItem * & gdi)
	{
	VDAutoMutex mutex(&lock);
	DiskItemType type;
	GenericFile * gf;
	GenericDirectory * gd;
	Error err;

	GNREASSERT(gfsi->GetItemType(type));
	switch (type)
		{
		case DIT_FILE:
			GNREASSERT(CreateFile(accessType, gf));
			gdi = gf;
			break;
		case DIT_DIR:
			GNREASSERT(CreateDirectory(accessType, gd));
			gdi = gd;
			break;
		default:
			GNRAISE(GNR_OBJECT_INVALID);
		}

	if (IS_ERROR(err = gdi->Open(gfsi, accessType)))
		delete gdi;

	GNRAISE(err);
	}

//
//  Get extended item information
//

Error GenericFileSystem::GetExtItemInfo(GenericFileSystemIterator * gfsi, ExtItemInfo & info)
	{
	info.exType = EDIT_NONE;
	GNRAISE_OK;
	}
