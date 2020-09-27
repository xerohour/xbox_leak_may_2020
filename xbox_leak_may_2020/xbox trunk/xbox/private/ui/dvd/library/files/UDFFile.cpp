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
//  UDF File Classes
//
////////////////////////////////////////////////////////////////////

#include "library/files/UDFFile.h"

////////////////////////////////////////////////////////////////////
//
//  Defines
//
////////////////////////////////////////////////////////////////////

//
//  UDF descriptor tag values
//

#define UDF_TAG_TERMINATE			  8
#define UDF_TAG_FILESET_DESC		256
#define UDF_TAG_FILE_IDENT			257
#define UDF_TAG_ALLOC_EXTENT		258
#define UDF_TAG_INDIRECT_ENTRY	259
#define UDF_TAG_TERMINAL_ENTRY	260
#define UDF_TAG_FILE_ENTRY			261
#define UDF_TAG_EXT_ATTR_HEADER	262
#define UDF_TAG_UNALLOC_SPACE		263
#define UDF_TAG_SPACE_MAP			264
#define UDF_TAG_PARTITION_INTEG	265

//
//  UDF File Entry (ICB) Offsets
//

#define UDF_ICB_DESC_TAG			0		// 16 bytes
#define UDF_ICB_INFO_LEN			56		// 8 bytes
#define UDF_ICB_EXT_ATTR_LEN		168	// 4 bytes
#define UDF_ICB_ALLOC_DESC_LEN	172	// 4 bytes
#define UDF_ICB_EXT_ATTR			176

//
//  Offsets within Primary Volume Descriptor
//

#define UDF_PVD_VOLUME_IDENT		24
#define UDF_PVD_VOLUME_SET_INDEX	56
#define UDF_PVD_VOLUME_SET_SIZE	58
#define UDF_PVD_VOLUME_SET_IDENT	72
#define UDF_PVD_RECORDING_DATE	376

//
//  UDF File Identifier Descriptor Offsets
//

#define UDF_FID_DESC_TAG		0		// 16 bytes
#define UDF_FID_FILE_CHAR		18		// 1 byte
#define UDF_FID_IDENT_LEN		19		// 1 byte
#define UDF_FID_ICB				20		// 16 bytes
#define UDF_FID_IMP_USE_LEN	36		// 2 bytes

//
//  UDF file characteristics in file identifier descriptor
//

#define UDF_FILECHAR_EXIST			0x01		// If 1 the user shall not see this entry
#define UDF_FILECHAR_DIR			0x02  	// If 1 this is a directory entry
#define UDF_FILECHAR_DELETED		0x04		// If 1 this entry has been deleted
#define UDF_FILECHAR_PARENT		0x08		// If 1 this entry points to the parent dir
#define UDF_FILECHAR_METADATA		0x10		// Something with stream directories

//
//  Access macros
//

#define UDF_BYTE(d)	((d)[0])
#define UDF_WORD(d)	MAKEWORD((d)[0], (d)[1])
#define UDF_DWORD(d)	MAKELONG(MAKEWORD((d)[0], (d)[1]), MAKEWORD((d)[2], (d)[3]))
#define UDF_QWORD(d)	KernelInt64(UDF_DWORD(d), UDF_DWORD(d + 4))

////////////////////////////////////////////////////////////////////
//
//  UDF File Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

UDFFile::UDFFile(UDFFileSystem * udffs) : GenericFile(udffs)
	{
	this->udffs = udffs;
	udfi = NULL;
	gcmiValid = FALSE;
	}

//
//  Destructor
//

UDFFile::~UDFFile(void)
	{
	delete udfi;
	}

//
//  Open
//

Error UDFFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericFileSystemIterator * gfsi2;
	DWORD dummy;
	Error err;

	if (accessType & FAT_WRITE)
		GNRAISE(GNR_FILE_READ_ONLY);

	GNREASSERT(GenericFile::Open(gfsi, accessType));

	GNREASSERT(gfsi->Clone(gfsi2));
	udfi = (UDFIterator *)gfsi2;

	err = udffs->GetAllocDescriptor(udfi, 0, startBlock, dummy);

	if (!IS_ERROR(err))
		err = udffs->GetCopyManagementInfo(udfi, gcmi);

	if (IS_ERROR(err))
		{
		delete udfi;
		udfi = NULL;
		}

	GNRAISE(err);
	}

//
//  Close
//

