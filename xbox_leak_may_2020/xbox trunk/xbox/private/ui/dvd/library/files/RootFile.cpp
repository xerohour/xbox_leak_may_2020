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
////////////////////////////////////////////////////////////////////

#include "RootFile.h"

////////////////////////////////////////////////////////////////////
//
//  Internal constants
//
////////////////////////////////////////////////////////////////////

#define RF_INIT_DRIVE_BLOCK_NUM 150		// Defines the number of drive blocks a volume initially has

////////////////////////////////////////////////////////////////////
//
//  File system mount point (internal use only)
//
////////////////////////////////////////////////////////////////////

class MountNode : public Node
	{
	public:
		KernelString			mountName;
		GenericDrive		*	drive;			// This one is only valid if a drive is really connected
		GenericVolume		*	volume;			// The volume in that drive
		GenericFileSystem	*	fileSystem;		// One of the possible file systems on that volume
		WORD						userCount;		// Number of users for volume and file system

		MountNode(void) : Node() { drive = NULL; volume = NULL; fileSystem = NULL; userCount = 0; }
		~MountNode(void);
	};

//
//  Destructor
//

MountNode::~MountNode(void)
	{
	if (fileSystem)
		{
		//DP("From ~MountNode - FS R:");
		fileSystem->Release();
		}

	if (volume)
		{
		//DP("From ~MountNode - V R:");
		volume->Release();
		}
	}

////////////////////////////////////////////////////////////////////
//
//  Root directory class
//
////////////////////////////////////////////////////////////////////

#if GFS_ENABLE_DIRECTORIES

//
//  Constructor
//

RootDirectory::RootDirectory(RootFileSystem * rootfs) : GenericDirectory(rootfs)
	{
	this->rootfs = rootfs;
	}

//
//  Destructor
//

RootDirectory::~RootDirectory(void)
	{
	}

//
//  Open
//

Error RootDirectory::Open(GenericFileSystemIterator * gfsi, DWORD accessType)
	{
//	return rootfs->OpenDir(this, gfsi, accessType);
	GNRAISE_OK;
	}

//
//  Close
//

Error RootDirectory::Close(void)
	{
//	return rootfs->CloseDir(this);
	GNRAISE_OK;
	}

//
//  Get item name
//

Error RootDirectory::GetName(DiskItemName & name)
	{
	return rootfs->GetItemName(rit, name);
	}

//
//  Get path name
//

Error RootDirectory::GetPathName(DiskItemName & name)
	{
	return rootfs->GetPathName(rit, name);
	}

//
//  Get iterator
//

Error RootDirectory::GetIterator(GenericFileSystemIterator * & gfsi)
	{
	return rit->Clone(gfsi);
	}

//
//  Create item
//

