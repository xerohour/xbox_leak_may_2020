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
//  ISO 9660 File Classes
//
////////////////////////////////////////////////////////////////////

#include "library/files/ISOFile.h"

////////////////////////////////////////////////////////////////////
//
//  Internal defines
//
////////////////////////////////////////////////////////////////////

//
//  Volume descriptor types
//

#define ISO_VDT_BOOT				0
#define ISO_VDT_PRIMARY			1
#define ISO_VDT_SUPPLEMENT		2
#define ISO_VDT_PARTITION		3
#define ISO_VDT_TERMINATOR		255

//
//  Directory record offsets
//

#define ISO_DR_LEN			0		// 1 byte
#define ISO_DR_EAD_LEN		1		// 1 byte
#define ISO_DR_EXT_LOC		2		// 8 bytes
#define ISO_DR_EXT_LEN		10		// 8 bytes
#define ISO_DR_FLAGS			25		// 1 byte
#define ISO_DR_FID_LEN		32		// 1 byte
#define ISO_DR_FID			33		// Beginning of file identifier

//
//  File flags in directory record (values are bit numbers)
//

#define ISO_FILE_EXIST		0x01		// 1 for a hidden file
#define ISO_FILE_DIR			0x02		// 1 if record identifies directory
#define ISO_FILE_ASSOC		0x04		// 1 if associated file
#define ISO_FILE_RECORD		0x08		// 1 if file format specified by extended attribute record
#define ISO_FILE_PROTECT	0x10		//	1 if access rights are specified
#define ISO_FILE_MULTIEXT	0x80		// 1 if record is not the final record of directory

//
//  Flags for System Use field (bytes 4 and 5, 5 being MSB)
//

#define ISO_SUI_OWNER_READ				0x0001
#define ISO_SUI_OWNER_EXEC				0x0004
#define ISO_SUI_GROUP_READ				0x0010
#define ISO_SUI_GROUP_EXEC				0x0040
#define ISO_SUI_WORLD_READ				0x0100
#define ISO_SUI_WORLD_EXEC				0x0400
#define ISO_SUI_FILE_FORM1				0x0800
#define ISO_SUI_FILE_FORM2				0x1000
#define ISO_SUI_FILE_INTERLEAVED		0x2000
#define ISO_SUI_FILE_CDDA				0x4000
#define ISO_SUI_FILE_IS_DIR			0x8000

//
//  Defines for Suplementary Volume Descriptor
//

#define ISO_SVD_ESCAPE		88
#define ISO_SVD_ESCAPE_LEN	32

//
//  Data access macros (d must be BYTE*)
//

#define ISO_DWORD(d)		(MAKELONG4((d)[0], (d)[1], (d)[2], (d)[3]))
#define ISO_WORD(d)		(MAKEWORD((d)[0], (d)[1]))
#define ISO_BYTE(d)		((d)[0])

////////////////////////////////////////////////////////////////////
//
//  ISO File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

ISOFile::ISOFile(ISOFileSystem * isofs) : GenericFile(isofs)
	{
	this->isofs = isofs;
	isoi = NULL;
	}

//
//  Destructor
//

ISOFile::~ISOFile(void)
	{
	Close();
	}

//
//  Open file
//

Error ISOFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericFileSystemIterator * gfsi2;
	Error err;

	if (accessType & FAT_WRITE)
		GNRAISE(GNR_FILE_READ_ONLY);

	GNREASSERT(GenericFile::Open(gfsi, accessType));

	GNREASSERT(gfsi->Clone(gfsi2));
	isoi = (ISOIterator*)gfsi2;

	err = isofs->GetStartBlock(isoi, startBlock);
	if (!IS_ERROR(err))
		err = isofs->GetItemSize(isoi, size);

	if (IS_ERROR(err))
		{
		delete isoi;
		isoi = NULL;
		}

	GNRAISE(err);
	}

//
//  Close file
//

Error ISOFile::Close(void)
	{
	delete isoi;
	isoi = NULL;
	GNRAISE_OK;
	}

//
//  Get name
//

Error ISOFile::GetName(DiskItemName & name)
	{
	return isofs->GetItemName(isoi, name);
	}

