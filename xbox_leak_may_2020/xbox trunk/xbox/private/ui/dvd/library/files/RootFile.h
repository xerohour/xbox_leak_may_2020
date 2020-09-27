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
//  Root File Classes
//
//  NOTE: GenericDiskItem, GenericFile, GenericChainFile,
//  GenericDirectory and GenericFileSystemIterator have protected
//  constructors so they can only be created by the file system
//  (must be friend). Only copy constructor is public.
//
////////////////////////////////////////////////////////////////////

#ifndef ROOTFILE_H
#define ROOTFILE_H

#include "Library/hardware/drives/generic/DriveTable.h"
#include "Library/Files/GenericFile.h"
#include "Library/Files/UDFFile.h"
#include "Library/Files/ISOFile.h"
#include "Library/Files/CDFile.h"

class RootDirectory;
class RootIterator;
class RootFileSystem;
class MountNode;

//
// Flags for RootFileSystem::CreateFileSystem
//

#define FS_NONE								0x00
#define FS_CD_FILE_SYSTEM_REQUIRED		0x01

////////////////////////////////////////////////////////////////////
//
//  Disk information class
//
////////////////////////////////////////////////////////////////////

class DiskInformation
	{
	public:
		KernelString		driveName;
		PhysicalDiskType	diskType;
		KernelString		volumeName;
		BYTE					uniqueKey[8];
		BYTE					diskRegion;
		BOOL					isRPC2;
		BYTE					driveRegion;
		BYTE					availSets;
	};

////////////////////////////////////////////////////////////////////
//
//  Root Directory Class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

