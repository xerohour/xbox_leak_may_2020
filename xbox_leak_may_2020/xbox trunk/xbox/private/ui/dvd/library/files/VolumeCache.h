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
//  Volume Cache Classes
//
////////////////////////////////////////////////////////////////////

#ifndef VOLUMECACHE_H
#define VOLUMECACHE_H

#include "library/hardware/drives/generic/GenericDrive.h"

////////////////////////////////////////////////////////////////////
//
//  Volume Cache Block Class
//
////////////////////////////////////////////////////////////////////

class VolumeCacheBlock
	{
	friend class VolumeCache;

	protected:
		VolumeCacheBlock	*	next;
		DriveBlock				driveBlock;
		int						lockCnt;

	public:
		VolumeCacheBlock(DWORD blockSize);
		~VolumeCacheBlock(void);

		void Obtain(void) {lockCnt++;}
		void Release(void) {lockCnt--;}
	};

////////////////////////////////////////////////////////////////////
//
//  Volume Cache Class
//
////////////////////////////////////////////////////////////////////

class VolumeCache
	{
	protected:
		VDLocalMutex lock;
		VolumeCacheBlock * first;
		GenericDrive * drive;
		DWORD blockSize;

	public:
		VolumeCache(void);
		virtual ~VolumeCache(void);

		virtual Error Init(GenericDrive * drive, DWORD blockNum, DWORD blockSize);
		virtual Error Cleanup(void);

		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error Flush(void);
	};

#endif