//
//  Get path name
//

Error ISOFile::GetPathName(DiskItemName & name)
	{
	return isofs->GetPathName(isoi, name);
	}

//
//  Get size
//

Error ISOFile::GetSize(KernelInt64 & size)
	{
	size = this->size;
	GNRAISE_OK;
	}

//
//  Get iterator
//

Error ISOFile::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return isoi->Clone(gfsi);
	}

//
//  Build unique key
//

Error ISOFile::BuildUniqueKey(void)
	{
	GNRAISE_OK;
	}

//
//  Get copy management mode
//

Error ISOFile::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	gcmi = GCMI_COPY_FORBIDDEN;
	GNRAISE_OK;
	}

//
//  Seek block
//

Error ISOFile::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	if (block * isofs->blockSize >= size)
		GNRAISE(GNR_INVALID_PARAMETERS);

	return isofs->SeekBlock(block, flags, rh);
	}

//
//  Lock blocks
//

Error ISOFile::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
//	if ((block + num - 1) * isofs->blockSize >= size)
//		GNRAISE(GNR_INVALID_PARAMETERS);
	if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | isofs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | isofs->GetDataSectorType();

	return isofs->LockBlocks(startBlock + block, num, blocks, flags, rh);
	}

//
//  Unlock blocks
//

Error ISOFile::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
//	if ((block + num - 1) * isofs->blockSize >= size)
//		GNRAISE(GNR_INVALID_PARAMETERS);
	if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | isofs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | isofs->GetDataSectorType();

	return isofs->UnlockBlocks(startBlock + block, num, blocks, flags, rh);
	}

////////////////////////////////////////////////////////////////////
//
//  ISO Directory Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

//
//  Constructor
//

ISODirectory::ISODirectory(ISOFileSystem * isofs) : GenericDirectory(isofs)
	{
	this->isofs = isofs;
	isoi = NULL;
	}

//
//  Destructor
//

ISODirectory::~ISODirectory(void)
	{
	Close();
	}

//
//  Open directory
//

Error ISODirectory::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericFileSystemIterator * gfsi2;

	if (accessType & FAT_WRITE)
		GNRAISE(GNR_FILE_READ_ONLY);

	GNREASSERT(gfsi->Clone(gfsi2));
	isoi = (ISOIterator*)gfsi2;

	GNRAISE_OK;
	}

//
//  Close
//

Error ISODirectory::Close(void)
	{
	delete isoi;
	isoi = NULL;
	GNRAISE_OK;
	}

//
//  Get name
//

Error ISODirectory::GetName(DiskItemName & name)
	{
	return isofs->GetItemName(isoi, name);
	}

//
//  Get path name
//

Error ISODirectory::GetPathName(DiskItemName & name)
	{
	return isofs->GetPathName(isoi, name);
	}

//
//  Get iterator
//

Error ISODirectory::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return isoi->Clone(gfsi);
	}

//
//  Create item
//

Error ISODirectory::CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Delete item
//

Error ISODirectory::DeleteItem(GenericFileSystemIterator * gfsi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Get number of items in dir
//

Error ISODirectory::GetNumberOfItems(DWORD & num)
	{
	return isofs->GetNumberOfItems(isoi, num);
	}

//
//  Find item
//

Error ISODirectory::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * where;
	Error err;

	GNREASSERT(GetIterator(where));
	if (!IS_ERROR(err = isofs->FindItem(name, type, flags, where, gfsi)))
		delete where;
	GNRAISE(err);
	}

#endif

////////////////////////////////////////////////////////////////////
//
//  ISO File System Iterator Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

ISOIterator::ISOIterator(GenericFileSystem * gfs, DWORD dirStart, DWORD dirSize) : GenericFileSystemIterator(gfs)
	{
	this->dirStart = dirStart;
	this->dirSize = dirSize;
	((ISOFileSystem*)gfs)->GoToFirstItem(this);
	}

//
//  Constructor
//

ISOIterator::ISOIterator(GenericFileSystem * gfs, DWORD dirStart, DWORD dirSize, DWORD entryOffset) : GenericFileSystemIterator(gfs)
	{
	this->dirStart = dirStart;
	this->dirSize = dirSize;
	this->entryOffset = entryOffset;
	}

