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
//  Drive Block Buffer Classes
//
////////////////////////////////////////////////////////////////////

#include "DriveBlockBuffer.h"
#include "library/common/vddebug.h"

//
//  Return values for FindLockedBlock method
//

#define FB_BLOCK_NOT_FOUND					0xffffffff

////////////////////////////////////////////////////////////////////
//
//  Scatter Drive Block Buffer Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

ScatterDriveBlockBuffer::ScatterDriveBlockBuffer(void)
	{
	blockNum = 64;
	blockSize = 2048;
	freeBlocks = blockNum;
	data = NULL;
	info = NULL;
	indices = NULL;
	Resize(blockNum, blockSize);
	}

//
//  Destructor
//

ScatterDriveBlockBuffer::~ScatterDriveBlockBuffer(void)
	{
	delete[] info;
	delete[] indices;
	}

//
//  Resize buffer
//  GNR_OBJECT_IN_USE: There are some locked blocks
//  GNR_NOT_ENOUGH_MEMORY: Not enough memory for so many blocks
//

Error ScatterDriveBlockBuffer::Resize(DWORD blockNum, DWORD blockSize)
	{
	if (freeBlocks != this->blockNum)
		GNRAISE(GNR_OBJECT_IN_USE);

	if (this->blockNum != blockNum || this->blockSize != blockSize)
		{
		this->blockNum = blockNum;
		this->blockSize = blockSize;
		freeBlocks = blockNum;

		//
		//  Resize data block
		//

		if (data)
			FreeData();
		AllocateData(blockNum * blockSize);
		if (data == NULL)
			{
			blockNum = blockSize = 0;
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			}

		//
		//  Resize info block
		//

		delete[] info;
		info = new ScatterDriveBlockInfo[blockNum + 2];	// Two more for ease of programming
		if (info == NULL)
			{
			FreeData();
			data = NULL;
			blockNum = blockSize = NULL;
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
			}

		InitInfo();

		//
		//  Resize indices array
		//

		delete[] indices;
		indices = new DWORD[blockNum];
		}

	GNRAISE_OK;
	}

//
//  Initialize control data
//

void ScatterDriveBlockBuffer::InitInfo(void)
	{
	DWORD i;

	if (info != NULL)
		{
		for (i=0; i<blockNum; i++)
			{
			info[i].lockCount = 0;
			info[i].succ = blockNum - i;
			info[i].lockMode = LM_NONE;
			}

		info[blockNum].lockCount = 0;
		info[blockNum].succ = 0;
		info[blockNum].lockMode = LM_NONE;

		info[blockNum + 1].lockCount = 0;
		info[blockNum + 1].succ = 0;
		info[blockNum + 1].lockMode = LM_NONE;
		}

	freeBlocks = blockNum;

	nextSearchPosition = 0;
	}

//
//  Set number of drive blocks
//  GNR_INVALID_PARAMETERS: Illegal value specified
//

Error ScatterDriveBlockBuffer::SetBlockNum(DWORD blockNum)
	{
	if (blockNum == 0)
		GNRAISE(GNR_INVALID_PARAMETERS);

	GNRAISE(Resize(blockNum, this->blockSize));
	}

//
//  Get number of drive blocks
//

Error ScatterDriveBlockBuffer::GetBlockNum(DWORD & num)
	{
	num = blockNum;
	GNRAISE_OK;
	}

//
//  Set drive block size
//  GNR_INVALID_PARAMETERS: Illegal value specified
//

Error ScatterDriveBlockBuffer::SetBlockSize(DWORD blockSize)
	{
	if (blockSize == 0)
		GNRAISE(GNR_INVALID_PARAMETERS);

	GNRAISE(Resize(this->blockNum, blockSize));
	}

//
//  Get drive block size
//

Error ScatterDriveBlockBuffer::GetBlockSize(DWORD & size)
	{
	size = blockSize;
	GNRAISE_OK;
	}

//
//  Lock blocks
//  This one returns the number of contiguously locked
//  blocks in private0 of the first block of each segment.
//  For previously locked blocks this value is 0.
//  GNR_NOT_EOUGH_MEMORY: Not enough free blocks (only for those who need to be newly allocated)
//  GNR_BLOCK_ALREADY_LOCKED: Attempt to read-lock a write-locked block or to write lock an already locked block (read or write)
//

