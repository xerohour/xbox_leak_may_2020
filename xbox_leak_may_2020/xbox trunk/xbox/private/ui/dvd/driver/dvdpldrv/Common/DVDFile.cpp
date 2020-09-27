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
//  DVD File Classes
//
////////////////////////////////////////////////////////////////////

#include "DVDFile.h"

////////////////////////////////////////////////////////////////////
//
//  DVD Header File
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDHeaderFile::DVDHeaderFile(DVDFileSystem * dvdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher)
	: GenericFile(dvdfs)
	, EventSender(pEventDispatcher)
	{
	this->baseFS = baseFS;
	file = NULL;
	}

//
//  Destuctor
//

DVDHeaderFile::~DVDHeaderFile(void)
	{
	if (file)
		{
		file->Close();
		delete file;
		}
	}

//
//  Open file
//

Error DVDHeaderFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericDiskItem * gdi = NULL;
	RequestHandle rh;
	KernelInt64 size;
	BYTE b;
	int i;

	//
	//  Open the represented file
	//

	GNREASSERT(GenericFile::Open(gfsi, accessType));
	GNREASSERT(baseFS->OpenItem(gfsi, accessType, gdi));
	file = (GenericFile*)gdi;
	GNREASSERT(file->GetSize(size));

	//
	//  Initialize cache (forward because of streaming)
	//

	for (i=0; i<(int)(min(size, 8 * DVD_BLOCK_SIZE).ToDWORD()); i += DVD_BLOCK_SIZE)
		GNREASSERT(ReadByte(i, b, &rh));

	//
	//  Put cache data in LRU order, buffer 0 first)
	//

	for (i=(int)(min(size, 8 * DVD_BLOCK_SIZE).ToDWORD() - DVD_BLOCK_SIZE); i>=0; i -= DVD_BLOCK_SIZE)
		GNREASSERT(ReadByte(i, b, &rh));

	GNRAISE_OK;
	}

//
//  Close file
//

Error DVDHeaderFile::Close(void)
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
//  Get copy management mode
//

Error DVDHeaderFile::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	GNRAISE(file->GetCopyManagementInfo(gcmi));
	}

//
//  Get name
//

Error DVDHeaderFile::GetName(DiskItemName & name)
	{
	return file->GetName(name);
	}

//
//  Get path name
//

Error DVDHeaderFile::GetPathName(DiskItemName & name)
	{
	return file->GetPathName(name);
	}

//
//  Get size
//

Error DVDHeaderFile::GetSize(KernelInt64 & size)
	{
	return file->GetSize(size);
	}

//
//  Get iterator
//

Error DVDHeaderFile::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return file->GetIterator(gfsi);
	}

//
//  Build Unique Key
//

Error DVDHeaderFile::BuildUniqueKey(void)
	{
	VDAutoMutex mutex(&lock);
	RequestHandle rh;
	DriveBlock db;
	DWORD block;
	DWORD i;
	KernelInt64 size;
	DWORD headerSize;
	DWORD dataSize;
	Error err;

	GNREASSERT(file->GetSize(size));
	headerSize = baseFS->GetHeaderHeaderSize();
	dataSize = baseFS->GetHeaderDataSize();

	for (i=0; i<8; i++)
		uniqueKey.key[i] = 0x00;

	//
	//  Read all the blocks and use them for unique key
	//

	for (block=0; block < (size / dataSize).ToDWORD(); block++)
		{
		if (!IS_ERROR(err = LockBlocks(block, 1, &db, baseFS->GetHeaderSectorType() | DAF_CACHED | DAT_LOCK_AND_READ, &rh)))
			{
			for (i=headerSize; i<headerSize + dataSize; i++)
				{
				uniqueKey.key[0] += db.data[i];
				uniqueKey.key[1] += uniqueKey.key[0] + 0x32;
				uniqueKey.key[2] += uniqueKey.key[1] + 0x47;
				uniqueKey.key[3] += uniqueKey.key[2] + 0x21;
				uniqueKey.key[4] += uniqueKey.key[3] + 0x46;
				uniqueKey.key[5] += uniqueKey.key[4] + 0x57;
				uniqueKey.key[6] += uniqueKey.key[5] + 0x94;
				uniqueKey.key[7] += uniqueKey.key[6] + 0x25;
				}
			}

		UnlockBlocks(block, 1, &db, baseFS->GetHeaderSectorType() | DAF_CACHED | DAT_UNLOCK_CLEAN, &rh);
		GNREASSERT(err);
		}

	GNRAISE_OK;
	}