//
//  Destructor
//

ISOIterator::~ISOIterator(void)
	{
	}

//
//  Clone
//

Error ISOIterator::Clone(ISOIterator * & isoi) const
	{
	isoi = new ISOIterator(gfs, dirStart, dirSize, entryOffset);
	if (isoi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Clone
//

Error ISOIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	gfsi = new ISOIterator(gfs, dirStart, dirSize, entryOffset);
	if (gfsi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Comparison function
//

BOOL ISOIterator::Equals(GenericFileSystemIterator * gfsi)
	{
	GNRAISE( (dirStart    == ((ISOIterator*)gfsi)->dirStart) &&
				(entryOffset == ((ISOIterator*)gfsi)->entryOffset));
	}

//
//  Go to first item
//

Error ISOIterator::GoToFirstItem(void)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GoToFirstItem(this));
	}

//
//  Go to next item
//

Error ISOIterator::GoToNextItem(void)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GoToNextItem(this));
	}

//
//  Go to sub dir
//

Error ISOIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GoToSubDir(this, gfsi));
	}

//
//  Go to parent dir
//

Error ISOIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GoToParentDir(this, gfsi));
	}

//
//  Get path name
//

Error ISOIterator::GetPathName(DiskItemName & name)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetPathName(this, name));
	}

//
//  Get item type
//

Error ISOIterator::GetItemType(DiskItemType & type)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetItemType(this, type));
	}

//
//  Get item name
//

Error ISOIterator::GetItemName(DiskItemName & name)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetItemName(this, name));
	}

//
//  Get item size
//

Error ISOIterator::GetItemSize(KernelInt64 & size)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetItemSize(this, size));
	}

//
//  Get number of items in the directory the iterator is in
//

Error ISOIterator::GetNumberOfItems(DWORD & num)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetNumberOfItems(this, num));
	}

//
//  Get current directory
//

Error ISOIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	GNRAISE(((ISOFileSystem*)gfs)->GetDir(this, gd));
	}

//
//  Open item
//

Error ISOIterator::OpenItem(DWORD accessType, GenericDiskItem * & gdi)
	{
	GNRAISE(((ISOFileSystem*)gfs)->OpenItem(this, accessType, gdi));
	}

////////////////////////////////////////////////////////////////////
//
//   ISO File System Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

ISOFileSystem::ISOFileSystem(void) : GenericFileSystem()
	{
	cdvdVolume = NULL;
	fileSystemType = FS_ISO;
	}

//
//  Destructor
//

ISOFileSystem::~ISOFileSystem(void)
	{
	}

//
//  Initialize
//