Error UDFFile::Close(void)
	{
	delete udfi;
	udfi = NULL;
	GNRAISE_OK;
	}

//
//  Get name
//

Error UDFFile::GetName(DiskItemName & name)
	{
	return udffs->GetItemName(udfi, name);
	}

//
//  Get path name
//

Error UDFFile::GetPathName(DiskItemName & name)
	{
	return udffs->GetPathName(udfi, name);
	}

//
//  Get size
//

Error UDFFile::GetSize(KernelInt64 & size)
	{
	return udffs->GetItemSize(udfi, size);
	}

//
//  Get iterator
//

Error UDFFile::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return udfi->Clone(gfsi);
	}

//
//  Build unique key
//

Error UDFFile::BuildUniqueKey(void)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get copy management mode
//

Error UDFFile::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	if (!gcmiValid)
		GNREASSERT(udffs->GetCopyManagementInfo(udfi, this->gcmi));

	gcmi = this->gcmi;
	gcmiValid = TRUE;
	GNRAISE_OK;
	}

//
//  Seek block
//

Error UDFFile::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	return udffs->SeekBlock(startBlock + block, flags, rh);
	}

//
//  Lock blocks
//

Error UDFFile::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | udffs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | udffs->GetDataSectorType();

	return udffs->LockBlocks(startBlock + block, num, blocks, flags, rh);
	}

//
//  Unlock blocks
//

Error UDFFile::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	if (headerFile)
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | udffs->GetHeaderSectorType();
	else
		flags = (flags & ~GD_SECTOR_TYPE_MASK) | udffs->GetDataSectorType();

	return udffs->UnlockBlocks(startBlock + block, num, blocks, flags, rh);
	}

////////////////////////////////////////////////////////////////////
//
//  UDF Directory Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

//
//  Constructor
//

UDFDirectory::UDFDirectory(UDFFileSystem * udffs) : GenericDirectory(udffs)
	{
	this->udffs = udffs;
	udfi = NULL;
	}

//
//  Desctructor
//

UDFDirectory::~UDFDirectory(void)
	{
	delete udfi;
	}

//
//  Open directory
//

Error UDFDirectory::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GNRAISE_OK;
	}

//
//  Close
//

Error UDFDirectory::Close(void)
	{
	GNRAISE_OK;
	}

//
//  Get name
//

Error UDFDirectory::GetName(DiskItemName & name)
	{
	return udffs->GetItemName(udfi, name);
	}

//
//  Get path name
//

Error UDFDirectory::GetPathName(DiskItemName & name)
	{
	return udffs->GetPathName(udfi, name);
	}

//
//  Return iterator pointing to this directory
//

Error UDFDirectory::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return udfi->Clone(gfsi);
	}

//
//  Create item
//

Error UDFDirectory::CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Delete item
//

Error UDFDirectory::DeleteItem(GenericFileSystemIterator * gfsi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Get number of items
//

Error UDFDirectory::GetNumberOfItems(DWORD & num)
	{
	return udffs->GetNumberOfItems(udfi, num);
	}

//
//  Find item
//

Error UDFDirectory::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * where;
	Error err;

	GNREASSERT(GetIterator(where));
	if (!IS_ERROR(err = udffs->FindItem(name, type, flags, where, gfsi)))
		delete where;
	GNRAISE(err);
	}

#endif

////////////////////////////////////////////////////////////////////
//
//  UDF File System Iterator Class
//  NOTE: This points to a UDF File Identifier Descriptor
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

UDFIterator::UDFIterator(GenericFileSystem * gfs) : GenericFileSystemIterator(gfs)
	{
	}

//
//  Destructor
//

UDFIterator::~UDFIterator(void)
	{
	}

//
//  Clone
//

Error UDFIterator::Clone(UDFIterator * & udfi) const
	{
	udfi = new UDFIterator(gfs);
	if (udfi)
		{
		udfi->dirICB = dirICB;
		udfi->dirStart = dirStart;
		udfi->dirSize = dirSize;
		udfi->entryOffset = entryOffset;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Clone
//

Error UDFIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	UDFIterator * udfi;

	GNREASSERT(Clone(udfi));
	gfsi = udfi;
	GNRAISE_OK;
	}

//
//  Comparison function
//

BOOL UDFIterator::Equals(GenericFileSystemIterator * gfsi)
	{
	return (dirStart    == ((UDFIterator*)gfsi)->dirStart) &&
			 (entryOffset == ((UDFIterator*)gfsi)->entryOffset);
	}

//
//  Go to first item in directory
//

Error	UDFIterator::GoToFirstItem(void)
	{
	return ((UDFFileSystem*)gfs)->GoToFirstItem(this);
	}

//
//  Go to next item in directory
//

Error	UDFIterator::GoToNextItem(void)
	{
	return ((UDFFileSystem*)gfs)->GoToNextItem(this);
	}

//
//  Enter subdirectory
//

Error	UDFIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	return ((UDFFileSystem*)gfs)->GoToSubDir(this, gfsi);
	}