//
//  Test if file is encrypted
//

Error DVDHeaderFile::IsEncrypted(BOOL & enc)
	{
	return ((DVDFileSystem*)gfs)->DVDIsEncrypted(enc);
	}

//
//  Do authentication command
//

Error DVDHeaderFile::DoAuthenticationCommand(DVDAuthenticationCommand com, BYTE * key)
	{
	DWORD block;

	GNREASSERT(file->GetStartBlock(block));
	return ((DVDFileSystem*)gfs)->DoAuthenticationCommand(com, block, key);
	}

//
//  Read Byte
//

Error DVDHeaderFile::ReadByte(DWORD position, BYTE & b, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return file->ReadByte(position, b, DAF_CACHED, rh);
	}

//
//  Read Word
//

Error DVDHeaderFile::ReadWord(DWORD position, WORD & w, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return file->ReadWord(position, w, DAF_CACHED, rh);
	}

//
//  Read DWord
//

Error DVDHeaderFile::ReadDWord(DWORD position, DWORD & d, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	return file->ReadDWord(position, d, DAF_CACHED, rh);
	}

//
//  Read Byte Sequentially
//

Error DVDHeaderFile::ReadByteSeq(DWORD & position, BYTE & b, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	GNREASSERT(file->ReadByte(position, b, DAF_CACHED, rh));
	position++;
	GNRAISE_OK;
	}

//
//  Read Word Sequentially
//

Error DVDHeaderFile::ReadWordSeq(DWORD & position, WORD & w, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	GNREASSERT(file->ReadWord(position, w, DAF_CACHED, rh));
	position += 2;
	GNRAISE_OK;
	}

//
//  Read DWord Sequentially
//

Error DVDHeaderFile::ReadDWordSeq(DWORD & position, DWORD & d, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);

	GNREASSERT(file->ReadDWord(position, d, DAF_CACHED, rh));
	position += 4;
	GNRAISE_OK;
	}

//
//  Seek block
//

Error DVDHeaderFile::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	return file->SeekBlock(block, flags, rh);
	}

//
//  Lock blocks
//

Error DVDHeaderFile::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	Error err;
	err = file->LockBlocks(block, num, blocks, flags, rh);
	//DP("Lock %08lx %08lx %08lx", block, num, err);
	if (err == GNR_FILE_READ_ERROR)
		SendEvent(DNE_READ_ERROR, 0);
	GNRAISE(err);
	}

//
//  Unlock blocks
//

Error DVDHeaderFile::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	Error err = file->UnlockBlocks(block, num, blocks, flags, rh);
//	DP("Unlock %08lx %08lx %08lx", block, num, err);
	GNRAISE(err);
	}


//
//  Flush
//

Error DVDHeaderFile::Flush(void)
	{
	return file->Flush();
	}

//
//  Terminate request
//

Error DVDHeaderFile::TerminateRequest(RequestHandle * rh)
	{
	return file->TerminateRequest(rh);
	}

////////////////////////////////////////////////////////////////////
//
//  File node class
//
////////////////////////////////////////////////////////////////////

//
//  Class declaration
//

class ChainFileNode : public Node
	{
	public:
		ChainFileNode(void);
		~ChainFileNode(void);

		GenericFile * gf;
		KernelInt64 size;
		DWORD numBlocks;
	};

//
//  Constructor
//

ChainFileNode::ChainFileNode(void)
	{
	gf = NULL;
	size = 0;
	numBlocks = 0;
	}

//
//  Destructor
//

ChainFileNode::~ChainFileNode(void)
	{
	if (gf)
		{
		gf->Close();
		delete gf;
		}
	}