Error ISOFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	VDAutoMutex mutex(&lock);
	DWORD pvdBlock;
	DWORD descSeqStart;	// The beginning of the volume descriptor sequence on either DVD or CD-ROM
	CDSessionType sessionType = CST_NONE;
	PhysicalDiskType diskType;
	Error err;
	BOOL valid = FALSE;
	WORD retry = 16;
	DriveBlock driveBlock;
	int pos;
	int i;

	if (cdvdVolume)
		GNRAISE(GNR_OBJECT_IN_USE);

	GNREASSERT(GenericFileSystem::Init(mountPoint, volume, profile));

	//
	//  Determine location of Volume descriptor sequence
	//  Must be in first track of last session (if this is a data track)
	//

	GNREASSERT(volume->GetDiskType(diskType));
	cdvdVolume = (CDVDVolume*)volume;
	switch (diskType)
		{
		case PHDT_CDROM:
		case PHDT_CDI:
		case PHDT_CDROM_XA:
			{
			WORD sessions;
			CDTocEntry * toc;
			WORD tocEntryNum;

			//
			//  Determine number of sessions
			//

			GNREASSERT(cdvdVolume->GetNumberOfSessions(sessions));
			if (sessions == 0)
				GNRAISE(GNR_VOLUME_INVALID);

			DP("There are %d sessions", sessions);

			//
			//  Get base block, which is first block of first track in first session
			//

			GNREASSERT(cdvdVolume->ReadCDTOC(1, sessionType, toc, tocEntryNum, &rh));
			logicalBaseBlock = toc[0].GetStartBlock();
			delete[] toc;

			//
			//  Get descriptor sequence start block, block 16 of first track of last session
			//

			if (sessions > 1)
				{
				GNREASSERT(cdvdVolume->ReadCDTOC(sessions, sessionType, toc, tocEntryNum, &rh));
				descSeqStart = toc[0].GetStartBlock() + 15 - logicalBaseBlock;		// Start with one less (should be 16) because we increment first
																										// Also this already includes the logicalBaseBlock
				delete[] toc;
				}
			else
				descSeqStart = 15;
			break;
			}
		case PHDT_DVD_ROM:
			descSeqStart = 15;
			logicalBaseBlock = 0;
			break;
		default:
			GNRAISE(GNR_VOLUME_INVALID);
		}

	//
	//  Find Primary And Supplementary Volume Descriptor
	//

	unicodeType = IUT_NONE;
	pvdBlock = descSeqStart;
	rootDirSize = 0;

	do
		{
		pvdBlock++;
		if (IS_ERROR(err = LockBlocks(pvdBlock, 1, &driveBlock, headerSectorType | DAT_LOCK_AND_READ, &rh)))
			{
			UnlockBlocks(pvdBlock, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN, &rh);
			GNRAISE(err);
			}

		//
		//  Check standard identifier and on Volume Descriptor Set Terminator
		//  If standard is not correct or Terminator reached then we're f***ed
		//

		if (valid = ((driveBlock.data[dataHeaderSize + 1] == 'C') &&
						 (driveBlock.data[dataHeaderSize + 2] == 'D') &&
						 (driveBlock.data[dataHeaderSize + 3] == '0') &&
						 (driveBlock.data[dataHeaderSize + 4] == '0') &&
						 (driveBlock.data[dataHeaderSize + 5] == '1')))
			{
			//
			//  Now check which type of volume descriptor we have
			//

			if (driveBlock.data[dataHeaderSize] == ISO_VDT_TERMINATOR)
				{
				retry = 1;	// Exit loop
				}
			else if (driveBlock.data[dataHeaderSize] == ISO_VDT_PRIMARY)
				{
				//
				//  Initialize misc
				//

				volumeName = DiskItemName((char*)(driveBlock.data + dataHeaderSize + 40), 32);
				volumeSetSize = ISO_WORD(driveBlock.data + dataHeaderSize + 120);
				volumeSetIndex = ISO_WORD(driveBlock.data + dataHeaderSize + 124);
				volumeSetName = DiskItemName((char*)(driveBlock.data + dataHeaderSize + 190), 128);

				//
				//  Read root directory record
				//

				rootDirStart = ISO_DWORD(driveBlock.data + dataHeaderSize + 156 + 2);
				rootDirSize = ISO_DWORD(driveBlock.data + dataHeaderSize + 156 + 10);

				//
				//  Compute unique key from time stamp
				//

				for (i=0; i<8; i++)
					uniqueKey.key[i] = ISO_BYTE(driveBlock.data + dataHeaderSize + 813 + i) ^
											 ISO_BYTE(driveBlock.data + dataHeaderSize + 821 + i);
				}
			else if (driveBlock.data[dataHeaderSize] == ISO_VDT_SUPPLEMENT)
				{
				DWORD tmpRootDirStart = ISO_DWORD(driveBlock.data + dataHeaderSize + 156 + 2);
				DWORD tmpRootDirSize = ISO_DWORD(driveBlock.data + dataHeaderSize + 156 + 10);

				BYTE drLen = 0;
				GNREASSERT(ReadByte(tmpRootDirStart, ISO_DR_LEN, drLen));

 				if (drLen > 1)
					{
					rootDirStart = tmpRootDirStart;
					rootDirSize	= tmpRootDirSize;
					}

				//
				//  Look for Joliet Unicode character escape sequence
				//

				pos = dataHeaderSize + ISO_SVD_ESCAPE;
				for (i=0; i<ISO_SVD_ESCAPE_LEN -2 ; i++)
					{
					if (driveBlock.data[pos] == 0x25 && driveBlock.data[pos + 1] == 0x2f)
						{
						switch (driveBlock.data[pos + 2])
							{
							case 0x40:
								unicodeType = IUT_UCS2_L1;
								break;
							case 0x43:
								unicodeType = IUT_UCS2_L2;
								break;
							case 0x45:
								unicodeType = IUT_UCS2_L3;
								break;
							}
						}
					pos++;
					}
				}
			}

		UnlockBlocks(pvdBlock, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN, &rh);
		retry--;
		}
	while (retry);

	if (!rootDirSize)		// No PVD found
		GNRAISE(GNR_VOLUME_INVALID);

	GNRAISE_OK;
	}