//
//  Go to parent directory
//

Error	UDFIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	return ((UDFFileSystem*)gfs)->GoToParentDir(this, gfsi);
	}

//
//  Get path name of current directory
//

Error UDFIterator::GetPathName(DiskItemName & name)
	{
	return ((UDFFileSystem*)gfs)->GetPathName(this, name);
	}

//
//  Get type of item pointed to
//

Error UDFIterator::GetItemType(DiskItemType & type)
	{
	return ((UDFFileSystem*)gfs)->GetItemType(this, type);
	}

//
//  Get name of item pointed to
//

Error UDFIterator::GetItemName(DiskItemName & name)
	{
	return ((UDFFileSystem*)gfs)->GetItemName(this, name);
	}

//
//  Get size of item pointed to
//

Error UDFIterator::GetItemSize(KernelInt64 & size)
	{
	return ((UDFFileSystem*)gfs)->GetItemSize(this, size);
	}

//
//  Get the number of items in the directory the iterator is in
//

Error UDFIterator::GetNumberOfItems(DWORD & num)
	{
	return ((UDFFileSystem*)gfs)->GetNumberOfItems(this, num);
	}

//
//  Get the directory the iterator is currently in
//

Error UDFIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	return ((UDFFileSystem*)gfs)->GetDir(this, gd);
	}

//
//  Open item pointed to
//

Error	UDFIterator::OpenItem(DWORD fileAccessType, GenericDiskItem * & gdi)
	{
	return ((UDFFileSystem*)gfs)->OpenItem(this, fileAccessType, gdi);
	}

////////////////////////////////////////////////////////////////////
//
//  UDF File System Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

UDFFileSystem::UDFFileSystem(void) : GenericFileSystem()
	{
	cdvdVolume = NULL;
	fileSystemType = FS_UDF;
	}

//
//  Destructor
//

UDFFileSystem::~UDFFileSystem(void)
	{
	}

//
//  Initialize
//