////////////////////////////////////////////////////////////////////
//
//  DVD Data File
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDDataFile::DVDDataFile(DVDFileSystem * dvdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher)
	: GenericFile(dvdfs)
	, EventSender(pEventDispatcher)
	{
	this->baseFS = baseFS;
	}

//
//  Destructor
//

DVDDataFile::~DVDDataFile(void)
	{
	RemoveFiles();
	}

//
//  Open file
//

Error DVDDataFile::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericFileSystemIterator * gfsi2;
	DiskItemName name;
	Error err;

	//
	//  So far this is read only
	//

	if (accessType & FAT_WRITE)
		GNRAISE(GNR_FILE_READ_ONLY);

	//
	//  Initialize misc. variables
	//

	size = 0;
	fileOffset = 0;
	headerSize = baseFS->GetHeaderHeaderSize();
	dataSize = baseFS->GetHeaderDataSize();

	//
	//  Open this one and add the first file to the list
	//

	GNREASSERT(GenericFile::Open(gfsi, accessType | FAT_HEADER));
	GNREASSERT(AppendFile(gfsi, accessType));
	lastFile.node = (ChainFileNode*)files.First();
	lastFile.relativeStartBlock = 0x0;

	//
	//  Append more files if we open as chain file
	//

	if (accessType & FAT_CHAIN)
		{
		GNREASSERT(gfsi->GetItemName(name));
		name[name.Length() - 5]++;

		while (!IS_ERROR(err = baseFS->FindItem(name, DIT_FILE, FIF_NONE, gfsi, gfsi2)))
			{
			err = AppendFile(gfsi2, accessType);
			delete gfsi2;

			if (IS_ERROR(err))
				break;
			name[name.Length() - 5]++;		// For the moment only one character changes (i.e. xxx_1.vob, xxx_2.vob, etc.)
			}

		//
		//  If there is no more file, that's ok
		//

		if (err != GNR_OK && err != GNR_ITEM_NOT_FOUND)
			GNRAISE(err);
		}

	//
	//  Some more things to init
	//

	GNREASSERT(FindVOBStart());
	GNREASSERT(FindFirstEncryptedBlock());

	GNRAISE_OK;
	}

//
//  Close file
//

Error DVDDataFile::Close(void)
	{
	return RemoveFiles();
	}

//
//  Append file
//

Error DVDDataFile::AppendFile(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
	GenericDiskItem * gdi;
	ChainFileNode * node;
	Error err;

	//
	//  Get new node
	//

	node = new ChainFileNode();
	if (!node)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Fill node with data
	//

	err = gfsi->OpenItem(accessType | FAT_HEADER, gdi);
	if (!IS_ERROR(err))
		{
		node->gf = (GenericFile*)gdi;
		if (!IS_ERROR(err = gfsi->GetItemSize(node->size)))
			{
			node->numBlocks = (node->size / dataSize).ToDWORD();
			size += node->size;
			}
		}

	//
	//  Insert node into file list
	//

	if (!IS_ERROR(err))
		files.InsertLast(node);
	else
		delete node;

	GNRAISE(err);
	}

//
//  Remove files
//

Error DVDDataFile::RemoveFiles(void)
	{
	ChainFileNode * node;

	while (files.Num())
		{
		node = (ChainFileNode*)files.Pop();
		delete node;
		}

	GNRAISE_OK;
	}

//
//  Get pointer to file containing a certain block
//

Error DVDDataFile::GetFile(DWORD block, GenericFile * & file)
	{
	DWORD dummy1, dummy2;

	return GetFile(block, 1, file, dummy1, dummy2);
	}

//
//  Get pointer to file containing a certain block and the number of blocks available after that block in the file
//

