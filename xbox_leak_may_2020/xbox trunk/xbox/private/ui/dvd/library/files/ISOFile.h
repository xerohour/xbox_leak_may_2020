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
//  NOTE: ISOFile, ISODirectory and ISOIterator have protected
//  constructors so they can only be created by the file system
//  (must be friend). Only copy constructor is public.
//
////////////////////////////////////////////////////////////////////

#ifndef ISOFILE_H
#define ISOFILE_H

#include "library/Files/GenericFile.h"
#include "library/Files/CDVDVolume.h"

////////////////////////////////////////////////////////////////////
//
//  Defines
//
////////////////////////////////////////////////////////////////////

#define ISO_BLOCK_SIZE 2048

class ISOFile;
class ISODirectory;
class ISOIterator;
class ISOFileSystem;

////////////////////////////////////////////////////////////////////
//
//  ISO File Class
//
////////////////////////////////////////////////////////////////////

class ISOFile : public GenericFile
	{
	friend class ISOFileSystem;

	protected:
		ISOFileSystem * isofs;
		ISOIterator * isoi;
		KernelInt64 size;

		ISOFile(ISOFileSystem * isofs);

		virtual Error BuildUniqueKey(void);

	public:
		virtual ~ISOFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accesType);
		virtual Error Close(void);

		//
		//  Misc. inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetSize(KernelInt64 & size);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);
		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  ISO Directory Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

class ISODirectory : public GenericDirectory
	{
	friend class ISOFileSystem;

	protected:
		ISOFileSystem * isofs;
		ISOIterator * isoi;

		ISODirectory(ISOFileSystem * isofs);

	public:
		virtual ~ISODirectory(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name);
		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi);

		//
		//  Item handling
		//

		virtual Error CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi);
		virtual Error DeleteItem(GenericFileSystemIterator * gfsi);

		virtual Error GetNumberOfItems(DWORD & num);
		virtual Error FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi);
	};

#endif

////////////////////////////////////////////////////////////////////
//
//  ISO File System Iterator Class
//  Represents an ISO directory record (and a little more)
//
////////////////////////////////////////////////////////////////////

class ISOIterator : public GenericFileSystemIterator
	{
	friend class ISOFileSystem;

	protected:
		DWORD dirStart;		// Start block of the dir we are in
		DWORD dirSize;			// Length of the dir we are in
		DWORD entryOffset;	// Offset to this entry within entryBlock

		//
		//  Constructor
		//

		ISOIterator(GenericFileSystem * gfs, DWORD dirStart, DWORD dirSize);
		ISOIterator(GenericFileSystem * gfs, DWORD dirStart, DWORD dirSize, DWORD entryOffset);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi);

	public:
		virtual ~ISOIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(ISOIterator * & isoi) const;

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
//   ISO File System Class
//
////////////////////////////////////////////////////////////////////

enum ISOUnicodeType
	{
	IUT_NONE,
	IUT_UCS2_L1,
	IUT_UCS2_L2,
	IUT_UCS2_L3
	};

class ISOFileSystem : public GenericFileSystem
	{
	friend class ISOFile;
	friend class ISODirectory;
	friend class ISOIterator;

	protected:
		CDVDVolume * cdvdVolume;

		RequestHandle rh;					// For internal directory operations
		DriveBlock driveBlock;			// For internal directory operations
		PathStringCache cache;			// For caching paths, marked by the dir start block
		DWORD rootDirStart;				// Start block of root directory
		DWORD rootDirSize;				// Size of root directory
		ISOUnicodeType	unicodeType;	// The type of unicode used for directory entries

		//
		//  Factory Methods
		//

				  Error CreateIterator(ISOIterator * & isoi, DWORD dirStart, DWORD dirSize);
		virtual Error CreateIterator(ISOIterator * & isoi);
		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);
		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);

		//
		//  Item handling
		//

		Error	GoToFirstItem(ISOIterator * isoi);
		Error	GoToNextItem(ISOIterator * isoi);
		Error	GoToSubDir(ISOIterator * isoi, GenericFileSystemIterator * & gfsi);
		Error	GoToParentDir(ISOIterator * isoi, GenericFileSystemIterator * & gfsi);

		Error GetPathName(ISOIterator * isoi, DiskItemName & name);
		Error GetItemType(ISOIterator * isoi, DiskItemType & type);
		Error GetItemName(ISOIterator * isoi, DiskItemName & name);
		Error GetItemSize(ISOIterator * isoi, KernelInt64 & size);

		Error GetDir(ISOIterator * isoi, GenericDirectory * & gd);
		Error GetNumberOfItems(ISOIterator * isoi, DWORD & num);

		//
		//  Internal helper functions
		//

		Error ReadByte(DWORD block, DWORD offset, BYTE & b);
		Error ReadWord(DWORD block, DWORD offset, WORD & w);
		Error ReadDWord(DWORD block, DWORD offset, DWORD & d);
		Error ReadQWord(DWORD block, DWORD offset, KernelInt64 & q);
		Error BuildUniqueKey(void);
		Error GetStartBlock(ISOIterator * isoi, DWORD & startBlock);
		Error ReadXASUInfo(ISOIterator * isoi, WORD & sui);

	public:
		//
		//  Con/Destruction, initialization
		//

		ISOFileSystem(void);
		virtual ~ISOFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);
	};

#endif