//
//  Read BYTE (directory block only)
//

Error ISOFileSystem::ReadByte(DWORD block, DWORD offset, BYTE & b)
	{
	Error err = GNR_OK;
	block += offset / headerDataSize;
	offset = (offset % headerDataSize) + headerHeaderSize;

	if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, headerSectorType | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
		b = ISO_BYTE(driveBlock.data + offset);
	UnlockBlocks(block, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
	GNRAISE(err);
	}

//
//  Read WORD (directory block only)
//

Error ISOFileSystem::ReadWord(DWORD block, DWORD offset, WORD & w)
	{
	Error err = GNR_OK;
	block += offset / headerDataSize;
	offset = (offset % headerDataSize) + headerHeaderSize;

	if (offset + 2 < blockSize)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, headerSectorType | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
			w = ISO_WORD(driveBlock.data + offset);
		UnlockBlocks(block, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
		GNRAISE(err);
		}
	else
		{
		GNREASSERT(ReadByte(block, offset, ((BYTE*)&w)[0]));
		return ReadByte(block, offset + 1, ((BYTE*)&w)[1]);
		}
	}

//
//  Read DWORD (directory block only)
//

Error ISOFileSystem::ReadDWord(DWORD block, DWORD offset, DWORD & d)
	{
	Error err = GNR_OK;
	block += offset / headerDataSize;
	offset = (offset % headerDataSize) + headerHeaderSize;

	if (offset + 4 < blockSize)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, headerSectorType | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
			d = ISO_DWORD(driveBlock.data + offset);
		UnlockBlocks(block, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
		GNRAISE(err);
		}
	else
		{
		GNREASSERT(ReadByte(block, offset, ((BYTE*)&d)[0]));
		GNREASSERT(ReadByte(block, offset + 1, ((BYTE*)&d)[1]));
		GNREASSERT(ReadByte(block, offset + 2, ((BYTE*)&d)[2]));
		return ReadByte(block, offset + 3, ((BYTE*)&d)[3]);
		}
	}

//
//  Create iterator
//

Error ISOFileSystem::CreateIterator(ISOIterator * & isoi, DWORD dirStart, DWORD dirSize)
	{
	isoi = new ISOIterator(this, dirStart, dirSize);
	if (isoi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create iterator
//

Error ISOFileSystem::CreateIterator(ISOIterator * & isoi)
	{
	return CreateIterator(isoi, rootDirStart, rootDirSize);
	}

//  Create iterator
//

Error ISOFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	ISOIterator * isoi;

	GNREASSERT(CreateIterator(isoi, rootDirStart, rootDirSize));
	gfsi = isoi;
	GNRAISE_OK;
	}

//
//	 Create file
//

Error ISOFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	gf = new ISOFile(this);
	if (gf)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error ISOFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
#if GFS_ENABLE_DIRECTORIES
	gd = new ISODirectory(this);
	if (gd)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
#else
	GNRAISE(GNR_UNIMPLEMENTED);
#endif
	}



//
//  Go to first item
//

Error ISOFileSystem::GoToFirstItem(ISOIterator * isoi)
	{
	VDAutoMutex mutex(&lock);
	BYTE drLen;
	BYTE flags;

	//
	//  Find offset to third (first real) entry
	//

	isoi->entryOffset = isoi->dirStart;

	GNREASSERT(ReadByte(isoi->dirStart, ISO_DR_LEN, drLen));
	isoi->entryOffset = drLen;

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_LEN, drLen));
	isoi->entryOffset += drLen;

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_LEN, drLen));


	if ((isoi->entryOffset >= isoi->dirSize) || (!drLen))
		GNRAISE(GNR_ITEM_NOT_FOUND);
	else
		{
		//
		//  Check if file is an associated one. If so, the skip it.
		//  This fixes double directory entries created by Macintosh CD writing programs.
		//

		GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FLAGS, flags));
		if (flags & ISO_FILE_ASSOC)
			GNRAISE(GoToNextItem(isoi));
		GNRAISE_OK;
		}
	}

