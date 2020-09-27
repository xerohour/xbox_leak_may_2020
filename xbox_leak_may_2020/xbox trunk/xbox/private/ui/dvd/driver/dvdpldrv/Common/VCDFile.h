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
//  VCD File Classes
//
////////////////////////////////////////////////////////////////////

#ifndef VCDFILE_H
#define VCDFILE_H

class VCDFile;
class VCDHeaderFile;
class VCDDataFile;
class VCDDirectory;
class VCDIterator;
class VCDFileSystem;

#include "Library/Files/RootFile.h"
#include "EventSender.h"

////////////////////////////////////////////////////////////////////
//
//  VCD File Class
//
////////////////////////////////////////////////////////////////////

class VCDFile : public GenericFile, public EventSender
	{
	protected:
		GenericFileSystem * baseFS;
		GenericFile * file;
		KernelInt64 size;

		VCDFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher);

	public:
		virtual ~VCDFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Misc. inquiry
		//

		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		virtual Error ReadBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh);
		virtual Error WriteBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh);

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  VCD Header File Class
//
////////////////////////////////////////////////////////////////////

class VCDHeaderFile : public VCDFile
	{
	friend class VCDFileSystem;

	protected:
		int useCount;

		VCDHeaderFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher);

		virtual Error BuildUniqueKey(void);

	public:
		virtual ~VCDHeaderFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);

		void Obtain(void);
		void Release(void);

		Error ReadByte(DWORD position, BYTE & b, RequestHandle * rh);
		Error ReadWord(DWORD position, WORD & w, RequestHandle * rh);
		Error ReadDWord(DWORD position, DWORD & d, RequestHandle * rh);
		Error ReadSectorAddress(DWORD position, DWORD & a, RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  VCD Data File Class
//
////////////////////////////////////////////////////////////////////

class VCDDataFile : public VCDFile
	{
	friend class VCDFileSystem;

	protected:
		DWORD nextBlock;
		DWORD numBlocks;
		BOOL	isPAL;

		VCDDataFile(VCDFileSystem * vcdfs, GenericFileSystem * baseFS, EventDispatcher* pEventDispatcher);

		virtual Error BuildUniqueKey(void);

	public:
		virtual ~VCDDataFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);

		Error GetNumberOfBlocks(DWORD & num) { num = numBlocks; GNRAISE_OK; }

		DWORD SectorToBlock(DWORD sector)
			{
			if (sector > startBlock)
				return sector - startBlock;
			else
				return 0;
			}

		DWORD AbsoluteBlockAddress(DWORD block)
			{
			return block + startBlock;
			}

		Error IsPALSequence(BOOL & pal) { pal = isPAL; GNRAISE_OK; }
	};

////////////////////////////////////////////////////////////////////
//
//  VCD Directory Class
//
////////////////////////////////////////////////////////////////////

class VCDDirectory : public GenericDirectory
	{
	friend class VCDFileSystem;

	protected:
		GenericFileSystem * baseFS;
		GenericDirectory * gd;

		VCDDirectory(VCDFileSystem * gfs, GenericFileSystem * baseFS);

	public:
		virtual ~VCDDirectory(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);

		//
		//  Item handling
		//

		virtual Error CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi);
		virtual Error DeleteItem(GenericFileSystemIterator * gfsi);

		virtual Error GetNumberOfItems(DWORD & num);
		virtual Error FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi);
	};

////////////////////////////////////////////////////////////////////
//
//  VCD File System Iterator Class
//
////////////////////////////////////////////////////////////////////

class VCDIterator : public GenericFileSystemIterator
	{
	friend class VCDFileSystem;

	protected:
		GenericFileSystemIterator * gfsi;

		VCDIterator(VCDFileSystem * vcdfs, GenericFileSystemIterator * gfsi);

	public:
		virtual ~VCDIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(VCDIterator * & vcdi) const;

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

		//
		//  File Access
		//

		virtual Error OpenItem(DWORD accessType, GenericDiskItem * & gdi);
	};

////////////////////////////////////////////////////////////////////
//
//  VCD File System Class
//
////////////////////////////////////////////////////////////////////

class VCDFileSystem : public GenericFileSystem, public EventSender
	{
	friend class VCDFile;
	friend class VCDDirectory;
	friend class VCDIterator;

	protected:
		GenericFileSystem * baseFS;

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
		//  Internal helper functions
		//

		virtual Error BuildUniqueKey(void);

	public:
		VCDFileSystem(EventDispatcher* pEventDispatcher);
		virtual ~VCDFileSystem(void);

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
		virtual Error SpinUpDrive(void);
		virtual Error SpinDownDrive(void);
	};

#endif