Error DVDDataFile::GetFile(DWORD block, DWORD num, GenericFile * & file, DWORD & relativeStart, DWORD & avail)
	{
	DWORD currentBlock = 0;
	ChainFileNode * node;

	//
	//  Let's see if last file requested will do the job
	//

	if (lastFile.relativeStartBlock <= block && lastFile.relativeStartBlock + lastFile.node->numBlocks > block)
		{
		node = lastFile.node;
		relativeStart = lastFile.relativeStartBlock;

		file = node->gf;
		avail = node->numBlocks - (block - relativeStart);
		if (avail > num)
			avail = num;
		GNRAISE_OK;
		}
	else
		{
		//
		//  Look for the file containing the requested block
		//

		ITERATE_UNTIL(node, &files, currentBlock + node->numBlocks > block)
			currentBlock += node->numBlocks;
		ITERATE_END;

		//
		//  See if we found the file and return it if so
		//

		if (node && currentBlock + node->numBlocks > block)
			{
			file = node->gf;
			avail = node->numBlocks - (block - currentBlock);
			if (avail > num)
				avail = num;

			lastFile.node = node;
			lastFile.relativeStartBlock = currentBlock;
			relativeStart = currentBlock;

			GNRAISE_OK;
			}
		else
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		}
	}

//
//  Find VOB start
//  BUG fix for Madonna "Truth or Dare"
//  This title has an offset into the "vob" files.
//

Error DVDDataFile::FindVOBStart(void)
	{
	RequestHandle rh;
	DriveBlock driveBlock;
	Error err = GNR_OBJECT_NOT_FOUND;
	DWORD offset = 0;
	DWORD num = (size / dataSize).ToDWORD();
	if (num > 16) num = 16;

	while (offset < num && err == GNR_OBJECT_NOT_FOUND)
		{
		if (!IS_ERROR(err = LockBlocks(offset, 1, &driveBlock, DAT_LOCK_AND_READ, &rh)))
			{
			if (driveBlock.data[0]  == 0x00 && driveBlock.data[1]  == 0x00 &&
				 driveBlock.data[2]  == 0x01 && driveBlock.data[3]  == 0xba &&
				 driveBlock.data[14] == 0x00 && driveBlock.data[15] == 0x00 &&
				 driveBlock.data[16] == 0x01 && driveBlock.data[17] == 0xbb)
				{
				SetFileOffset(fileOffset);
				err = GNR_OK;
				}
			else
				err = GNR_OBJECT_NOT_FOUND;
			}

		UnlockBlocks(offset, 1, &driveBlock, DAT_UNLOCK_CLEAN, &rh);
		offset++;
		}

	if (offset == num)
		GNRAISE_OK;
	else
		GNRAISE(err);
	}

//
//  Find first encrypted block
//

Error DVDDataFile::FindFirstEncryptedBlock(void)
	{
	RequestHandle rh;
	BYTE b;
	DWORD num;

	num = (size / (KernelInt64)dataSize).ToDWORD();
	if (num > 16) num = 16;

	firstEncryptedBlock = 1;

	while (firstEncryptedBlock < num)
		{
		// If the read attempt fails because the sector is encrypted and cannot be
		// accessed, firstEncryptedBlock already has the correct value.
		GNREASSERT(ReadByte(firstEncryptedBlock * dataSize + 17, b, &rh));
		if ((b & 0xf8) == 0xe0 || ((b & 0xf8) == 0xc0) || (b == 0xbd))
			{
			GNREASSERT(ReadByte(firstEncryptedBlock * dataSize + 20, b, &rh));
			if ((b & 0x30) != 0)
				GNRAISE_OK;
			}

		firstEncryptedBlock++;
		}

	firstEncryptedBlock = 0;

	GNRAISE_OK;
	}

//
//  Get name
//

Error DVDDataFile::GetName(DiskItemName & name)
	{
	GenericFile * file;

	GNREASSERT(GetFile(0, file));
	return file->GetName(name);
	}

//
//  Get path name
//

Error DVDDataFile::GetPathName(DiskItemName & name)
	{
	GenericFile * file;

	GNREASSERT(GetFile(0, file));
	return file->GetPathName(name);
	}

//
//  Get size
//

Error DVDDataFile::GetSize(KernelInt64 & size)
	{
	size = this->size;
	GNRAISE_OK;
	}

//
//  Get iterator
//

Error DVDDataFile::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	GenericFile * file;

	GNREASSERT(GetFile(0, file));
	return file->GetIterator(gfsi);
	}

//
//  Build unique key
//

Error DVDDataFile::BuildUniqueKey(void)
	{
	int i;

	for (i=0; i<8; i++)
		uniqueKey.key[i] = 0x00;

	GNRAISE_OK;
	}

