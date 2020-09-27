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

#include "VolumeCache.h"


////////////////////////////////////////////////////////////////////
//
//  Volume Cache Block Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VolumeCacheBlock::VolumeCacheBlock(DWORD blockSize)
	{
	driveBlock.data = (BYTE *) ::VirtualAlloc(NULL, blockSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

	if (driveBlock.data == NULL)
		BREAKPOINT;

	driveBlock.block = 0xffffffff;
	lockCnt = 0;
	}

//
//  Destructor
//

VolumeCacheBlock::~VolumeCacheBlock(void)
	{
	::VirtualFree (driveBlock.data, 0, MEM_RELEASE);
	}

////////////////////////////////////////////////////////////////////
//
//  Volume Cache Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

VolumeCache::VolumeCache(void)
	{
	first = NULL;
	}

//
//  Destructor
//

VolumeCache::~VolumeCache(void)
	{
	Cleanup();
	}

//
//  Initialize
//

Error VolumeCache::Init(GenericDrive * drive, DWORD blockNum, DWORD blockSize)
	{
	VDAutoMutex mutex(&lock);
	VolumeCacheBlock * block;
	DWORD i;

	//
	//  Make sure it is not initialized twice
	//

	if (first)
		GNRAISE(GNR_OBJECT_IN_USE);

	//
	//  Instantiate all the buffers
	//

	this->drive = drive;
	this->blockSize = blockSize;

	for (i=0; i<blockNum; i++)
		{
		block = new VolumeCacheBlock(blockSize);
		if (block)
			{
			block->next = first;
			first = block;
			}
		else
			{
			Cleanup();
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			}
		}

	GNRAISE_OK;
	}

//
//  Cleanup
//

Error VolumeCache::Cleanup(void)
	{
	VDAutoMutex mutex(&lock);
	VolumeCacheBlock * block;

	while (first)
		{
		block = first;
		first = first->next;
		delete block;
		}

	first = NULL;
	GNRAISE_OK;
	}

//
//  Lock blocks
//

Error VolumeCache::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);
	VolumeCacheBlock * search, * searchPred, * lastFree, * lastFreePred;
	DriveBlock db;
	Error err = GNR_OK, helpErr;
	DWORD i;

	flags &= ~GD_COMMAND_MASK;
//	DP("LockBlocks %d %d", block, num);
	for (i=0; i<num; i++)
		{
		//
		//  Initialize search pointers
		//

		search = first;			// Search pointer
		searchPred = NULL;		// Predecessor of search pointer
		lastFree = NULL;			// Last free block in buffer (lock count == 0)
		lastFreePred = NULL;		// Predecessor of last free block

		//
		//  Find block in cache
		//

		while (search && (search->driveBlock.block != block + i))
			{
			if (search->lockCnt == 0)
				{
				lastFreePred = searchPred;
				lastFree =  search;
				}

			searchPred = search;
			search = search->next;
			}

		//
		//  If we found the block, then make it first one in order (LRU)
		//  else take least recently used and not locked block
		//

		if (search)
			{
			//
			//  Make if first if it is not already the first one
			//

			if (searchPred)
				{
				searchPred->next = search->next;
				search->next = first;
				first = search;
				}
			}
		else
			{
			//
			//  Make sure that we found a free block at all
			//

			if (!lastFree)
				GNRAISE(GNR_NOT_ENOUGH_MEMORY);

			//
			//  Make it first if it is not already the first one
			//

			if (lastFreePred)
				{
				lastFreePred->next = lastFree->next;
				lastFree->next = first;
				first = lastFree;
				}

			//
			//  Read data
			//

			search = lastFree;
			helpErr = drive->LockBlocks(block + i, 1, &db, flags | DAT_LOCK_AND_READ, rh);
			db.Copy(&(search->driveBlock));
			if (!IS_ERROR(helpErr))
				{
				memcpy(search->driveBlock.data, db.data, blockSize);
				}
			else
				{
				DP("Lock error occurred");
				if ((IS_WARNING(err) && IS_ERROR(helpErr)) || !IS_ERROR(err))
					err = helpErr;
				}
			drive->UnlockBlocks(block + i, 1, &db, flags | DAT_UNLOCK_CLEAN, rh);
			}

		//
		//  Lock the block and return its address
		//

		search->driveBlock.Copy(&(blocks[i]));
		blocks[i].data = search->driveBlock.data;
		search->Obtain();
		}

	GNRAISE(err);
	}

//
//  Unlock blocks
//

Error VolumeCache::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&lock);
	VolumeCacheBlock * search;
	DWORD i;

//	DP("UnlockBlocks %d %d", block, num);
	for (i=0; i<num; i++)
		{
		search = first;
		while (search)
			{
			if (search->driveBlock.block == block + i)
				{
				search->Release();
				break;
				}
			search = search->next;
			}
		if (!search)
			DP("Couldn't find block %d", block + i);
		}

	GNRAISE_OK;
	}

//
//  Flush
//

Error VolumeCache::Flush(void)
	{
	VDAutoMutex mutex(&lock);
	VolumeCacheBlock * block;

	block = first;
	while (block)
		{
		block->driveBlock.block = 0xffffffff;
		block = block->next;
		}

	GNRAISE_OK;
	}