//
//  Go to next item
//

Error ISOFileSystem::GoToNextItem(ISOIterator * isoi)
	{
	VDAutoMutex mutex(&lock);
	BYTE drLen;
	BYTE flags;
	DWORD offset;

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_LEN, drLen));

	offset = isoi->entryOffset + drLen;

	//
	//  If length byte of the next is 0 then the next record didn't fit into block
	//  (so we have to move to the next block) or we reached the end of the dir
	//

	GNREASSERT(ReadByte(isoi->dirStart, offset + ISO_DR_LEN, drLen));

	if (!drLen)
		{
		offset = isoi->entryOffset + headerDataSize - (isoi->entryOffset % headerDataSize);

		if (offset >= isoi->dirSize)
			GNRAISE(GNR_ITEM_NOT_FOUND);
		}

	isoi->entryOffset = offset;

	//
	//  Check if file is an associated one. If so, the skip it.
	//  This fixes double directory entries created by Macintosh CD writing programs.
	//

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FLAGS, flags));
	if (flags & ISO_FILE_ASSOC)
		GNRAISE(GoToNextItem(isoi));

	GNRAISE_OK;
	}

//
//  Go to sub dir
//

Error ISOFileSystem::GoToSubDir(ISOIterator * isoi, GenericFileSystemIterator * & gfsi)
	{
	VDAutoMutex mutex(&lock);
	ISOIterator * newIsoi;
	DWORD start;
	DWORD len;
	BYTE flags;
	Error err;

	//
	//  Make sure it's a dir
	//

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FLAGS, flags));
	if (!(flags & ISO_FILE_DIR))
		GNRAISE(GNR_NOT_A_DIRECTORY);

	//
	//  Read sub dir info and create iterator with it
	//

	GNREASSERT(ReadDWord(isoi->dirStart, isoi->entryOffset + ISO_DR_EXT_LOC, start));
	GNREASSERT(ReadDWord(isoi->dirStart, isoi->entryOffset + ISO_DR_EXT_LEN, len));

	err = CreateIterator(newIsoi, start, len);
	gfsi = newIsoi;
	GNRAISE(err);
	}

//
//  Go to parent dir
//

Error ISOFileSystem::GoToParentDir(ISOIterator * isoi, GenericFileSystemIterator * & gfsi)
	{
	VDAutoMutex mutex(&lock);
	ISOIterator * newIsoi;
	Error err;
	DWORD start;
	DWORD len;
	BYTE drLen;

	//
	//  Create new iterator from either parent dir entry (2nd) or parent file file system
	//

	if (isoi->dirStart == rootDirStart)
		return mountPoint->Clone(gfsi);

	//
	//  Find second entry (to parent), then read all we need to know,
	//  then create iterator from it
	//

	GNREASSERT(ReadByte(isoi->dirStart, ISO_DR_LEN, drLen));
	GNREASSERT(ReadDWord(isoi->dirStart, drLen + ISO_DR_EXT_LOC, start));
	GNREASSERT(ReadDWord(isoi->dirStart, drLen + ISO_DR_EXT_LEN, len));

	err = CreateIterator(newIsoi, start, len);
	gfsi = newIsoi;
	GNRAISE(err);

	}

//
//  Get path name to item
//