//
//  Seek block
//

Error DVDDataFile::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	GenericFile * file;

	GNREASSERT(GetFile(block, file));

	return file->SeekBlock(block, flags, rh);
	}

//
//  Lock blocks
//

Error DVDDataFile::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GenericFile * file;
	DWORD relativeStart;
	DWORD avail;
	DWORD i;

	while (num)
		{
		Error err;
		GNREASSERT(GetFile(block, num, file, relativeStart, avail));
		err = file->LockBlocks(block - relativeStart, avail, blocks, flags | DAF_STREAMING, rh);
		for (i=0; i<avail; i++)
			blocks[i].data += headerSize;

		if (err == GNR_FILE_READ_ERROR)
			SendEvent(DNE_READ_ERROR, 0);

		GNREASSERT(err);

		num -= avail;
		block += avail;
		blocks += avail;
		}

	GNRAISE_OK;
	}

//
//  Unlock blocks
//

Error DVDDataFile::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GenericFile * file;
	DWORD relativeStart;
	DWORD avail;

	while (num)
		{
		GNREASSERT(GetFile(block, num, file, relativeStart, avail));
		GNREASSERT(file->UnlockBlocks(block - relativeStart, avail, blocks, flags | DAF_STREAMING, rh));
		num -= avail;
		block += avail;
		blocks += avail;
		}

	GNRAISE_OK;
	}

//
//  Read byte
//

Error DVDDataFile::ReadByte(KernelInt64 position, BYTE & b, RequestHandle * rh)
	{
	GenericFile * file;
	DWORD block;

	block = (position / dataSize).ToDWORD();
	GNREASSERT(GetFile(block, file));

	return file->ReadBytes(position, 1, &b, DAF_STREAMING, rh);
	}

//
//  Test if file is encrypted
//

Error DVDDataFile::IsEncrypted(BOOL & enc)
	{
	GNREASSERT(((DVDFileSystem*)gfs)->DVDIsEncrypted(enc));
	enc = enc && firstEncryptedBlock != 0;
	GNRAISE_OK;
	}

//
//  Do authentication command
//

Error DVDDataFile::DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD & block, BYTE * key, int retry)
	{
	GenericFile * file;

	GNREASSERT(GetFile(0, file));
	GNREASSERT(file->GetStartBlock(block));

	if (com == DAC_READ_TITLE_KEY)
		block += firstEncryptedBlock + retry;

	return ((DVDFileSystem*)gfs)->DoAuthenticationCommand(com, block, key);
	}

//
//  Get copy management mode
//

Error DVDDataFile::GetCopyManagementInfo(GenericCopyManagementInfo & gcmi)
	{
	GenericFile * file;

	GNREASSERT(GetFile(0, file));
	GNRAISE(file->GetCopyManagementInfo(gcmi));
	}

//
//  Flush
//

Error DVDDataFile::Flush(void)
	{
	GNRAISE_OK;
	}

//
//  Terminate request
//

Error DVDDataFile::TerminateRequest(RequestHandle * rh)
	{
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  DVD Iterator Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDIterator::DVDIterator(DVDFileSystem * dvdfs, GenericFileSystemIterator * gfsi) : GenericFileSystemIterator(dvdfs)
	{
	this->gfsi = gfsi;
	}

//
//  Destructor
//

DVDIterator::~DVDIterator(void)
	{
	delete gfsi;
	}

//
//  Clone
//

Error DVDIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	DVDIterator * dvdi;

	GNREASSERT(Clone(dvdi));
	gfsi = dvdi;
	GNRAISE_OK;
	}

//
//  Clone
//

