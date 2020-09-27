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
//  Generic Volume Classes
//
////////////////////////////////////////////////////////////////////

#ifndef GENERICVOLUME_H
#define GENERICVOLUME_H

class GenericVolume;

#include "library/hardware/drives/generic/GenericDrive.h"
#include "library/Files/VolumeCache.h"
#include "library/Files/GenericFile.h"
#include "library/common/krnlstr.h"
#include "library/common/krnlsync.h"

////////////////////////////////////////////////////////////////////
//
//  Volume Information
//
////////////////////////////////////////////////////////////////////

class VolumeInfo
	{
	public:
		DWORD attributes;
	};

//
//  Volume Information Attributes
//

#define VIA_NONE							0
#define VIA_CONTAINS_AUDIO_TRACKS	1

////////////////////////////////////////////////////////////////////
//
//  Generic Volume Declaration
//
////////////////////////////////////////////////////////////////////

class GenericVolume
	{
	protected:
		GenericDrive * genericDrive;
	public:
		int refCount;
	protected:
		VDLocalMutex monitor;
		DWORD maxBlockSize;
		VolumeID volumeID;

	public:
		GenericVolume(void);
		virtual ~GenericVolume(void);

		//
		//  Init/Cleanup
		//

		virtual Error Init(GenericDrive * drive, GenericProfile * profile);
		virtual Error Obtain(void);
		virtual Error Release(void);
		virtual Error DetachDrive(void);
		virtual Error GetVolumeID(VolumeID & volumeID);

		//
		//  Drive Handling
		//

		virtual Error GetDiskType(PhysicalDiskType & type);
		virtual Error GetVolumeInfo(VolumeInfo & volumeInfo);
		virtual Error GetDriveName(KernelString & name);
#if MSNDISC
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset);
#else
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
#endif
		virtual Error SpinUpDrive(void);
		virtual Error SpinDownDrive(void);
		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & mode);
		virtual Error TerminateRequest(RequestHandle * rh);

		//
		//  General Access Methods
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		virtual Error Flush(void);

		virtual Error GetDriveBlockSize(DWORD sectorType, DWORD & size, DWORD & minSize, DWORD & maxSize, DWORD & step);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		virtual Error GetBlockInfo(DWORD sectorType, DWORD & blockSize, DWORD & headerSize, DWORD & dataSize);
	};

#endif