Error ScatterDriveBlockBuffer::LockBlocks(DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * driveBlocks)
	{
	DWORD contNum;			// Number of contiguous blocks
	DWORD blocksLeft;		// Blocks left to lock
	DWORD lockedBlock;	// Index of already locked block
	DWORD i;


	//
	//  Lock blocks in maximal portions
	//

	blocksLeft = num;				// Number of blocks left to lock
	i = 0;

	while (blocksLeft)
		{
		//
		//  Find block which is already locked
		//

		contNum = 0;
		while (contNum < blocksLeft && (lockedBlock = FindLockedBlock(block + i + contNum)) == FB_BLOCK_NOT_FOUND)
			contNum++;

		//
		//  Mark blocks as lockable if there is enough free space
		//

		if (contNum > freeBlocks)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);

		if (contNum)
			{
			driveBlocks[i].lockMode = LM_NONE;
			driveBlocks[i].private0 = contNum;
			i += contNum;
			blocksLeft -= contNum;
			}

		//
		//  There may be multiple reads but only one write at a time
		//

		if (lockedBlock != FB_BLOCK_NOT_FOUND)
			{
			if (info[lockedBlock].lockMode == LM_WRITE || lockMode == LM_WRITE)
				GNRAISE(GNR_BLOCK_ALREADY_LOCKED);

			driveBlocks[i].lockMode = lockMode;
			driveBlocks[i].private0 = lockedBlock;
			i++;
			blocksLeft--;
			}
		}

	//
	//  We found space for all blocks, now let's really lock them
	//

	i = 0;
	while (i < num)
		{
		if (driveBlocks[i].lockMode == LM_NONE)
			{
			contNum = driveBlocks[i].private0;
			LockFreeBlocks(block + i, driveBlocks[i].private0, lockMode, &(driveBlocks[i]));
			i += contNum;
			}
		else
			{
			RelockBlocks(driveBlocks[i].private0, 1, &(driveBlocks[i]));
			driveBlocks[i].private0 = 1;
			i++;
			}
		}

#if DBB_ENABLE_CHECK
	Check();
#endif

	GNRAISE_OK;
	}

//
//  Lock free blocks
//  The availability of enough room must be asserted by caller.
//  This function assumes that none of the blocks (block <= b < block + num) is already locked.
//

void ScatterDriveBlockBuffer::LockFreeBlocks(DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * driveBlocks)
	{
	DWORD smallestFit;	// Smallest block in which whole request fits (pointer to dummy element)
	DWORD largestSub;		// Largest subset of request (pointer to dummy element)
	DWORD searchIdx;
	DWORD succs;

	info[blockNum].succ = blockNum;		// Initialize dummy element
	info[blockNum + 1].succ = 0;			// Initialize dummy element

	//
	//  Repeat searching until all blocks are allocated
	//

	while (num)
		{
		searchIdx = 0;
		smallestFit = blockNum;
		largestSub = blockNum + 1;

		//
		//  Find either minimal contiguous set of blocks for whole request or maximal pieces
		//

		while (searchIdx < blockNum)
			{
			if (info[searchIdx].lockMode == LM_NONE)
				{
				if (info[searchIdx].succ >= num)
					{
					if (info[searchIdx].succ <= info[smallestFit].succ)	// == is important to skip dummy element
						{
						smallestFit = searchIdx;
						}
					}
				else
					{
					if (info[searchIdx].succ >= info[largestSub].succ)		// == is important to skip dummy element
						{
						largestSub = searchIdx;
						}
					}
				}

			searchIdx += info[searchIdx].succ;
			}

		//
		//  Now allocate blocks pointed to by either smallest fit or largest sub
		//

		if (smallestFit != blockNum)
			{
			LockBlockSeq(smallestFit, block, num, lockMode, driveBlocks);
			num = 0;
			}
		else	// No need to check largest sub here since existence of enough blocks is assured
			{
			succs = info[largestSub].succ;	// This really sucks. info[largestSub].succ might change inside LockBlockSeq()
			LockBlockSeq(largestSub, block, succs, lockMode, driveBlocks);
			num -= succs;
//			if (num > 0xffff)
//				BREAKPOINT;
			driveBlocks += succs;
			block += succs;
			}
		}
	}

//
//  Lock block sequence (for free blocks only)
//  start: index into info structure
//  block: index of the block on disk
//

void ScatterDriveBlockBuffer::LockBlockSeq(DWORD start, DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * driveBlocks)
	{
	int i;
	DWORD numSucc;

	//
	//  Determine number of successors
	//

	numSucc = num;
	if (start + num < blockNum && info[start + num].lockMode != LM_NONE)
		numSucc += info[start + num].succ;

	//
	//  Lock the blocks
	//

	for (i=0; i<(int)num; i++)
		{
		info[start + i].lockCount = 1;
		info[start + i].succ = numSucc - i;
		info[start + i].block = block + i;
		info[start + i].lockMode = lockMode;
		driveBlocks[i].block = block + i;
		driveBlocks[i].data = data + (start + i) * blockSize;
		driveBlocks[i].lockMode = info[start + i].lockMode;
		driveBlocks[i].private0 = num - i;
		}

	//
	//  Update info of predecessor blocks
	//

	if (start > 0)
		{
		i = ((int)start) - 1;

		if (info[i].lockMode == LM_NONE)
			{
			//
			//  If blocks are free reduce successor count
			//  NOTE: This should never happen since we always allocate at the
			//  beginnig of a free block segment, but is here for completeness
			//

			while (i >= 0 && info[i].lockMode == LM_NONE)
				{
				info[i].succ -= num;
				i--;
				}
			}
		else
			{
			//
			//  If blocks were locked with same mode increase successor count
			//

			while (i >= 0 && info[i].lockMode != LM_NONE)
				{
				info[i].succ += numSucc;
				i--;
				}
			}
		}

	freeBlocks -= num;

#if DBB_ENABLE_CHECK
	Check();
#endif
	}