Error DVDIterator::Clone(DVDIterator * & dvdi) const
	{
	GenericFileSystemIterator * help;

	GNREASSERT(gfsi->Clone(help));
	dvdi = new DVDIterator((DVDFileSystem*)gfs, help);
	if (dvdi)
		GNRAISE_OK;
	else
		{
		delete gfsi;
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	}

//
//  Comparison function
//

BOOL DVDIterator::Equals(GenericFileSystemIterator * gfsi)
	{
	return *(this->gfsi) == (*gfsi);
	}

//
//  Go to first item
//

Error DVDIterator::GoToFirstItem(void)
	{
	return gfsi->GoToFirstItem();
	}

//
//  Go to next item
//

Error DVDIterator::GoToNextItem(void)
	{
	return gfsi->GoToNextItem();
	}

//
//  Go to sub dir
//

Error DVDIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(this->gfsi->GoToSubDir(help));
	gfsi = new DVDIterator((DVDFileSystem*)gfs, help);

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

Error DVDIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(this->gfsi->GoToSubDir(help));
	gfsi = new DVDIterator((DVDFileSystem*)gfs, help);

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

Error DVDIterator::GetPathName(DiskItemName & name)
	{
	return gfsi->GetPathName(name);
	}

//
//  Get item type
//

Error DVDIterator::GetItemType(DiskItemType & type)
	{
	return gfsi->GetItemType(type);
	}

//
//  Get item name
//

Error DVDIterator::GetItemName(DiskItemName & name)
	{
	return gfsi->GetItemName(name);
	}

//
//  Get item size
//

Error DVDIterator::GetItemSize(KernelInt64 & size)
	{
	return gfsi->GetItemSize(size);
	}

//
//  Get number of items
//

Error DVDIterator::GetNumberOfItems(DWORD & num)
	{
	return gfsi->GetNumberOfItems(num);
	}

//
// Get current dir
//

Error DVDIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Open item
//

Error DVDIterator::OpenItem(DWORD accessType, GenericDiskItem * & gdi)
	{
	return gfs->OpenItem(this, accessType, gdi);
	}

////////////////////////////////////////////////////////////////////
//
//  DVD File System Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDFileSystem::DVDFileSystem(EventDispatcher* pEventDispatcher)
	: GenericFileSystem()
	, EventSender(pEventDispatcher)
	{
	cdvdVolume = NULL;
	baseFS = NULL;
	fileSystemType = FS_DVD;
	}

//
//  Destructor
//

DVDFileSystem::~DVDFileSystem(void)
	{
	//DP("From ~DVDFileSystem - FS R:");
	if (baseFS)
		baseFS->Release();

	//DP("From ~DVDFileSystem - V R:");
	if (cdvdVolume)
		cdvdVolume->Release();
	}

//
//  Initialize
//  (Note that volume is ignored)
//

Error DVDFileSystem::Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile)
	{
	GNREASSERT(((RootIterator*)mountPoint)->ObtainSubFileSystem(baseFS));
	GNREASSERT(baseFS->ObtainVolume(volume));
	cdvdVolume = (CDVDVolume*)volume;
	GNREASSERT(GenericFileSystem::Init(mountPoint, volume, profile));
	return baseFS->GetLogicalBaseBlock(logicalBaseBlock);
	}

//
//  Open item by iterator
//

Error DVDFileSystem::OpenItem(GenericFileSystemIterator * gfsi, DWORD accessType, GenericDiskItem * & gdi)
	{
	GenericFile * gf;
	GenericDirectory * gd;
	DiskItemType type;
	Error err;

	GNREASSERT(gfsi->GetItemType(type));
	if (type == DIT_FILE)
		{
		GNREASSERT(CreateFile(accessType, gf));
		gdi = gf;
		}
	else if (type == DIT_DIR)
		{
		GNREASSERT(CreateDirectory(accessType, gd));
		gdi = gd;
		}
	else
		GNRAISE(GNR_OBJECT_INVALID);

	if (IS_ERROR(err = gdi->Open(((DVDIterator*)gfsi)->gfsi, accessType)))
		delete gdi;

	GNRAISE(err);
	}

//
//  Open item by name (this is here only because of the stupidity of C++)
//

Error DVDFileSystem::OpenItem(const DiskItemName & name, DWORD accessType, GenericDiskItem * & gdi)
	{
	return GenericFileSystem::OpenItem(name, accessType, gdi);
	}

//
//  Create file
//

Error DVDFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	if (flags & FAT_HEADER)
		gf = new DVDHeaderFile(this, baseFS, GetEventDispatcher());
	else
		gf = new DVDDataFile(this, baseFS, GetEventDispatcher());

	if (gf)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error DVDFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Create iterator