Error ISOFileSystem::GetPathName(ISOIterator *isoi, DiskItemName & name)
	{
	VDAutoMutex mutex(&lock);

	DiskItemName pname;	// used to take a look into the cache...
	name = DiskItemName("");

	ISOIterator * imisoi;
	ISOIterator * parenti = NULL;

	Error err = GNR_OK;

	GNREASSERT(isoi->Clone(imisoi));
	while ((IS_ERROR(err = cache.Get(imisoi->dirStart, pname))) &&  (imisoi->dirStart != this->rootDirStart) )
		{
		err = this->GoToParentDir(imisoi, (GenericFileSystemIterator * &) parenti);
		if (!IS_ERROR(err))
			err = this->GoToFirstItem(parenti);

		if (!IS_ERROR(err))
			{
			// search
			BOOL found = FALSE;
			while (!found)
				{
				DiskItemType type;
				err = this->GetItemType(parenti, type);
				if (!IS_ERROR(err))


					{
					if (type == DIT_DIR)
						{
						ISOIterator * isosubi = NULL;
						err = parenti->GoToSubDir((GenericFileSystemIterator *&) isosubi);
						if (!IS_ERROR(err))
							if (isosubi->dirStart == imisoi->dirStart)
								{
								DiskItemName rname;
								parenti->GetItemName(rname);
								name = rname + name;
								found = TRUE;
								}
						delete isosubi;
						}
					}

				if (IS_ERROR(err))
					{
					delete parenti;
					delete imisoi;
					}



				err = GoToNextItem(parenti);
				if ( (!found) && IS_ERROR(err) )
					{
					delete parenti;
					delete imisoi;
					GNRAISE(GNR_ITEM_NOT_FOUND);
					}
				}
			}

		delete imisoi;
		imisoi = parenti;
		}


	// found it in cache?
	if (err == GNR_OK)
		{
		name = pname + "/" + name;
		}
	// go through mount point...
	else if (this->mountPoint)
		{
		DiskItemName mpName;
		DiskItemName mpPath;
		if (IS_ERROR(err = this->mountPoint->GetPathName(mpPath)))
			{
			delete imisoi;
			GNRAISE(err);
			}
		if (IS_ERROR(err = this->mountPoint->GetItemName(mpName)))
			{
			delete imisoi;
			GNRAISE(err);
			}
		name = mpPath + (DiskItemName)"/" + mpName + name;
		cache.Add(name, isoi->dirStart);
		}
	else
		{
		// no mount point? Should not be, but do not panic. Claim to be root...
		name = (DiskItemName)"/" + name;
		}

	delete imisoi;
	GNRAISE_OK;
	}

//
//  Get type of item
//

Error ISOFileSystem::GetItemType(ISOIterator * isoi, DiskItemType & type)
	{
	VDAutoMutex mutex(&lock);
	BYTE flags;

	//
	//  Read file flags of entry
	//

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FLAGS, flags));

	if (flags & ISO_FILE_DIR)
		type = DIT_DIR;
	else
		type = DIT_FILE;

	GNRAISE_OK;
	}

//
//  Get name of item
//

Error ISOFileSystem::GetItemName(ISOIterator * isoi, DiskItemName & name)
	{
	VDAutoMutex mutex(&lock);
	RequestHandle rh;
	DWORD block;
	DWORD offset;
	BYTE buffer[256];
	BYTE identLen;
	BYTE len;
	BYTE inc;
	Error err;
	int i;

	//
	//  Read length of file identifier
	//

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FID_LEN, identLen));

	//
	//  Let's compute the block if the identifier, it can't cross block boundaries
	//

	offset = isoi->entryOffset + ISO_DR_FID;
	block = isoi->dirStart + (offset / headerDataSize);
	offset = (offset % headerDataSize) + headerHeaderSize;

	//
	//  Read file identifier
	//

	if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, headerSectorType | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
		{
		int pos = 0;

		len = identLen;
		inc = 1;
		if (unicodeType != IUT_NONE)
			{
			if (!driveBlock.data[offset])
				{
				identLen >>= 1;	// Real length is only half of it
				offset++;			// Begin with the first non-zero byte
				inc = 2;
				}
			else
				unicodeType = IUT_NONE;		// This is a fake for some CD writing programs who claim to write unicode, but really do not
			}

		for (i=0; i<len; i+=inc)
			{
			buffer[pos] = driveBlock.data[offset + i];
			pos++;
			}
		}
	UnlockBlocks(block, 1, &driveBlock, headerSectorType | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);

	GNREASSERT(err);

	//
	//  Remove trailing separator/file version number
	//

	len = identLen - 1;
	while (len)
		{
		if (buffer[len] == 0x3b)	// Separator 2
			{
			name = DiskItemName((char*)buffer, len);
			GNRAISE_OK;
			}
		len--;
		}

	name = DiskItemName((char*)(buffer), identLen);
	GNRAISE_OK;
	}