Error UDFFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	VDAutoMutex mutex(&lock);
	RequestHandle rh;
	PhysicalDiskType type;
	DriveBlock db;
	Error err;
	int i;

	//
	//  Make sure we have the right volume type
	//

	GNREASSERT(volume->GetDiskType(type));
	if (type != PHDT_DVD_ROM)
		GNRAISE(GNR_VOLUME_INVALID);

	GNREASSERT(GenericFileSystem::Init(mountPoint, volume, profile));
	this->cdvdVolume = (CDVDVolume*)volume;

	//
	//  Read Anchor Volume Descriptor Pointer
	//  and find Primary Volume Descriptor
	//

	err = cdvdVolume->LockBlocks(256, 1, &db, DST_DVD_ROM | DAT_LOCK_AND_READ, &rh);
	if (!IS_ERROR(err) && UDF_BYTE(db.data) == 0x02)	// Make sure we have what we hope to have
		primaryVolumeDescriptor = UDF_DWORD(db.data + 20);

	cdvdVolume->UnlockBlocks(256, 1, &db, DST_DVD_ROM | DAT_UNLOCK_CLEAN, &rh);

	//
	//  Read Volume Name and compute unique id
	//

	if (!IS_ERROR(err))
		{
		err = cdvdVolume->LockBlocks(primaryVolumeDescriptor, 1, &db, DST_DVD_ROM | DAT_LOCK_AND_READ, &rh);

		if (!IS_ERROR(err))
			{
			char name[129];

			// Get volume name
			memcpy(name, db.data + UDF_PVD_VOLUME_IDENT + 1, 32-1); //+1/-1 for skipping leading 0x08 (Why is 0x08 there? No one knows...)
			name[32-1] = 0; //put the elephant in Cairo
			volumeName = name;
			volumeName = volumeName.Trim();

			// Get volume set name
			memcpy(name, db.data + UDF_PVD_VOLUME_SET_IDENT + 1, 128-1); //+1/-1 for skipping leading 0x08
			name[128-1] = 0;
			volumeSetName = name;
			volumeSetName = volumeSetName.Trim();

			// Get some more values
			volumeSetIndex = UDF_WORD(db.data + UDF_PVD_VOLUME_SET_INDEX);
			volumeSetSize = UDF_WORD(db.data + UDF_PVD_VOLUME_SET_SIZE);

			//
			// Read time stamp and compute unique id from it
			//

			for (i=0; i<8; i++)
				uniqueKey.key[i] = UDF_BYTE(db.data + UDF_PVD_RECORDING_DATE + i);
			}

		cdvdVolume->UnlockBlocks(primaryVolumeDescriptor, 1, &db, DST_DVD_ROM | DAT_UNLOCK_CLEAN, &rh);
		}

	//
	//  Read partition descriptor
	//  and initialize base for logical access
	//

	if (!IS_ERROR(err))
		{
		err = cdvdVolume->LockBlocks(primaryVolumeDescriptor + 2, 1, &db, DST_DVD_ROM | DAT_LOCK_AND_READ, &rh);

		if (!IS_ERROR(err) && UDF_BYTE(db.data))
			logicalBaseBlock = UDF_DWORD(db.data + 188);

		cdvdVolume->UnlockBlocks(primaryVolumeDescriptor + 2, 1, &db, DST_DVD_ROM | DAT_UNLOCK_CLEAN, &rh);
		}

	//
	//  Get root ICB
	//

	if (!IS_ERROR(err))
		{
		err = cdvdVolume->LockBlocks(logicalBaseBlock, 1, &db, DST_DVD_ROM | DAT_LOCK_AND_READ, &rh);

		if (!IS_ERROR(err))
			rootICB = UDF_DWORD(db.data + 404);

		cdvdVolume->UnlockBlocks(logicalBaseBlock, 1, &db, DST_DVD_ROM | DAT_UNLOCK_CLEAN, &rh);
		}

	//
	//  Cleanup
	//

	if (IS_ERROR(err))
		this->cdvdVolume = NULL;

	GNRAISE(err);
	}

//
//  Create iterator
//

Error UDFFileSystem::CreateIterator(UDFIterator * & udfi, DWORD dirICB)
	{
	DWORD extAttrLen;
	Error err = GNR_OK;

	udfi = new UDFIterator(this);

	if (!udfi)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	udfi->dirICB = dirICB;
	GNREASSERT(ReadDWord(dirICB, UDF_ICB_INFO_LEN, udfi->dirSize));
	GNREASSERT(ReadDWord(dirICB, UDF_ICB_EXT_ATTR_LEN, extAttrLen));

	GNREASSERT(ReadDWord(dirICB, UDF_ICB_EXT_ATTR + extAttrLen + 4, udfi->dirStart));
	udfi->dirStart &= 0x3fffffff;	// Bits 30 and 31 characterize extent type (ignored)

	udfi->entryOffset = 0;

	if (IS_ERROR(err = GoToFirstItem(udfi)))
		{
		delete udfi;
		udfi = NULL;
		}

	GNRAISE(err);
	}

//
//  Create iterator
//

Error UDFFileSystem::CreateIterator(UDFIterator * & udfi)
	{
	return CreateIterator(udfi, rootICB);
	}

//
//  Create iterator
//

Error UDFFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi, DWORD dirICB)
	{
	UDFIterator * udfi;		// Because of stupid type cast problems
	Error err;

	err = CreateIterator(udfi, dirICB);
	gfsi = udfi;
	GNRAISE(err);
	}

//
//  Create iterator
//

Error UDFFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	UDFIterator * udfi;		// Because of stupid type cast problems
	Error err;

	err = CreateIterator(udfi, rootICB);
	gfsi = udfi;
	GNRAISE(err);
	}

//
//  Read BYTE
//

Error UDFFileSystem::ReadByte(DWORD block, DWORD offset, BYTE & b)
	{
	Error err = GNR_OK;
	block += offset / blockSize;
	offset %= blockSize;

	if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
		b = UDF_BYTE(driveBlock.data + offset);
	UnlockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
	GNRAISE(err);
	}

//
//  Read WORD
//

