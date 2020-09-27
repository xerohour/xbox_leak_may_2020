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
//  Generic File Classes
//
//  NOTE: GenericDiskItem, GenericFile
//  GenericDirectory and GenericFileSystemIterator have protected
//  constructors so they can only be created by the file system
//  (must be friend). Only copy constructor is public.
//
////////////////////////////////////////////////////////////////////

#ifndef GENERICFILE_H
#define GENERICFILE_H

#define GFS_ENABLE_DIRECTORIES 0		// Set this if you want to enable directory objects

class GenericFileSystem;

#include "Library/Files/GenericVolume.h"
#include "Library/Files/DiskItemName.h"
#include "Library/Hardware/Drives/Generic/DriveErrors.h"
#include "Library/Files/FileErrors.h"
#include "Library/Common/KrnlInt.h"
#include "Library/Common/KrnlSync.h"
#include "Library/General/Lists.h"

////////////////////////////////////////////////////////////////////
//
//  Type Definitions
//
////////////////////////////////////////////////////////////////////

//
//  Disk Item Type
//

enum DiskItemType
	{
	DIT_NONE		= 0x0000,
	DIT_FILE		= 0x0001,
	DIT_DIR		= 0x0002,
//	DIT_DRIVE	= 0x0004,
	DIT_ALL		= 0xffff
	};

//
//  Extended Disk Item Type
//

enum ExtDiskItemType
	{
	EDIT_NONE,
	EDIT_AUDIO_TRACK,			// CD Audio Track
	EDIT_DATA_TRACK,			// CD Data Track
	EDIT_DRIVE					// A drive shown as directory
//	EDIT_SYS_DIR				// System directory
	};

//
// File system types
//
enum enumFileSystemType
	{
	FS_GENERIC = 0,
	FS_ROOT,
	FS_CD,
	FS_ISO,
	FS_UDF,
	FS_CDDA,
	FS_DVD,
	FS_VCD,
	FS_AVF
	};


//
//  File Access Type (for Open() call)
//

#define FAT_NONE					0x00000000
#define FAT_READ					0x00000001	// Open file to read
#define FAT_WRITE					0x00000002	// Open file to write
#define FAT_CHAIN					0x00000004	// Open file as chain file	(DVD data file only)
#define FAT_HEADER				0x00000008	// Open file as header file (i.e. form 1 sectors), else as data file (i.e. form 2 sectors)
#define FAT_RECURSIVE			0x00000010	// Search recursively for file
#define FAT_CASE_SENSITIVE		0x00000020	// Be case sensitive when opening file by name

//
//  Read flags
//

#define FAT_RAW			0x00000800	// Read raw data (i.e. include checksum data etc.)

//
//  FindItem flags
//

#define FIF_NONE					0x00000000		// Nothing at all
#define FIF_RECURSIVE			0x00000001		// Do recursive search
#define FIF_CASE_SENSITIVE		0x00000002		// Search is case sensitive

//
//  Forward declarations
//

class GenericFile;
class GenericDirectory;
class GenericFileSystemIterator;
class GenericFileSystem;

////////////////////////////////////////////////////////////////////
//
//  Extended item information
//
////////////////////////////////////////////////////////////////////

class ExtItemInfo
	{
	public:
		ExtItemInfo(void)							{ exType = EDIT_NONE; }
		ExtItemInfo(ExtDiskItemType type)	{ exType = type; }

		ExtDiskItemType	exType;
	};

////////////////////////////////////////////////////////////////////
//
//  Unique Key Class
//
////////////////////////////////////////////////////////////////////

class UniqueKey
	{
	public:
		BOOL valid;
		BYTE key[8];

		UniqueKey(void)	{ valid = FALSE; }
	};

////////////////////////////////////////////////////////////////////
//
//  Path cache class
//
////////////////////////////////////////////////////////////////////


#define PATHCACHESIZE 16

class StringCacheElement : protected Node
	{
	public:
		friend class PathStringCache;

	protected:
		StringCacheElement(DWORD b, const DiskItemName & name) : block(b), name(name) {};

		DWORD block;
		DiskItemName name;
	};


class PathStringCache : protected List
	{
	private:
		int size;
	public:
		PathStringCache(int size = PATHCACHESIZE)
			{
			this->size = size;
			}

		~PathStringCache(void)
			{
			StringCacheElement * node = NULL;

			IteratorHandle h(CreateIterator());
			while (node = ((StringCacheElement*) h.Proceed()))
				{
				Remove((Node*) node);
				delete (StringCacheElement*) node;
				}
			}

		void Add(DiskItemName name, DWORD block);

		Error Get(DWORD block, DiskItemName & name);
	};

////////////////////////////////////////////////////////////////////
//
//  Generic Disk Item Class
//
////////////////////////////////////////////////////////////////////

