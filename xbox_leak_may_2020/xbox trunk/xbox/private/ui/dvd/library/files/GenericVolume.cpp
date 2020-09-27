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
//  Generic Volume Implementation
//
////////////////////////////////////////////////////////////////////

#include "GenericVolume.h"

//
//  Constructor
//

GenericVolume::GenericVolume(void)
	{
	genericDrive = NULL;
	refCount = 0;
	}

//
//  Destructor
//

GenericVolume::~GenericVolume(void)
	{
	//DPF("~GenericVolume");
	if (genericDrive)
		{
		Flush();
		genericDrive = NULL;
		}
	}

//
//  Init
//

Error GenericVolume::Init(GenericDrive * drive, GenericProfile * profile)
	{
	DWORD size, minSize, step;

	//DPF("GenericVolume::Init, refCount 1");
	genericDrive = drive;
	refCount = 1;
	GNREASSERT(genericDrive->GetVolumeID(volumeID));
	return genericDrive->GetDriveBlockSize(DST_NONE, size, minSize, maxBlockSize, step); //we are only curious about maximum size here, so DST_NONE is OK
	}

//
//  Obtain volume
//

Error GenericVolume::Obtain(void)
	{
	VDAutoMutex mutex(&monitor);

	refCount++;
	//DPF("Obtain Volume, refCount %d\n", refCount);
	GNRAISE_OK;
	}

//
//  Release volume
//

Error GenericVolume::Release(void)
	{
		{
		VDAutoMutex mutex(&monitor);

		refCount--;
		}
	//DPF("Release Volume, refCount %d\n", refCount);

	if (refCount < 1)
		delete this;
	GNRAISE_OK;
	}

//
//  Detach drive
//

Error GenericVolume::DetachDrive(void)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		{
		Flush();
#if MSNDISC
		genericDrive->UnlockClamp();
#else
		genericDrive->UnlockTray();
#endif
		genericDrive = NULL;
		}

	GNRAISE_OK;
	}

//
//  Get Volume ID
//

Error GenericVolume::GetVolumeID(VolumeID & volumeID)
	{
	volumeID = this->volumeID;
	GNRAISE_OK;
	}

//
//  Get disk type
//

Error GenericVolume::GetDiskType(PhysicalDiskType & type)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetDiskType(type));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get volume information
//

Error GenericVolume::GetVolumeInfo(VolumeInfo & volumeInfo)
	{
	volumeInfo.attributes = VIA_NONE;
	GNRAISE_OK;
	}

//
//  Get drive name
//

Error GenericVolume::GetDriveName(KernelString & name)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetDriveName(name));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get drive capabilities
//

#if MSNDISC
Error GenericVolume::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetDriveCaps(type, caps, slots,positions,changerOpenOffset));
	else
		GNRAISE(GNR_NO_DRIVE);
	}
#else
Error GenericVolume::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetDriveCaps(type, caps, slots));
	else
		GNRAISE(GNR_NO_DRIVE);
	}
#endif
//
//  Spin drive up
//

Error	GenericVolume::SpinUpDrive(void)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->SpinUp());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Spin drive down
//

Error GenericVolume::SpinDownDrive(void)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->SpinDown());
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Seek block
//

Error GenericVolume::SeekBlock(DWORD block, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->SeekBlock(block, flags, rh));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Lock drive blocks
//

Error GenericVolume::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->LockBlocks(block, num, blocks, flags, rh));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Unlock drive blocks
//

Error GenericVolume::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->UnlockBlocks(block, num, blocks, flags, rh));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Flush data
//

Error GenericVolume::Flush(void)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->Flush());

	GNRAISE_OK;
	}

//
//  Get copy management mode
//

Error GenericVolume::GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetCopyManagementInfo(block, gcmi));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Terminate request
//

Error GenericVolume::TerminateRequest(RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->TerminateRequest(rh));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get drive block size
//

Error GenericVolume::GetDriveBlockSize(DWORD sectorType, DWORD & currentSize, DWORD & minSize, DWORD & maxSize, DWORD & step)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->GetDriveBlockSize(sectorType, currentSize, minSize, maxSize, step));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Set drive block size
//

Error GenericVolume::SetDriveBlockSize(DWORD size)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		return genericDrive->SetDriveBlockSize(size);
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get number of drive blocks
//

Error GenericVolume::GetNumberOfDriveBlocks(DWORD & num)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		return genericDrive->GetNumberOfDriveBlocks(num);
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Set number of drive blocks
//

Error GenericVolume::SetNumberOfDriveBlocks(DWORD num)
	{
	VDAutoMutex mutex(&monitor);

	if (genericDrive)
		GNRAISE(genericDrive->SetNumberOfDriveBlocks(num));
	else
		GNRAISE(GNR_NO_DRIVE);
	}

//
//  Get block info
//

Error GenericVolume::GetBlockInfo(DWORD sectorType, DWORD & blockSize, DWORD & headerSize, DWORD & dataSize)
	{
	if (genericDrive)
		GNRAISE(genericDrive->GetBlockInfo(sectorType, blockSize, headerSize, dataSize));
	else
		GNRAISE(GNR_NO_DRIVE);
	}