Error UDFFileSystem::ReadWord(DWORD block, DWORD offset, WORD & w)
	{
	Error err = GNR_OK;
	block += offset / blockSize;
	offset %= blockSize;

	if (offset + 2 < blockSize)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
			w = UDF_WORD(driveBlock.data + offset);
		UnlockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
		GNRAISE(err);
		}
	else
		{
		GNREASSERT(ReadByte(block, offset, ((BYTE*)&w)[0]));
		return ReadByte(block, offset + 1, ((BYTE*)&w)[1]);
		}
	}

//
//  Read DWORD
//

Error UDFFileSystem::ReadDWord(DWORD block, DWORD offset, DWORD & d)
	{
	Error err = GNR_OK;
	block += offset / blockSize;
	offset %= blockSize;

	if (offset + 4 < blockSize)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
			d = UDF_DWORD(driveBlock.data + offset);
		UnlockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
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
//  Read QWORD
//

Error UDFFileSystem::ReadQWord(DWORD block, DWORD offset, KernelInt64 & q)
	{
	Error err = GNR_OK;
	block += offset / blockSize;
	offset %= blockSize;

	if (offset + 8 < blockSize)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_LOCK_AND_READ | DAF_CACHED, &rh)))
			q = UDF_QWORD(driveBlock.data + offset);
		UnlockBlocks(block, 1, &driveBlock, DST_DVD_ROM | DAT_UNLOCK_CLEAN | DAF_CACHED, &rh);
		GNRAISE(err);
		}
	else
		{
		GNREASSERT(ReadByte(block, offset, ((BYTE*)&q)[0]));
		GNREASSERT(ReadByte(block, offset + 1, ((BYTE*)&q)[1]));
		GNREASSERT(ReadByte(block, offset + 2, ((BYTE*)&q)[2]));
		GNREASSERT(ReadByte(block, offset + 3, ((BYTE*)&q)[3]));
		GNREASSERT(ReadByte(block, offset + 4, ((BYTE*)&q)[4]));
		GNREASSERT(ReadByte(block, offset + 5, ((BYTE*)&q)[5]));
		GNREASSERT(ReadByte(block, offset + 6, ((BYTE*)&q)[6]));
		return ReadByte(block, offset + 7, ((BYTE*)&q)[7]);
		}
	}

//
//	 Create file
//

Error UDFFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	gf = new UDFFile(this);
	if (gf)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error UDFFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
#if GFS_ENABLE_DIRECTORIES
	gd = new UDFDirectory(this);
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

Error UDFFileSystem::GoToFirstItem(class UDFIterator * udfi)
	{
	VDAutoMutex mutex(&lock);
	BYTE fileChar;

	udfi->entryOffset = 0;
	if (udfi->entryOffset < udfi->dirSize)
		{
		//
		//  Skip parent dir entry
		//

		GNREASSERT(ReadByte(udfi->dirStart, UDF_FID_FILE_CHAR, fileChar));
		if (fileChar & UDF_FILECHAR_PARENT)
			return GoToNextItem(udfi);
		else
			GNRAISE_OK;
		}
	else
		GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Go to next item
//

Error UDFFileSystem::GoToNextItem(class UDFIterator * udfi)
	{
	VDAutoMutex mutex(&lock);
	BYTE identLen;
	WORD impUseLen;
	BYTE fileChar;
	WORD tag;

	GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_IDENT_LEN, identLen));
	GNREASSERT(ReadWord(udfi->dirStart, udfi->entryOffset + UDF_FID_IMP_USE_LEN, impUseLen));

	udfi->entryOffset += 38 + identLen + impUseLen;
	udfi->entryOffset = (udfi->entryOffset + 3) & 0xfffffffc;		// Aligned on 4 bytes

	if (udfi->entryOffset + 1 < udfi->dirSize)
		{
		GNREASSERT(ReadWord(udfi->dirStart, udfi->entryOffset, tag));
		while (tag != UDF_TAG_FILE_IDENT && udfi->entryOffset + 2 < udfi->dirSize)
			{
			udfi->entryOffset++;
			GNREASSERT(ReadWord(udfi->dirStart, udfi->entryOffset, tag));
			}
		if (tag == UDF_TAG_FILE_IDENT)
			{
			//
			//  Skip parent dir entry
			//

			GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_FILE_CHAR, fileChar));
			if (fileChar & UDF_FILECHAR_PARENT)
				return GoToNextItem(udfi);
			else
				GNRAISE_OK;
			}
		else
			GNRAISE(GNR_ITEM_NOT_FOUND);
		}
	else
		GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Go to sub dir