class GenericDiskItem
	{
	protected:
		GenericFileSystem * gfs;
		VDLocalMutex lock;

		GenericDiskItem(GenericFileSystem * gfs);

	public:
		virtual ~GenericDiskItem(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType) = 0;
		virtual Error Close(void) = 0;

		//
		//  Inquiry
		//

		virtual Error GetName(DiskItemName & name) = 0;
		virtual Error GetPathName(DiskItemName & name) = 0;
		virtual Error GetSize(KernelInt64 & size) = 0;
		virtual Error GetIterator(GenericFileSystemIterator * & gfsi) = 0;

		//
		//  Misc.
		//

		virtual Error Flush(void);
		virtual Error TerminateRequest(RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  Generic File Class
//
////////////////////////////////////////////////////////////////////

class GenericFile : public GenericDiskItem
	{
	protected:
		UniqueKey uniqueKey;
		DWORD startBlock;
		BOOL headerFile;		// TRUE if header file, data file else

		GenericFile(GenericFileSystem * gfs);

		virtual Error BuildUniqueKey(void) = 0;

	public:
		virtual ~GenericFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);

		//
		//  Misc. inquiry
		//

		virtual Error GetUniqueKey(BYTE * key);
		virtual Error GetCopyManagementInfo(GenericCopyManagementInfo & gcmi) = 0;
		virtual Error GetStartBlock(DWORD & startBlock);

		//
		//  Access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh) = 0;
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh) = 0;
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh) = 0;

		virtual Error ReadByte(DWORD pos, BYTE & b, DWORD flags, RequestHandle * rh);
		virtual Error ReadWord(DWORD pos, WORD & w, DWORD flags, RequestHandle * rh);
		virtual Error ReadDWord(DWORD pos, DWORD & d, DWORD flags, RequestHandle * rh);

		virtual Error ReadByte(KernelInt64 pos, BYTE & b, DWORD flags, RequestHandle * rh);
		virtual Error ReadWord(KernelInt64 pos, WORD & w, DWORD flags, RequestHandle * rh);
		virtual Error ReadDWord(KernelInt64 pos, DWORD & d, DWORD flags, RequestHandle * rh);
		virtual Error ReadBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh);
		virtual Error WriteBytes(KernelInt64 pos, KernelInt64 num, BYTE * buffer, DWORD flags, RequestHandle * rh);
	};

////////////////////////////////////////////////////////////////////
//
//  Generic Directory Class
//
////////////////////////////////////////////////////////////////////

class GenericDirectory : public GenericDiskItem
	{
	protected:
		GenericDirectory(GenericFileSystem * gfs);

	public:
		virtual ~GenericDirectory(void);

		//
		//  Misc
		//

		virtual Error GetSize(KernelInt64 & size);

		//
		//  Item handling
		//

		virtual Error CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi) = 0;
		virtual Error DeleteItem(GenericFileSystemIterator * gfsi) = 0;

		virtual Error GetNumberOfItems(DWORD & num) = 0;
		virtual Error FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi);
	};

////////////////////////////////////////////////////////////////////
//
//  Generic File System Iterator Class
//
////////////////////////////////////////////////////////////////////

class GenericFileSystemIterator
	{
	friend class GenericFileSystem;

	protected:
		GenericFileSystem * gfs;

		GenericFileSystemIterator(GenericFileSystem * gfs);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi) = 0;

	public:
		virtual ~GenericFileSystemIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const = 0;
		friend int operator==(GenericFileSystemIterator & gfsi1, GenericFileSystemIterator & gfsi2);

		//
		//  Directory Handling
		//

		virtual Error GoToFirstItem(void) = 0;
		virtual Error GoToNextItem(void) = 0;
		virtual Error GoToSubDir(GenericFileSystemIterator * & gfsi) = 0;
		virtual Error GoToParentDir(GenericFileSystemIterator * & gfsi) = 0;

		//
		//  Item inquiry
		//

		virtual Error GetPathName(DiskItemName & name);
		virtual Error GetItemType(DiskItemType & type) = 0;
		virtual Error GetItemName(DiskItemName & name) = 0;
		virtual Error GetItemSize(KernelInt64 & size) = 0;
		virtual Error GetExtItemInfo(ExtItemInfo & info);

		//
		//  Directory/FileSystem inquiry
		//

		virtual Error GetNumberOfItems(DWORD & num) = 0;
		virtual Error GetCurrentDir(GenericDirectory * & gd) = 0;
		virtual Error GetFileSystem(GenericFileSystem * & gfs);
		virtual Error GetSubFileSystem(GenericFileSystem * & gfs);

		//
		//  File Access
		//

		virtual Error OpenItem(DWORD accessType, GenericDiskItem * & gdi) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  Generic (Hierarchical) File System Class