//

Error DVDFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	GenericFileSystemIterator * help;

	GNREASSERT(baseFS->CreateIterator(help));

	gfsi = new DVDIterator(this, help);
	if (gfsi)
		GNRAISE_OK;
	else
		{
		delete help;
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}
	}

//
//  Seek block
//

Error DVDFileSystem::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Lock blocks
//

Error DVDFileSystem::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Unlock blocks
//

Error DVDFileSystem::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Get copy management mode
//

Error DVDFileSystem::GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi)
	{
	gcmi = GCMI_COPY_FORBIDDEN;
	GNRAISE_OK;
	}

//
//  Get volume name
//

Error DVDFileSystem::GetVolumeName(KernelString & name)
	{
	return baseFS->GetVolumeName(name);
	}

//
//  Get volume set name
//

Error DVDFileSystem::GetVolumeSetName(KernelString & name)
	{
	return baseFS->GetVolumeSetName(name);
	}

//
//  Get volume set index
//

Error DVDFileSystem::GetVolumeSetIndex(int & index)
	{
	return baseFS->GetVolumeSetIndex(index);
	}

//
//  Get volume set size
//

Error DVDFileSystem::GetVolumeSetSize(int & size)
	{
	return baseFS->GetVolumeSetSize(size);
	}

//
//  Build unique key
//

Error DVDFileSystem::BuildUniqueKey(void)
	{
	GenericDiskItem * gdi;
	DVDHeaderFile * file;
	Error err = GNR_OK;

	err = GNR_INVALID_UNIQUE_KEY;
	if (!IS_ERROR(OpenItem("video_ts\\video_ts.ifo", FAT_HEADER, gdi)) ||
		 !IS_ERROR(OpenItem("video_ts\\video_ts.inf", FAT_HEADER, gdi)) ||
		 !IS_ERROR(OpenItem("audio_ts\\audio_ts.ifo", FAT_HEADER, gdi)))
		{
		file = (DVDHeaderFile*)gdi;
		err = file->GetUniqueKey(uniqueKey.key);
		file->Close();
		delete file;
		}

	GNRAISE(err);
	}

//
//  Set drive block size
//

Error DVDFileSystem::SetDriveBlockSize(DWORD size)
	{
	GNRAISE(GNR_OPERATION_NOT_SUPPORTED);
	}

//
//  Get number of drive blocks
//

Error DVDFileSystem::GetNumberOfDriveBlocks(DWORD & num)
	{
	return baseFS->GetNumberOfDriveBlocks(num);
	}

//
//  Set number of drive blocks
//

Error DVDFileSystem::SetNumberOfDriveBlocks(DWORD num)
	{
	return baseFS->SetNumberOfDriveBlocks(num);
	}

//
//  Terminate request
//

Error DVDFileSystem::TerminateRequest(RequestHandle * rh)
	{
	return baseFS->TerminateRequest(rh);
	}

//
//  Flush
//

Error DVDFileSystem::Flush(void)
	{
	return baseFS->Flush();
	}

//
//  Get drive name
//

Error DVDFileSystem::GetDriveName(KernelString & name)
	{
	return baseFS->GetDriveName(name);
	}

//
//  Test if DVD is encrypted
//

Error DVDFileSystem::DVDIsEncrypted(BOOL & enc)
	{
	if (cdvdVolume)
		return cdvdVolume->DVDIsEncrypted(enc);
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Do authentication command
//

Error DVDFileSystem::DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD block, BYTE * key)
	{
	if (cdvdVolume)
		return cdvdVolume->DoAuthenticationCommand(com, logicalBaseBlock + block, key);
	else
		GNRAISE(GNR_NO_VOLUME);
	}

//
//  Get RPC data
//

Error DVDFileSystem::GetRPCData(BOOL & isRPC2, BYTE & region, BYTE & availSets)
	{
	if (cdvdVolume)
		return cdvdVolume->GetRPCData(isRPC2, region, availSets);
	else
		GNRAISE(GNR_NO_VOLUME);
	}
