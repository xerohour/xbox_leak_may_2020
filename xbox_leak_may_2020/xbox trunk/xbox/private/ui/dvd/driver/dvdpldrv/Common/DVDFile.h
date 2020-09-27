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

#ifndef DVDFILE_H
#define DVDFILE_H

class DVDFile;
class DVDChainFile;
class DVDFileSystem;

#include "Library/Files/RootFile.h"
#include "Library/Files/CDVDVolume.h"
#include "EventSender.h"

//////////////////////////////////////////////////////////////////////
//
//  DVD Header File Class
//
//////////////////////////////////////////////////////////////////////

class DVDHeaderFile : public GenericFile, public EventSender
	{
	friend class DVDFileSystem;

	protected:
		GenericFileSystem * baseFS;
		GenericFile * file;

		virtual Error BuildUniqueKey(void);

		DVDHeaderFile(DVDFileSystem * dvdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher);

	public:
		virtual ~DVDHeaderFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Misc. inquiry
		//

		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);
		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		Error ReadByte(DWORD position, BYTE & b, RequestHandle * rh);
		Error ReadWord(DWORD position, WORD & w, RequestHandle * rh);
		Error ReadDWord(DWORD position, DWORD & d, RequestHandle * rh);

		Error ReadByteSeq(DWORD & position, BYTE & b, RequestHandle * rh);
		Error ReadWordSeq(DWORD & position, WORD & w, RequestHandle * rh);
		Error ReadDWordSeq(DWORD & position, DWORD & d, RequestHandle * rh);

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);

		//
		//  DVD specific functions
		//

		Error	IsEncrypted(BOOL & enc);
		Error DoAuthenticationCommand(DVDAuthenticationCommand com, BYTE * key);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Data File Class
//
//////////////////////////////////////////////////////////////////////

class ChainFileNode;

class DVDDataFile : public GenericFile, public EventSender
	{
	friend class DVDFileSystem;

	protected:
		GenericFileSystem * baseFS;

		List files;

		KernelInt64 size;
		DWORD headerSize;
		DWORD dataSize;
		DWORD	nextBlock;
		DWORD	fileOffset;		// Offset in blocks in the file
		DWORD	firstEncryptedBlock;
		DWORD	secondEncryptedBlock;

		//
		//  Cache for accessing a chained file
		//

		struct
			{
			ChainFileNode * node;
			DWORD relativeStartBlock;
			} lastFile;

		//
		//  Specific functions
		//

		Error FindVOBStart(void);
		Error FindNextEncryptedBlock(DWORD & block);
		Error FindFirstEncryptedBlock(void);
		Error AppendFile(GenericFileSystemIterator * gfsi, DWORD accessType);
		Error RemoveFiles(void);

		//
		//  Internal helper functions
		//

		virtual Error BuildUniqueKey(void);
		void SetFileOffset(DWORD offset) { fileOffset = offset; }
		Error GetFile(DWORD block, GenericFile * & file);
		Error GetFile(DWORD block, DWORD num, GenericFile * & file, DWORD & relativeStart, DWORD & avail);

		DVDDataFile(DVDFileSystem * dvdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher);

	public:
		~DVDDataFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accesType);
		virtual Error Close(void);

		//
		//  Misc. inquiry
		//

		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);
		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		virtual Error ReadByte(KernelInt64 position, BYTE & b, RequestHandle * rh);

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);

		//
		//  DVD specific functions
		//

		Error IsEncrypted(BOOL & enc);
		Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD & sector, BYTE * key, int retry);
	};

////////////////////////////////////////////////////////////////////
//
//  DVD Iterator Class
//
////////////////////////////////////////////////////////////////////

class DVDIterator : public GenericFileSystemIterator
	{
	friend class DVDFileSystem;

	protected:
		GenericFileSystemIterator * gfsi;

		DVDIterator(DVDFileSystem * gfs, GenericFileSystemIterator * gfsi);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi);

	public:
		virtual ~DVDIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(DVDIterator * & dvdi) const;

		//
		//  Directory Handling
		//

		virtual Error GoToFirstItem(void);
		virtual Error GoToNextItem(void);
		virtual Error GoToSubDir(GenericFileSystemIterator * & gfsi);
		virtual Error GoToParentDir(GenericFileSystemIterator * & gfsi);

		//
		//  Inquiry
		//

		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetItemType(DiskItemType & type);
		virtual Error GetItemName(DiskItemName & name);
		virtual Error GetItemSize(KernelInt64 & size);
		virtual Error GetNumberOfItems(DWORD & num);
		virtual Error GetCurrentDir(GenericDirectory * & gd);
//		virtual Error GetFileSystem(GenericFileSystem * & gfs);

		//
		//  File Access
		//

		virtual Error OpenItem(DWORD accessType, GenericDiskItem * & gdi);
	};

////////////////////////////////////////////////////////////////////
//
//  DVD File System Class
//
////////////////////////////////////////////////////////////////////

class DVDFileSystem : public GenericFileSystem, public EventSender
	{
	friend class DVDFile;
	friend class DVDHeaderFile;
	friend class DVDDataFile;

	protected:
		GenericFileSystem * baseFS;
		CDVDVolume * cdvdVolume;

		//
		//  Factory methods
		//

		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);

		//
		//  Data Access Methods
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  Internal functions
		//

		Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD block, BYTE * key);
		Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi);
		Error OpenFile(DVDFile * file, GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error BuildUniqueKey(void);

	public:
		DVDFileSystem(EventDispatcher* pEventDispatcher);
		virtual ~DVDFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);

		//
		//  Item handling
		//

		virtual Error OpenItem(GenericFileSystemIterator * gfsi, DWORD accessType, GenericDiskItem * & gdi);
		virtual Error OpenItem(const DiskItemName & name, DWORD accessType, GenericDiskItem * & gdi);

		//
		//  Factory Methods
		//

		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Volume Information
		//

		virtual Error GetVolumeName(KernelString & name);
		virtual Error GetVolumeSetName(KernelString & name);
		virtual Error GetVolumeSetIndex(int & index);
		virtual Error GetVolumeSetSize(int & size);

		//
		//  Drive "Pass Through" Functions
		//

		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		virtual Error TerminateRequest(RequestHandle * rh);
		virtual Error Flush(void);

		virtual Error GetDriveName(KernelString & name);

		//
		//  DVD specific functions
		//

		virtual Error DVDIsEncrypted(BOOL & enc);
		virtual Error GetRPCData(BOOL & isRPC2, BYTE & region, BYTE & availSets);
	};

#endif