//  File Systems act as creators and destructors for files,
//  directories, etc.
//
////////////////////////////////////////////////////////////////////

class GenericFileSystem
	{
	friend class GenericFile;
	friend class GenericDirectory;
	friend class GenericFileSystemIterator;

	protected:
		GenericVolume	*	genericVolume;
		GenericProfile	*	profile;
		enumFileSystemType		fileSystemType;
	public:
		int					refCount;
	protected:
		UniqueKey			uniqueKey;
		DWORD					logicalBaseBlock;

		GenericFileSystemIterator * mountPoint;		//	Mount point of FS (in parent FS)
		VDLocalMutex lock;

		//
		//  Volume information
		//

		KernelString	volumeName;			// The name of the volume
		KernelString	volumeSetName;		// The name of the volume set
		WORD				volumeSetIndex;	// Index of volume within set
		WORD				volumeSetSize;		// Number of volumes in set
		DWORD				blockSize;			// Size of the drive blocks

		//
		//  Sector types
		//

		DWORD headerSectorType;		// Sector type for header files
		DWORD headerHeaderSize;		// Size of header in header sectors
		DWORD headerDataSize;		//	Size of data in header sectors
		DWORD dataSectorType;		// Sector type for data files
		DWORD dataHeaderSize;		// Size of header in data sectors
		DWORD dataDataSize;			// Size of data in data sectors

		//
		//  Factory methods
		//

		virtual Error CreateFile(DWORD flags, GenericFile * & gf) = 0;
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd) = 0;			// Root dir by default

		//
		//  Data Access Methods
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  Internal helper functions
		//

		virtual Error InternalFindItem(DiskItemName & name, DiskItemType type, DWORD flags,
												 GenericFileSystemIterator * where, GenericFileSystemIterator * & result);
		virtual Error BuildUniqueKey(void) = 0;

		virtual Error GetPathName(DiskItemName & name, GenericFileSystemIterator * where);

	public:
		GenericFileSystem(void);
		virtual ~GenericFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);
		virtual Error InitHeaderAndData(GenericVolume * volume);
		virtual Error Obtain(void);
		virtual Error Release(void);

		//
		//  Item handling
		//

		virtual Error FindItem(const DiskItemName & name, DiskItemType type, DWORD flags,
									  GenericFileSystemIterator * where, GenericFileSystemIterator * & result);
		virtual Error OpenItem(const DiskItemName & name, DWORD accessType, GenericDiskItem * & gdi);
		virtual Error OpenItem(GenericFileSystemIterator * gfsi, DWORD accessType, GenericDiskItem * & gdi);
		virtual Error GetExtItemInfo(GenericFileSystemIterator * gfsi, ExtItemInfo & info);

		//
		//  Factory Methods
		//

		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi) = 0;		// In root dir of current file system
		virtual Error CreateRootIterator(GenericFileSystemIterator * & gfsi);		// In root dir of ALL file systems
		virtual Error CreateRootDirectory(GenericDirectory * & gd);

		//
		//  Volume Information
		//

		virtual Error GetDiskType(PhysicalDiskType & type);
		virtual Error GetVolumeName(KernelString & name);
		virtual Error GetVolumeSetName(KernelString & name);
		virtual Error GetVolumeSetIndex(int & index);
		virtual Error GetVolumeSetSize(int & size);
		virtual Error GetUniqueKey(BYTE * key);
		virtual Error GetLogicalBaseBlock(DWORD & logicalBaseBlock);
		virtual Error GetDTSInfo(BOOL & isDTS, GenericCopyManagementInfo & gcmi);
		virtual Error ObtainVolume(GenericVolume * & volume);

		DWORD GetHeaderSectorType(void)	{ return headerSectorType; }
		DWORD GetHeaderHeaderSize(void)	{ return headerHeaderSize; }
		DWORD GetHeaderDataSize(void)		{ return headerDataSize; }
		DWORD GetDataSectorType(void)		{ return dataSectorType; }
		DWORD GetDataHeaderSize(void)		{ return dataHeaderSize; }
		DWORD GetDataDataSize(void)		{ return dataDataSize; }

		//
		//  Drive "Pass Through" Functions
		//

		virtual Error GetCurrentDriveBlockSize(DWORD & size);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		virtual Error TerminateRequest(RequestHandle * rh);
		virtual Error Flush(void);

		virtual Error GetDriveName(KernelString & name);
#if MSNDISC
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset);
#else
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
#endif
		virtual Error SpinUpDrive(void);
		virtual Error SpinDownDrive(void);

		enumFileSystemType GetFileSystemType(void) { return fileSystemType; }
	};

#endif