//

Error UDFFileSystem::GoToSubDir(class UDFIterator * udfi, class GenericFileSystemIterator * & gfsi)
	{
	VDAutoMutex mutex(&lock);
	DWORD subICB;
	BYTE fileChar;

	GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_FILE_CHAR, fileChar));
	if (!(fileChar & UDF_FILECHAR_DIR))
		GNRAISE(GNR_NOT_A_DIRECTORY);

	GNREASSERT(ReadDWord(udfi->dirStart, udfi->entryOffset + UDF_FID_ICB + 4, subICB));
	return CreateIterator(gfsi, subICB);
	}

//
//  Go to parent dir
//

Error UDFFileSystem::GoToParentDir(class UDFIterator * udfi, class GenericFileSystemIterator * & gfsi)
	{
	VDAutoMutex mutex(&lock);
	BYTE identLen;
	WORD impUseLen;
	BYTE fileChar;
	DWORD offset = 0;
	DWORD subICB;

	if (udfi->dirICB == rootICB)
		return mountPoint->Clone(gfsi);
	else
		{
		//
		//  Search for parent entry in dir
		//

		GNREASSERT(ReadByte(udfi->dirStart, UDF_FID_FILE_CHAR, fileChar));
		while (!(fileChar & UDF_FILECHAR_PARENT))
			{
			GNREASSERT(ReadByte(udfi->dirStart, offset + UDF_FID_IDENT_LEN, identLen));
			GNREASSERT(ReadWord(udfi->dirStart, offset + UDF_FID_IMP_USE_LEN, impUseLen));
			offset += 38 + identLen + impUseLen;
			offset = (offset + 3) & 4;		// Aligned on 4 bytes
			if (offset >= udfi->dirSize)
				break;
			GNREASSERT(ReadByte(udfi->dirStart, offset + UDF_FID_FILE_CHAR, fileChar));
			}

		//
		//  Make sure we actually found the parent entry
		//  Missing it should never happen since parent entry is mandatory, but you never know
		//

		if (offset > udfi->dirSize)
			return GNR_ITEM_NOT_FOUND;

		GNREASSERT(ReadDWord(udfi->dirStart, offset + UDF_FID_ICB + 4, subICB));
		return CreateIterator(gfsi, subICB);
		}
	}

//
//  Get path name
//

Error UDFFileSystem::GetPathName(UDFIterator * udfi, DiskItemName & name)
	{
	VDAutoMutex mutex(&lock);
	Error err = GNR_OK;
	DiskItemName pname;	// used to take a look into the cache...
	name = DiskItemName("");
	UDFIterator * imudfi;
	UDFIterator * parenti = NULL;
	DWORD extAttrLen = 0;
	DWORD rootDirStart = 0;

	// get directory start from rootICB;
	GNREASSERT(ReadDWord(this->rootICB, UDF_ICB_EXT_ATTR_LEN, extAttrLen));

	GNREASSERT(ReadDWord(this->rootICB, UDF_ICB_EXT_ATTR + extAttrLen + 4, rootDirStart));
	rootDirStart &= 0x3fffffff;	// Bits 30 and 31 characterize extent type (ignored)

	GNREASSERT(udfi->Clone(imudfi));

	while ( (imudfi->dirStart != rootDirStart) && (IS_ERROR(cache.Get(imudfi->dirStart, pname))) )
		{
		if (IS_ERROR(err = GoToParentDir(imudfi, (GenericFileSystemIterator * &) parenti)))
			{
			delete imudfi;
			GNRAISE(err);
			}
		if (IS_ERROR(parenti->GoToFirstItem()))
			{
			delete parenti;
			delete imudfi;
			GNRAISE(err);
			}

		// search
		bool found = false;
		while (!found)
			{
			DiskItemType type;
			this->GetItemType(parenti, type);
			if (type == DIT_DIR)
				{
				UDFIterator * udfsubi = NULL;
				if (IS_ERROR(err = parenti->GoToSubDir((GenericFileSystemIterator *&) udfsubi)))
					{
					delete parenti;
					delete udfsubi;
					delete imudfi;
					GNRAISE(err);
					}
				if (udfsubi->dirStart == imudfi->dirStart)
					{

					DiskItemName rname;
					parenti->GetItemName(rname);
					name = rname + "/" + name;
					found = true;
					}
				delete udfsubi;
				}

			if (!found)
				if (IS_ERROR(GoToNextItem(parenti)))
					{
					delete parenti;
					delete imudfi;
					GNRAISE(GNR_ITEM_NOT_FOUND);
					}
			}
		delete imudfi;
		imudfi = parenti;
		}

	// found it in cache?
	if (imudfi->dirStart != rootDirStart)
		{
		name = pname + "/" + name;
		}
	// go through mount point...
	else if (this->mountPoint)
		{
		DiskItemName mpName;
		DiskItemName mpPath;
		this->mountPoint->GetPathName(mpPath);
		this->mountPoint->GetItemName(mpName);
		name = mpPath + (DiskItemName)"/" + mpName + name;
		cache.Add(name, udfi->dirStart);
		}
	// no mount point? Should not be, but do not panic. Claim to be root...
	else
		{
		name = (DiskItemName)"/" + name;
		}

	delete imudfi;
	GNRAISE_OK;
	}