//
//  Relock already locked blocks
//  start: index into info structure
//

void ScatterDriveBlockBuffer::RelockBlocks(DWORD start, DWORD num, DriveBlock * driveBlocks)
	{
	DWORD i;

	//
	//  Allocate blocks
	//

	for (i=0; i<num; i++)
		{
		info[start + i].lockCount++;
		driveBlocks[i].block = info[start + i].block;
		driveBlocks[i].data = data + (start + i) * blockSize;
		driveBlocks[i].lockMode = info[start + i].lockMode;
		driveBlocks[i].private0 = info[start + i].succ;
		}
	}

//
//  Find locked block
//  Returns index in buffer or FB_BLOCK_NOT_FOUND
//

DWORD ScatterDriveBlockBuffer::FindLockedBlock(DWORD index)
	{
	DWORD i;

	for (i=0; i<blockNum; i++)
		{
		if (info[nextSearchPosition].block == index && info[nextSearchPosition].lockMode != LM_NONE)
			{
			return nextSearchPosition;
			}
//		nextSearchPosition = (nextSearchPosition + 1) % blockNum;
		nextSearchPosition++;
		if (nextSearchPosition == blockNum)
			nextSearchPosition = 0;
		}

	return FB_BLOCK_NOT_FOUND;
	}

//
//  Unlock blocks
//  GNR_NOT_ENOUGH_MEMORY: Attempt to unlock more blocks than there are
//  GNR_BLOCK_NOT_LOCKED: Attempt to unlock a block that is not locked
//

Error ScatterDriveBlockBuffer::UnlockBlocks(DWORD block, DWORD num)
	{
	int		i;
	int		j;
	DWORD		db;					// The block to unlock (index in info structure, not on drive)
	DWORD		prevSucc;			// Number of successors the block had before unlock
	BOOL		succFree;			// TRUE if successor of block was free


	//
	//  Check if there is an attempt to lock more blocks than there are
	//

	if (num > blockNum)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Check existence of blocks to be unlocked
	//

	for (i=0; i<(int)num; i++)
		{
		if ((indices[i] = FindLockedBlock(block + i)) == FB_BLOCK_NOT_FOUND)
			GNRAISE(GNR_BLOCK_NOT_LOCKED);
		}

	//
	//  Now that we are sure that all blocks exist unlock blocks back
	//  to front to prevent iterated recomputation of succ values
	//

	for (i = ((int)num) - 1; i >= 0; i--)
		{
		//
		//  Unlock a single block
		//

		db = indices[i];
		info[db].lockCount--;
		if (!info[db].lockCount)
			{
			info[db].lockMode = LM_NONE;
			freeBlocks++;

			//
			//  Update successor info if block is free again
			//

			if (db != blockNum - 1)
				{
				if (info[db + 1].lockMode == LM_NONE)
					{
					succFree = TRUE;
					info[db].succ = info[db + 1].succ + 1;
					}
				else
					{
					prevSucc = info[db].succ;	// Save old successor number since we may have broken a sequence
					info[db].succ = 1;
					succFree = FALSE;
					}
				}
			else
				{
				succFree = FALSE;		// chain ends here
				prevSucc = 1;
				}

			//
			//  Update predecessors
			//

			if (db > 0)
				{
				j = ((int)db) - 1;

				if (info[j].lockMode == LM_NONE)
					{
					do
						{
						info[j].succ += info[db].succ;
						j--;
						}
					while (j >= 0 && info[j].lockMode == LM_NONE);
					}
				else
					{
					do
						{
						if (succFree)
							info[j].succ--;
						else
							info[j].succ -= prevSucc;
						j--;
						}
					while (j >= 0 && info[j].lockMode != LM_NONE);
					}
				}

			}
		}

#if DBB_ENABLE_CHECK
	Check();
#endif

	GNRAISE_OK;
	}

//
//  Test if blocks are locked (lockMode of LM_NONE indicates don't care)
//

Error ScatterDriveBlockBuffer::BlocksLocked(DWORD block, DWORD num, DriveBlockLockMode lockMode, BOOL & locked)
	{
	DWORD i;
	DWORD db;

	locked = FALSE;

	for (i=0; i<num; i++)
		{
		if ((db = FindLockedBlock(block + i)) == FB_BLOCK_NOT_FOUND)
			GNRAISE_OK;

		if (lockMode != LM_NONE && info[db].lockMode != lockMode)
			GNRAISE_OK;
		}

	locked = TRUE;
	GNRAISE_OK;
	}

//
//  Flush data
//  GNR_OK
//

Error ScatterDriveBlockBuffer::Flush(void)
	{
	InitInfo();
	GNRAISE_OK;
	}

#if DBB_ENABLE_CHECK

//
//  Check consistency of buffer
//

void ScatterDriveBlockBuffer::Check(void)
	{
	int i;

	for (i=0; i<(int)blockNum - 1; i++)
		{
		if ((info[i].succ > 1 && info[i+1].succ > info[i].succ) || info[i].succ == 0)
			BREAKPOINT;
		}

	if (info[blockNum-1].succ != 1)
		{
		BREAKPOINT;
		}
	}

#endif