Error RootDirectory::CreateItem(DWORD diskItemType, const DiskItemName & name, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Delete item
//

Error RootDirectory::DeleteItem(GenericFileSystemIterator * gfsi)
	{
	GNRAISE(GNR_DISK_READ_ONLY);
	}

//
//  Get number of items in dir
//

Error RootDirectory::GetNumberOfItems(DWORD & num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Find item
//

Error RootDirectory::FindItem(const DiskItemName & name, DiskItemType type, DWORD flags, GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

#endif

////////////////////////////////////////////////////////////////////
//
//  Root File System Iterator Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

RootIterator::RootIterator(GenericFileSystem * gfs) : GenericFileSystemIterator(gfs)
	{
	mountNode = NULL;
	((RootFileSystem*)gfs)->GoToFirstItem(this);
	}

//
//  Destructor
//

RootIterator::~RootIterator(void)
	{
//	((RootFileSystem*)gfs)->ReleaseSubFileSystem(this);
	}

//
//  Clone iterator
//

Error RootIterator::Clone(GenericFileSystemIterator * & gfsi) const
	{
	RootIterator * rit;

	GNREASSERT(Clone(rit));
	gfsi = rit;
	GNRAISE_OK;
	}

//
//  Clone iterator
//

Error RootIterator::Clone(RootIterator * & rit) const
	{
	rit = new RootIterator(gfs);
	if (rit)
		{
		rit->mountNode = mountNode;
//		((RootFileSystem*)gfs)->ObtainSubFileSystem(rit);
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Comparison function
//

BOOL RootIterator::Equals(GenericFileSystemIterator * gfsi)
	{
	return mountNode == ((RootIterator*)gfsi)->mountNode;
	}

//
//  Go to first item
//

Error RootIterator::GoToFirstItem(void)
	{
	return ((RootFileSystem*)gfs)->GoToFirstItem(this);
	}

//
//  Go to next item
//

Error RootIterator::GoToNextItem(void)
	{
	return ((RootFileSystem*)gfs)->GoToNextItem(this);
	}

//
//  Get subdirectory iterator
//

Error RootIterator::GoToSubDir(GenericFileSystemIterator * & gfsi)
	{
	return ((RootFileSystem*)gfs)->GoToSubDir(this, gfsi);
	}

//
//  Get parent dir iterator
//

Error RootIterator::GoToParentDir(GenericFileSystemIterator * & gfsi)
	{
	GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Get path name
//

Error RootIterator::GetPathName(DiskItemName & name)
	{
	name = DiskItemName("/");
	GNRAISE_OK;
	}

//
//  Get item type
//

Error RootIterator::GetItemType(DiskItemType & type)
	{
	return ((RootFileSystem*)gfs)->GetItemType(this, type);
	}

//
//  Get item name
//

Error RootIterator::GetItemName(DiskItemName & name)
	{
	return ((RootFileSystem*)gfs)->GetItemName(this, name);
	}

//
//  Get item size
//

Error RootIterator::GetItemSize(KernelInt64 & size)
	{
	return ((RootFileSystem*)gfs)->GetItemSize(this, size);
	}

//
//  Get number of items in dir
//

Error RootIterator::GetNumberOfItems(DWORD & num)
	{
	return ((RootFileSystem*)gfs)->GetNumberOfItems(this, num);
	}

//
//  Get current directory object
//

Error RootIterator::GetCurrentDir(GenericDirectory * & gd)
	{
	return ((RootFileSystem*)gfs)->GetDir(this, gd);
	}

//
//  Obtain sub file system
//

Error RootIterator::ObtainSubFileSystem(GenericFileSystem * & sfs, DWORD dwFlags)
	{
	return ((RootFileSystem*)gfs)->ObtainSubFileSystem(this, sfs, dwFlags);
	}

//
//  Release sub file system
//

Error RootIterator::ReleaseSubFileSystem(void)
	{
	return ((RootFileSystem*)gfs)->ReleaseSubFileSystem(this);
	}

//
//  Open item
//

Error RootIterator::OpenItem(DWORD accessType, GenericDiskItem * & gdi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get RPC data
//

Error RootIterator::GetRPCData(BOOL & isRPC2, BYTE & regionCode, BYTE & availSets)
	{
	return ((RootFileSystem*)gfs)->GetRPCData(this, isRPC2, regionCode, availSets);
	}

//
//  Set region code
//

Error RootIterator::SetRegionCode(BYTE region)
	{
	return ((RootFileSystem*)gfs)->SetRegionCode(this, region);
	}

////////////////////////////////////////////////////////////////////
//
//  Root File System Class
//
//  This class forms a single common tree out of all files/dirs
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

RootFileSystem::RootFileSystem(void)
	{
	fileSystemType = FS_ROOT;
	}

//
//  Destructor
//

RootFileSystem::~RootFileSystem(void)
	{
	MountNode * mn;

	//DP("Deleting RootFileSystem...");
	while (!mounts.IsEmpty())
		{
		mn = (MountNode*)mounts.Pop();
		delete mn;
		}
	//DP("RootFileSystem deleted");
	}

//
//  Init (build drive tables and mount the drives)
//

Error RootFileSystem::Init(GenericProfile * profile, DriveTable * driveTable)
	{
	DriveIterator it;
	MountNode * mn;
	Error err;

	//
	//  Get all drives from drive table and mount them into directory structure
	//

	err = driveTable->GoFirstDrive(it);
	while (!IS_ERROR(err))
		{
		mn = new MountNode();
		driveTable->GetDriveName(it, mn->mountName);
		driveTable->GetDrive(it, mn->drive);
		mn->volume = NULL;
		mn->fileSystem = NULL;
		mounts.InsertLast(mn);
		err = driveTable->GoNextDrive(it);
		}

	//
	//  Now add the file system folders
	//


	//
	//  Further drive tables like network etc. should be added here
	//

	GNRAISE_OK;
	}

//
//  Create file system for mount node
//  NOTE: This call also does an implicit Obtain()
//  The MountNode will be overwritten, existance of a FS/Volume has to be checked before calling this
//

Error RootFileSystem::CreateFileSystem(MountNode * mn, DWORD dwFlags)
	{
	RootIterator * rit = NULL;
	GenericVolume * volume;
	GenericFileSystem * fileSystem = NULL;
	PhysicalDiskType diskType;
	Error err = GNR_OK;

	//DP("RootFileSystem::CreateFileSystem");
	if (mn->fileSystem)
		BREAKPOINT;

	//
	//  Now create volume according to disk type
	//

	GNREASSERT(mn->drive->GetDiskType(diskType));
	switch (diskType)
		{
		case PHDT_NONE:
			GNRAISE(GNR_OBJECT_NOT_FOUND);
		case PHDT_CDDA:
			volume = new CDVDVolume();
			break;
		case PHDT_CDROM:
		case PHDT_CDI:
		case PHDT_CDROM_XA:
			volume = new CDVDVolume();
			break;
		case PHDT_DVD_ROM:
			volume = new CDVDVolume();
			break;
		case PHDT_DVD_RAM:
		case PHDT_DVD_R:
		case PHDT_DVD_RW:
		case PHDT_HD:
		default:
			GNRAISE(GNR_OBJECT_INVALID);
		}

	if (!volume)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Initialize volume
	//

	if (!IS_ERROR(err))
		err = volume->Init(mn->drive, profile);

	if (!IS_ERROR(err))
		err = volume->SetNumberOfDriveBlocks(RF_INIT_DRIVE_BLOCK_NUM);

	//
	//  Now create file system according to disk type
	//

	if (!IS_ERROR(err))
		{
		switch (diskType)
			{
			case PHDT_NONE:
				GNRAISE(GNR_OBJECT_NOT_FOUND);
			case PHDT_CDDA:
				fileSystem = new CDFileSystem();
				break;
			case PHDT_CDROM:
			case PHDT_CDI:
			case PHDT_CDROM_XA:
					if (dwFlags & FS_CD_FILE_SYSTEM_REQUIRED)
						fileSystem = new CDFileSystem();
					else
						fileSystem = new ISOFileSystem();
				break;
			case PHDT_DVD_ROM:
				fileSystem = new UDFFileSystem();
				break;
			case PHDT_DVD_RAM:
			case PHDT_DVD_R:
			case PHDT_DVD_RW:
			case PHDT_HD:
			default:
				GNRAISE(GNR_OBJECT_INVALID);
			}

		if (!fileSystem)
			{
			delete volume;
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			}
		}

	//
	//  Create mount point iterator
	//

	if (!IS_ERROR(err))
		{
		err = CreateIterator(rit);
//		rit->mountNode = NULL;
		rit->mountNode = mn;
		}

	//
	//  Initialize file system
	//

	if (!IS_ERROR(err))
		err = fileSystem->Init(rit, volume, profile);	// The file system has to clone the iterator

	//
	//  Cleanup
	//

	if (IS_ERROR(err))
		{
		delete fileSystem;	// Here delete instead of Release() is correct since nobody else can have obtained
		delete volume;			// this object and at least the FileSystem is not initiated
		}
	else
		{
		mn->volume = volume;
		mn->fileSystem = fileSystem;
		}

	delete rit;
	GNRAISE(err);
	}

//
//  Fill mount node
//

Error RootFileSystem::FillMountNode(MountNode * node, DWORD dwFlags /* = 0 */)
	{
	VolumeID volumeID1;
	VolumeID volumeID2;

	if (!node)
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);

	if (node->volume)		// At the moment there is no FS without volume. This might change ???
		{
		GNREASSERT(node->volume->GetVolumeID(volumeID1));
		GNREASSERT(node->drive->GetVolumeID(volumeID2));
		if (volumeID1 == volumeID2)
			{
			if (dwFlags & FS_CD_FILE_SYSTEM_REQUIRED) //Use this flag with caution - the caller
				//must ensure that all the references to file system are released (ref. count
				//should be one at this point). This is in case node has to be cleared.
				{
				if (node->fileSystem->GetFileSystemType() == FS_CD)
					GNRAISE_OK;
				}
			else
				{
				GNRAISE_OK;
				}
			}

		ClearMountNode(node);
		}

	GNRAISE(CreateFileSystem(node, dwFlags));
	}

//
//  Clear Mount Node
//

Error RootFileSystem::ClearMountNode(MountNode * node)
	{
	//DP("ClearMountNode");
	if (node->fileSystem)
		{
		//DP("From ClearMountNode - FS R:");
		node->fileSystem->Release();
		node->fileSystem = NULL;
		}

	if (node->volume)
		{
		//DP("From ClearMountNode - V R:");
		node->volume->Release();
		node->volume = NULL;
		}

	GNRAISE_OK;
	}

//
//  Obtain sub file system
//

Error RootFileSystem::ObtainSubFileSystem(RootIterator * rit, GenericFileSystem * & gfs, DWORD dwFlags /* = 0 */)
	{
	GNREASSERT(FillMountNode(rit->mountNode, dwFlags));

	//DP("From ObtainSubFileSystem - FS O: ");
	rit->mountNode->fileSystem->Obtain();
	gfs = rit->mountNode->fileSystem;
	GNRAISE_OK;
	}

//
//  Obtain sub file system
//

Error RootFileSystem::ObtainSubFileSystem(RootIterator * rit)
	{
	GenericFileSystem * gfs;

//	DP("From ObtainSFS2: ");
	GNRAISE(ObtainSubFileSystem(rit, gfs));
	}

//
//  Release sub file system
//

Error RootFileSystem::ReleaseSubFileSystem(RootIterator * rit, BOOL forced)
	{
	if (!rit->mountNode || !rit->mountNode->fileSystem)
		GNRAISE(GNR_OBJECT_INVALID);

	//DP("From ReleaseSubFileSystem - FS R: ");
	GNRAISE(rit->mountNode->fileSystem->Release());
	}

//
//  Move iterator to first item
//

Error RootFileSystem::GoToFirstItem(RootIterator * rit)
	{
	//
	//  Obtain new node
	//

	rit->mountNode = (MountNode*)mounts.First();
	if (rit->mountNode)		// This ensures, that we have mounts at all
		GNRAISE_OK;
	else
		GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Move iterator to next item
//

Error RootFileSystem::GoToNextItem(RootIterator * rit)
	{
	if (rit->mountNode && !rit->mountNode->IsLast())
		{
		rit->mountNode = (MountNode*)(rit->mountNode->Succ());
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_ITEM_NOT_FOUND);
	}

//
//  Create new iterator in sub dir
//

Error RootFileSystem::GoToSubDir(RootIterator * rit, GenericFileSystemIterator * & gfsi)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->CreateIterator(gfsi);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Access methods, all not supported here
//

Error RootFileSystem::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::BuildUniqueKey(void)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::GetUniqueKey(BYTE * key)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::GetCurrentDriveBlockSize(GenericFileSystemIterator * gfsi, DWORD & size)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::SetDriveBlockSize(GenericFileSystemIterator * gfsi, DWORD size)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::GetNumberOfDriveBlocks(GenericFileSystemIterator * gfsi, DWORD & num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::SetNumberOfDriveBlocks(GenericFileSystemIterator * gfsi, DWORD num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error RootFileSystem::CreateFile(DWORD flags, GenericFile * & gf)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Create iterator
//

Error RootFileSystem::CreateIterator(RootIterator * & rit)
	{
	rit = new RootIterator(this);
	if (rit)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create iterator
//

Error RootFileSystem::CreateIterator(GenericFileSystemIterator * & gfsi)
	{
	gfsi = new RootIterator(this);
	if (gfsi)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  Create directory
//

Error RootFileSystem::CreateDirectory(DWORD flags, GenericDirectory * & gd)
	{
#if GFS_ENABLE_DIRECTORIES
	gd = new RootDirectory(this);
	if (gd)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
#else
	GNRAISE(GNR_UNIMPLEMENTED);
#endif
	}

//
//  Get path name
//

Error RootFileSystem::GetPathName(RootIterator * rit, DiskItemName & name)
	{
	return name.GetDefaultSeparator();
	}

//
//  Get item type
//

Error RootFileSystem::GetItemType(RootIterator * rit, DiskItemType & type)
	{
	type = DIT_DIR;
	GNRAISE_OK;
	}

//
//  Get item name
//

Error RootFileSystem::GetItemName(RootIterator * rit, DiskItemName & name)
	{
	name = rit->mountNode->mountName;
	GNRAISE_OK;
	}

//
//  Get item size
//

Error RootFileSystem::GetItemSize(RootIterator * rit, KernelInt64 & size)
	{
	size = 0;
	GNRAISE_OK;
	}

//
//  Get directory
//

Error RootFileSystem::GetDir(RootIterator * rit, GenericDirectory * & gd)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get number of items in dir
//

Error RootFileSystem::GetNumberOfItems(RootIterator * rit, DWORD & num)
	{
	num = mounts.Num();
	GNRAISE_OK;
	}

//
//  Terminate request
//

Error RootFileSystem::TerminateRequest(RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Spin up drive
//

Error RootFileSystem::SpinUpDrive(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->fileSystem)
		GNRAISE(rit->mountNode->fileSystem->SpinUpDrive());
	else
		GNRAISE(GNR_NO_FILE_SYSTEM);
	}

//
//  Spin down drive
//

Error RootFileSystem::SpinDownDrive(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->fileSystem)
		GNRAISE(rit->mountNode->fileSystem->SpinDownDrive());
	else
		GNRAISE(GNR_NO_FILE_SYSTEM);
	}


#if MSNDISC

Error RootFileSystem::GetDriveCaps(RootIterator * rit, DWORD& caps, DWORD& slots, DWORD& positions, DWORD& changerOpenOffset )
	{
	GenericDriveType	type;

	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->GetDriveCaps(type,caps,slots,positions,changerOpenOffset));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::MoveChanger(RootIterator * rit, DWORD operation, DWORD flags, DWORD param )
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->MoveChanger(operation,flags,param));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::GetChangerStatus(RootIterator * rit, DWORD& status, DWORD& position)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->GetChangerStatus(status, position));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::GetSlotStatus(RootIterator * rit, DWORD slot, DWORD& status )
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->GetSlotStatus(slot, status));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::LockClamp(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->LockClamp());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::UnlockClamp(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->UnlockClamp());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::LoadMedia(RootIterator * rit, int slotNumber, DWORD flags)
	{
	DWORD changerStatus;
	DWORD changerPosition;
	GNREASSERT(GetChangerStatus(rit, changerStatus, changerPosition));
	if (rit->mountNode && rit->mountNode->drive)
		{
		GNREASSERT(rit->mountNode->drive->LoadMedia(slotNumber, flags));
		GNRAISE(LockClamp(rit));
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::UnloadMedia(RootIterator * rit, DWORD flags)
	{
	if (rit->mountNode && rit->mountNode->drive)
		{
		GNREASSERT(UnlockClamp(rit));
		GNRAISE(rit->mountNode->drive->UnloadMedia(flags));
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

Error RootFileSystem::GetRPCData(RootIterator * rit, BOOL & isRPC2, BYTE & regionCode, BYTE & availSets)
	{
	GenericDriveType type;
	DWORD caps;
	DWORD slots;
	DWORD positions;
	DWORD changerOpenOffset;

	GNREASSERT(rit->mountNode->drive->GetDriveCaps(type, caps, slots,positions,changerOpenOffset));
	if (type = GDRT_CDVD)
		GNRAISE(((CDVDDrive*)rit->mountNode->drive)->GetRPCData(isRPC2, regionCode, availSets));
	else
		GNRAISE(GNR_OBJECT_INVALID);
	}

Error RootFileSystem::SetRegionCode(RootIterator * rit, BYTE region)
	{
	GenericDriveType type;
	DWORD caps;
	DWORD slots;
	DWORD positions;
	DWORD changerOpenOffset;

	GNREASSERT(rit->mountNode->drive->GetDriveCaps(type, caps, slots,positions,changerOpenOffset));
	if (type = GDRT_CDVD)
		GNRAISE(((CDVDDrive*)rit->mountNode->drive)->SetRegionCode(region));
	else
		GNRAISE(GNR_OBJECT_INVALID);
	}

#else


//
//  Load media
//

Error RootFileSystem::LoadMedia(RootIterator * rit, int slotNumber, DWORD flags)
	{
	GenericTrayStatus trayStatus;

	GNREASSERT(GetTrayStatus(rit, trayStatus));
	if (rit->mountNode && rit->mountNode->drive)
		{
		GNREASSERT(rit->mountNode->drive->LoadMedia(slotNumber, flags));
		GNRAISE(LockTray(rit));
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Undload media
//

Error RootFileSystem::UnloadMedia(RootIterator * rit, DWORD flags)
	{
	if (rit->mountNode && rit->mountNode->drive)
		{
		GNREASSERT(UnlockTray(rit));
		GNRAISE(rit->mountNode->drive->UnloadMedia(flags));
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get tray status
//

Error RootFileSystem::GetTrayStatus(RootIterator * rit, GenericTrayStatus & status)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->GetTrayStatus(status));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Lock tray
//

Error RootFileSystem::LockTray(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->LockTray());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Unlock tray
//

Error RootFileSystem::UnlockTray(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		GNRAISE(rit->mountNode->drive->UnlockTray());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

#endif

//
// Abort current disk-access operation and disable all disk
// requests until EnableDiskRequests() is called
//

Error RootFileSystem::AbortAndDisableDiskRequests(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		{
		GNREASSERT(rit->mountNode->drive->TerminateRequest(NULL));
		rit->mountNode->drive->DisableRequests();
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
// Enable disk requests - to be used after AbortAndDisableDiskRequests()
//

Error RootFileSystem::EnableDiskRequests(RootIterator * rit)
	{
	if (rit->mountNode && rit->mountNode->drive)
		{
		rit->mountNode->drive->EnableRequests();
		//ClearMountNode(rit->mountNode);
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get RPC data
//

#if MSNDISC
#else
Error RootFileSystem::GetRPCData(RootIterator * rit, BOOL & isRPC2, BYTE & regionCode, BYTE & availSets)
	{
	GenericDriveType type;
	DWORD caps;
	DWORD slots;

	GNREASSERT(rit->mountNode->drive->GetDriveCaps(type, caps, slots));
	if (type = GDRT_CDVD)
		GNRAISE(((CDVDDrive*)rit->mountNode->drive)->GetRPCData(isRPC2, regionCode, availSets));
	else
		GNRAISE(GNR_OBJECT_INVALID);
	}
#endif
//
//  Set region code
//

#if MSNDISC
#else
Error RootFileSystem::SetRegionCode(RootIterator * rit, BYTE region)
	{
	GenericDriveType type;
	DWORD caps;
	DWORD slots;

	GNREASSERT(rit->mountNode->drive->GetDriveCaps(type, caps, slots));
	if (type = GDRT_CDVD)
		GNRAISE(((CDVDDrive*)rit->mountNode->drive)->SetRegionCode(region));
	else
		GNRAISE(GNR_OBJECT_INVALID);
	}
#endif
//
//  Get volume name
//

Error RootFileSystem::GetVolumeName(RootIterator * rit, KernelString name)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->GetVolumeName(name);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Get volume set name
//

Error RootFileSystem::GetVolumeSetName(RootIterator * rit, KernelString name)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->GetVolumeSetName(name);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Get volume set index
//

Error RootFileSystem::GetVolumeSetIndex(RootIterator * rit, int index)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->GetVolumeSetIndex(index);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Get volume set size
//

Error RootFileSystem::GetVolumeSetSize(RootIterator * rit, int size)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->GetVolumeSetSize(size);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Get unique key for drive
//

Error RootFileSystem::GetUniqueKey(RootIterator * rit, BYTE * key)
	{
	Error err;

	GNREASSERT(ObtainSubFileSystem(rit));
	err = rit->mountNode->fileSystem->GetUniqueKey(key);
	GNREASSERT(ReleaseSubFileSystem(rit));
	GNRAISE(err);
	}

//
//  Get disk information
//

Error RootFileSystem::GetDiskInformation(RootIterator * rit, DiskInformation * & di)
	{
	GNREASSERT(ObtainSubFileSystem(rit));
	GNREASSERT(rit->mountNode->drive->GetDriveName(di->driveName));
	GNREASSERT(rit->mountNode->drive->GetDiskType(di->diskType));
	GNREASSERT(rit->mountNode->fileSystem->GetVolumeName(di->volumeName));
	GNREASSERT(rit->mountNode->fileSystem->GetUniqueKey(di->uniqueKey));
	GNREASSERT(GetRPCData(rit, di->isRPC2, di->driveRegion, di->availSets));
	GNREASSERT(ReleaseSubFileSystem(rit));
//	diskRegion;?????
	GNRAISE_OK;
	}