//
//  Get item type
//

Error UDFFileSystem::GetItemType(UDFIterator * udfi, DiskItemType & type)
	{
	VDAutoMutex mutex(&lock);
	BYTE fileChar;

	GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_FILE_CHAR, fileChar));

	if (fileChar & UDF_FILECHAR_DIR)
		type = DIT_DIR;
	else
		type = DIT_FILE;

	GNRAISE_OK;
	}

//
//  Get item name
//

Error UDFFileSystem::GetItemName(UDFIterator * udfi, DiskItemName & name)
	{
	VDAutoMutex mutex(&lock);
	BYTE buffer[256];
	BYTE identLen;
	WORD impUseLen;
	int i;

	GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_IDENT_LEN, identLen));
	if (identLen < 2)
		{
		name = DiskItemName();
		GNRAISE_OK;
		}
	GNREASSERT(ReadWord(udfi->dirStart, udfi->entryOffset + UDF_FID_IMP_USE_LEN, impUseLen));

	//
	//  Now read name. Note that leading 0x08 byte is skipped
	//

	for (i=0; i<identLen; i++)
		GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + 38 + impUseLen + 1 + i, buffer[i]));

	name = DiskItemName((char*)buffer, identLen - 1);
	GNRAISE_OK;
	}

//
//  Get item size
//

Error UDFFileSystem::GetItemSize(UDFIterator * udfi, KernelInt64 & size)
	{
	VDAutoMutex mutex(&lock);
	BYTE fileChar;
	DWORD entryICB;

	GNREASSERT(ReadByte(udfi->dirStart, udfi->entryOffset + UDF_FID_FILE_CHAR, fileChar));

	if (fileChar & UDF_FILECHAR_DIR)
		{
		size = 0;
		GNRAISE_OK;
		}

	GNREASSERT(ReadDWord(udfi->dirStart, udfi->entryOffset + UDF_FID_ICB + 4, entryICB));
	return ReadQWord(entryICB, UDF_ICB_INFO_LEN, size);
	}

//
//  Get the directory the iterator is currently in
//