//
//  Get size of item
//

Error ISOFileSystem::GetItemSize(ISOIterator * isoi, KernelInt64 & size)
	{
	VDAutoMutex mutex(&lock);
	DiskItemType type;
	DWORD hsize;

	GNREASSERT(GetItemType(isoi, type));
	if (type == DIT_DIR)
		{
		size = 0;
		GNRAISE_OK;
		}

	GNREASSERT(ReadDWord(isoi->dirStart, isoi->entryOffset + ISO_DR_EXT_LEN, hsize));
	size = (KernelInt64)hsize;
	GNRAISE_OK;
	}

//
//  Read CD-ROM XA System Use Information
//

Error ISOFileSystem::ReadXASUInfo(ISOIterator * isoi, WORD & sui)
	{
	BYTE drLen, fidLen, systemUsePos;

	//
	//  Now let's see what the system use field says (See "System Description CD-ROM XA, Chapter III.2")
	//

	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_LEN, drLen));
	GNREASSERT(ReadByte(isoi->dirStart, isoi->entryOffset + ISO_DR_FID_LEN, fidLen));
	systemUsePos = 33 + fidLen + 1 - (fidLen & 1);
	DP("Size of System use is %d", drLen - systemUsePos);

	if (drLen - systemUsePos == 14)		// According to System Description CD-ROM XA
		{
		//
		//  Check signature bytes
		//

		GNREASSERT(ReadWord(isoi->dirStart, isoi->entryOffset + systemUsePos + 6, sui));
		if (sui != 0x4158)
			{
			DP("No sui");
			GNRAISE(GNR_OBJECT_NOT_FOUND);
			}

		GNREASSERT(ReadWord(isoi->dirStart, isoi->entryOffset + systemUsePos + 4, sui));
		sui = MAKEWORD(HIBYTE(sui), LOBYTE(sui));
		DP("SUI: %04x", sui);
		}

	GNRAISE_OK;
	}

//
//  Get the number of items in the dir the iterator is currently in
//

Error ISOFileSystem::GetNumberOfItems(ISOIterator * isoi, DWORD & num)
	{
	VDAutoMutex mutex(&lock);
	BYTE drLen;
	DWORD offset;

	//
	//  Find offset to third (first real) entry
	//

	GNREASSERT(ReadByte(isoi->dirStart, ISO_DR_LEN, drLen));
	offset = drLen;
	GNREASSERT(ReadByte(isoi->dirStart, offset + ISO_DR_LEN, drLen));
	offset += drLen;

	//
	//  Now repeat going to next item until end of dir is reached
	//

	num = 0;
	while (offset < isoi->dirSize)
		{
		GNREASSERT(ReadByte(isoi->dirStart, offset + ISO_DR_LEN, drLen));
		if (!drLen)
			{
			offset = offset + headerDataSize - (offset % headerDataSize);
			}
		else
			{
			offset += drLen;
			num++;
			}
		}

	GNRAISE_OK;
	}

//
//  Get directory object for dir the iterator points to
//

Error ISOFileSystem::GetDir(ISOIterator * isoi, GenericDirectory * & gd)
	{
	VDAutoMutex mutex(&lock);

	GNRAISE_OK;
	}

//
//  Build unique key
//

Error ISOFileSystem::BuildUniqueKey(void)
	{
	GNRAISE_OK;
	}

//
//  Get start block of file/dir
//

Error ISOFileSystem::GetStartBlock(ISOIterator * isoi, DWORD & startBlock)
	{
	return ReadDWord(isoi->dirStart, isoi->entryOffset + ISO_DR_EXT_LOC, startBlock);
	}