class RootDirectory : public GenericDirectory
	{
	friend class RootFileSystem;

	protected:
		RootFileSystem * rootfs;
		RootIterator * rit;

		RootDirectory(RootFileSystem * rootfs);

	public:
		virtual ~RootDirectory(void);

		//
		//  Open/close
		//

		virtual Error Open(GenericFileSystemIterator * gfsi, DWORD accessType);
		virtual Error Close(void);

		//
		//  Misc
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
//  Root File System Iterator Class
//
////////////////////////////////////////////////////////////////////

class RootIterator : public GenericFileSystemIterator
	{
	friend class RootFileSystem;

	protected:
		MountNode * mountNode;

		RootIterator(GenericFileSystem * gfs);

		virtual BOOL Equals(GenericFileSystemIterator * gfsi);

	public:
		virtual ~RootIterator(void);

		virtual Error Clone(GenericFileSystemIterator * & gfsi) const;
		virtual Error Clone(RootIterator * & gfsi) const;

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
		virtual Error ObtainSubFileSystem(GenericFileSystem * & gfs, DWORD dwFlags = 0);
		virtual Error ReleaseSubFileSystem(void);

		//
		//  File Access
		//

		virtual Error OpenItem(DWORD accessType, GenericDiskItem * & gdi);

		//
		//  Region handling
		//

		virtual Error GetRPCData(BOOL & isRPC2, BYTE & regionCode, BYTE & availSets);
		virtual Error SetRegionCode(BYTE region);
	};

////////////////////////////////////////////////////////////////////
//
//  Root File System Class
//
//  This class forms a single common tree out of all files/dirs
//
//  NOTE: In order to avoid circular references to it, for the time
//        being RootFileSystem doesn't support Obtain/Release, so it
//        should be removed by 'delete' operator at the end of session.
//
////////////////////////////////////////////////////////////////////

class RootFileSystem : public GenericFileSystem
	{
	friend class RootDirectory;
	friend class RootIterator;

	protected:
		List mounts;				// List of file systems/volumes/drives which are "mounted"

		//
		//  Mount point handling
		//

		Error CreateFileSystem(MountNode * mn, DWORD dwFlags);

		//
		//  Factory Methods
		//

		virtual Error CreateIterator(RootIterator * & rit);
		virtual Error CreateIterator(GenericFileSystemIterator * & gfsi);
		virtual Error CreateFile(DWORD flags, GenericFile * & gf);
		virtual Error CreateDirectory(DWORD flags, GenericDirectory * & gd);					// Root dir by default

		//
		//  Data Access Methods, all prohibited
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  Item Handling
		//

		Error GoToFirstItem(RootIterator * cdi);
		Error GoToNextItem(RootIterator * cdi);
		Error GoToSubDir(RootIterator * cdi, GenericFileSystemIterator * & gfsi);
		Error GoToParentDir(RootIterator * cdi, GenericFileSystemIterator * & gfsi);

		Error GetPathName(RootIterator * rit, DiskItemName & name);
		Error GetItemType(RootIterator * rit, DiskItemType & type);
		Error GetItemName(RootIterator * rit, DiskItemName & name);
		Error GetItemSize(RootIterator * rit, KernelInt64 & size);

		Error GetDir(RootIterator * rit, GenericDirectory * & gd);
		Error GetNumberOfItems(RootIterator * rit, DWORD & num);

		//
		//  Region code handling
		//

		virtual Error GetRPCData(RootIterator * rit, BOOL & isRPC2, BYTE & regionCode, BYTE & availSets);
		virtual Error SetRegionCode(RootIterator * rit, BYTE region);

		//
		//  Internal helper functions
		//
	public:
		virtual Error BuildUniqueKey(void);
		virtual Error ObtainSubFileSystem(RootIterator * rit, GenericFileSystem * & gfs, DWORD dwFlags = 0);
		virtual Error ObtainSubFileSystem(RootIterator * rit);
		virtual Error ReleaseSubFileSystem(RootIterator * rit, BOOL forced = FALSE);
		Error FillMountNode(MountNode * node, DWORD dwFlags = 0);
		Error ClearMountNode(MountNode * node);

	public:
		RootFileSystem(void);
		virtual ~RootFileSystem(void);

		virtual Error Init(GenericProfile * profile, DriveTable * driveTable);

		//RootFileSystem doesn't support Obtain/Release (see NOTE above)
		virtual Error Obtain(void){GNRAISE_OK;}
		virtual Error Release(void){GNRAISE_OK;}

//		virtual Error CheckPath(KernelString drivePath, DiskInformation * & di);

		//
		//  Volume Information
		//

		virtual Error GetUniqueKey(BYTE * key);

		//
		//  Drive "Pass Through" Functions
		//

		virtual Error GetCurrentDriveBlockSize(GenericFileSystemIterator * gfsi, DWORD & size);
		virtual Error SetDriveBlockSize(GenericFileSystemIterator * gfsi, DWORD size);

		virtual Error GetNumberOfDriveBlocks(GenericFileSystemIterator * gfsi, DWORD & num);
		virtual Error SetNumberOfDriveBlocks(GenericFileSystemIterator * gfsi, DWORD num);

		virtual Error TerminateRequest(RequestHandle * rh);
//		virtual Error Flush(GenericFileSystemIterator * gfsi);

		//
		//  Drive handling
		//

		virtual Error SpinUpDrive(RootIterator * rit);
		virtual Error SpinDownDrive(RootIterator * rit);

		virtual Error LoadMedia(RootIterator * rit, int slotNumber = 0, DWORD flags = LMF_WAIT);
		virtual Error UnloadMedia(RootIterator * rit, DWORD flags = UMF_WAIT);
#if MSNDISC
		virtual Error GetDriveCaps(RootIterator * rit, DWORD& caps, DWORD& slots, DWORD& positions, DWORD& changerOpenOffset );
		virtual Error MoveChanger(RootIterator * rit, DWORD operation, DWORD flags, DWORD param );
		virtual Error GetChangerStatus(RootIterator * rit, DWORD& status, DWORD& position);
		virtual Error GetSlotStatus( RootIterator * rot, DWORD slot, DWORD& status );
		virtual Error LockClamp(RootIterator * rit);
		virtual Error UnlockClamp(RootIterator * rit);
#else
		virtual Error LockTray(RootIterator * rit);
		virtual Error UnlockTray(RootIterator * rit);
		virtual Error GetTrayStatus(RootIterator * rit, GenericTrayStatus & status);
#endif
		virtual Error AbortAndDisableDiskRequests(RootIterator * rit);
		virtual Error EnableDiskRequests(RootIterator * rit);

		//
		//  Volume handling
		//

		virtual Error GetVolumeName(RootIterator * rit, KernelString name);
		virtual Error GetVolumeSetName(RootIterator * rit, KernelString name);
		virtual Error GetVolumeSetIndex(RootIterator * rit, int index);
		virtual Error GetVolumeSetSize(RootIterator * rit, int size);
		virtual Error GetUniqueKey(RootIterator * rit, BYTE * key);
		virtual Error GetDiskInformation(RootIterator * rit, DiskInformation * & di);
	};

#endif
