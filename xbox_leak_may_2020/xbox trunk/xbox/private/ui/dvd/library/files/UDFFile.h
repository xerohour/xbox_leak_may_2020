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
//  NOTE: UDFFile, UDFDirectory and UDFIterator have protected
//  constructors so they can only be created by the file system
//  (must be friend). Only copy constructor is public.
//
////////////////////////////////////////////////////////////////////

#ifndef UDFFILE_H
#define UDFFILE_H

#include "library/Files/GenericFile.h"
#include "library/Files/CDVDVolume.h"

class UDFFile;
class UDFDirectory;
class UDFIterator;
class UDFFileSystem;

////////////////////////////////////////////////////////////////////
//
//  UDF File Class
//
////////////////////////////////////////////////////////////////////

class UDFFile : public GenericFile
	{
	friend class UDFFileSystem;

	protected:
		UDFFileSystem	*	udffs;
		UDFIterator		*	udfi;					// The iterator pointing to this file

		GenericCopyManagementInfo gcmi;	// Cached GCMI
		BOOL gcmiValid;						// Cached GCMI is valid

		//
		//  Construction/destruction
		//

		UDFFile(UDFFileSystem * udffs);

		virtual Error BuildUniqueKey(void);

	public:
		virtual ~UDFFile(void);

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
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
//  UDF Directory Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

class UDFDirectory : public GenericDirectory
	{
	friend class UDFFileSystem;

	protected:
		UDFFileSystem	*	udffs;			// The file system this directory belongs to
		UDFIterator		*	udfi;				// The iterator pointing to this dir

		//
		//  Construction/destruction
		//

		UDFDirectory(UDFFileSystem * udffs);

	public:
		virtual ~UDFDirectory(void);

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
//  UDF File System Iterator Class
//  Corresponds to an UDF File Entry
//
////////////////////////////////////////////////////////////////////

class UDFIterator : public GenericFileSystemIterator
	{
	friend class UDFFileSystem;

	protected:
		DWORD dirICB;							// The ICB describing the dir the iterator is currently in
		DWORD dirStart;						// Start block of this dir
		DWORD dirSize;							// Size of this dir
		DWORD entryOffset;					// Offset of this entry within dir

		UDFIterator(GenericFileSystem * gfs);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi);

	public:
		virtual ~UDFIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(UDFIterator * & udfi) const;

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
//   UDF File System Class
//
////////////////////////////////////////////////////////////////////

class UDFFileSystem : public GenericFileSystem
	{
	friend class UDFFile;
	friend class UDFDirectory;
	friend class UDFIterator;
	friend class UDFFileEntry;

	protected:
		CDVDVolume		*	cdvdVolume;						// The volume we are operating on (type cast only)
		DWORD					primaryVolumeDescriptor;	// Block address of PVD
		DWORD					rootICB;							// Root directory ICB block (logical)

		RequestHandle		rh;								// For directory handling purposes
		DriveBlock			driveBlock;						// For directory handling purposes

		PathStringCache	cache;							// For caching directory entries

		//
		//  Factory Methods
		//

		Error CreateIterator(UDFIterator * & udfi, DWORD dirICB);
		Error CreateIterator(GenericFileSystemIterator * & gfsi, DWORD dirICB);

		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);

		//
		//  Item Handling
		//

		Error GoToFirstItem(UDFIterator * udfi);
		Error GoToNextItem(UDFIterator * udfi);
		Error GoToSubDir(UDFIterator * udfi, GenericFileSystemIterator * & gfsi);
		Error GoToParentDir(UDFIterator * udfi, GenericFileSystemIterator * & gfsi);

		Error GetPathName(UDFIterator * udfi, DiskItemName & name);
		Error GetItemType(UDFIterator * udfi, DiskItemType & type);
		Error GetItemName(UDFIterator * udfi, DiskItemName & name);
		Error GetItemSize(UDFIterator * udfi, KernelInt64 & size);

		Error GetDir(UDFIterator * udfi, GenericDirectory * & gd);		// Returns the dir the iterator is currently in
		Error GetNumberOfItems(UDFIterator * udfi, DWORD & num);			// Returns the number of items in the dir the iterator is currently in

		//
		//  Internal helper functions
		//

		Error ReadByte(DWORD block, DWORD offset, BYTE & b);
		Error ReadWord(DWORD block, DWORD offset, WORD & w);
		Error ReadDWord(DWORD block, DWORD offset, DWORD & d);
		Error ReadQWord(DWORD block, DWORD offset, KernelInt64 & q);
		Error BuildUniqueKey(void);
		Error GetAllocDescriptor(UDFIterator * udfi, WORD index, DWORD & start, DWORD & length);
		Error GetCopyManagementInfo(UDFIterator * udfi, GenericCopyManagementInfo & gcmi);

	public:
		UDFFileSystem(void);
		virtual ~UDFFileSystem(void);

		virtual Error Init(GenericFileSystemIterator * mountPoint, GenericVolume * volume, GenericProfile * profile);

		//
		//  Factory methods
		//

		virtual Error CreateIterator(UDFIterator * & udfi);						// In root dir of UDF file system
		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);		// In root dir of UDF file system
	};

#endif