Error UDFFileSystem::GetDir(UDFIterator * udfi, GenericDirectory * & gd)
	{
	VDAutoMutex mutex(&lock);

	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get the number of items in the dir the iterator is currently in
//

Error UDFFileSystem::GetNumberOfItems(UDFIterator * udfi, DWORD & num)
	{
	VDAutoMutex mutex(&lock);
	DWORD offset = 0;
	BYTE identLen;
	WORD impUseLen;
	BYTE fileChar;

	//
	//  Search for parent entry in dir
	//

	num = 0;
	while (offset < udfi->dirSize)
		{
		GNREASSERT(ReadByte(udfi->dirStart, offset + UDF_FID_FILE_CHAR, fileChar));

		//
		//  Ignore parent entry
		//

		if (!(fileChar & UDF_FILECHAR_PARENT))
			num++;

		//
		//  Go to next entry
		//

		GNREASSERT(ReadByte(udfi->dirStart, offset + UDF_FID_IDENT_LEN, identLen));
		GNREASSERT(ReadWord(udfi->dirStart, offset + UDF_FID_IMP_USE_LEN, impUseLen));

		offset += 38 + identLen + impUseLen;
		offset = (offset + 3) & 0xfffffff0;		// Aligned on 4 bytes
		}

	GNRAISE_OK;
	}

//
//  Build unique key
//

Error UDFFileSystem::BuildUniqueKey(void)
	{
	GNRAISE_OK;		// We already do this in Init()
	}

//
//  Get allocation descriptor of file (only short ones here)
//  length is given in bytes
//

Error UDFFileSystem::GetAllocDescriptor(UDFIterator * udfi, WORD index, DWORD & start, DWORD & length)
	{
	DWORD extAttrLen;
	KernelInt64 allocDesc;
	DWORD entryICB;

	//
	//  Find ICB belonging to this item
	//

	GNREASSERT(ReadDWord(udfi->dirStart, udfi->entryOffset + UDF_FID_ICB + 4, entryICB));

	//
	//  Now read the allocation descriptor
	//

	index <<= 3;	// Size of alloc descriptor is 8 for the short alloc descs used on DVD
	GNREASSERT(ReadDWord(entryICB, UDF_ICB_EXT_ATTR_LEN, extAttrLen));
	GNREASSERT(ReadQWord(entryICB, UDF_ICB_EXT_ATTR + extAttrLen, allocDesc));
	length = allocDesc.Lower() & 0x3fffffff;
	start = allocDesc.Upper();
	GNRAISE_OK;
	}

//
//  Get copy management mode of item pointed to by iterator (DVD only)
//

Error UDFFileSystem::GetCopyManagementInfo(UDFIterator * udfi, GenericCopyManagementInfo & gcmi)
	{
	DWORD extAttrLen;
	DWORD help1, help2;
	DWORD impUseOffset;
	DWORD impUseSize;
	char cgmsStr[] = "*UDF DVD CGMS Info";
	DWORD cgmsStrLen = strlen(cgmsStr);
	BYTE cgms;
	WORD tag;
	DWORD i;
	BYTE b;
	BOOL found;
	DWORD entryICB;

	//
	//  Get item's ICB
	//

	gcmi = GCMI_COPY_FORBIDDEN;
	GNREASSERT(ReadDWord(udfi->dirStart, udfi->entryOffset + UDF_FID_ICB + 4, entryICB));

	//
	//  Get copyright generation management system (CGMS) information
	//  from extended attributes
	//

	GNREASSERT(ReadDWord(entryICB, UDF_ICB_EXT_ATTR_LEN, extAttrLen));
	if (!extAttrLen)
		GNRAISE_OK;

	GNREASSERT(ReadWord(entryICB, UDF_ICB_EXT_ATTR, tag));
	if (tag != UDF_TAG_EXT_ATTR_HEADER)
		GNRAISE_OK;

	GNREASSERT(ReadDWord(entryICB, UDF_ICB_EXT_ATTR + 16, impUseOffset));
	GNREASSERT(ReadDWord(entryICB, UDF_ICB_EXT_ATTR + 20, impUseSize));
	impUseSize -= impUseOffset;
	impUseOffset += UDF_ICB_EXT_ATTR;
	found = FALSE;

	while (impUseSize && !found)
		{
		//
		//  Check implementation use attribute type and subtype
		//

		GNREASSERT(ReadDWord(entryICB, impUseOffset, help1));
		GNREASSERT(ReadDWord(entryICB, impUseOffset + 4, help2));
		if (help1 == 2048 && help2 == 1)	// Tag for CGMS
			{
			//
			//  Now compare id string
			//

			for (i=0; i<cgmsStrLen; i++)
				{
				GNREASSERT(ReadByte(entryICB, impUseOffset + 17 + i, b));
				if (b != cgmsStr[i])
					break;
				}

			if (i == cgmsStrLen)
				found = TRUE;
			}

		//
		//  If found extract CGMS information, else advance to next attribute
		//

		if (found)
			{
			//
			//  Read copyright info
			//

			GNREASSERT(ReadByte(entryICB, impUseOffset + 48 + 2, cgms));
			switch (cgms & 0x30)
				{
				case 0x00:
				case 0x10:
					gcmi = GCMI_NO_RESTRICTION;
					break;
				case 0x20:
					gcmi = GCMI_ONE_GENERATION;
					break;
				case 0x30:
					gcmi = GCMI_COPY_FORBIDDEN;
					break;
				}

			GNRAISE_OK;
			}
		else
			{
			GNREASSERT(ReadDWord(entryICB, impUseOffset + 8, help1));
			impUseOffset += help1;
			impUseSize -= help1;
			}
		}

	GNRAISE_OK;
	}
