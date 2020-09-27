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

#ifndef DRIVEBLOCKBUFFER_H
#define DRIVEBLOCKBUFFER_H

#define DBB_ENABLE_CHECK 1		// Enable consistency check

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"
#include "library/hardware/drives/generic/driveerrors.h"
#include "library/hardware/drives/generic/GenericDrive.h"

////////////////////////////////////////////////////////////////////
//
//  Drive Block Buffer Class
//
////////////////////////////////////////////////////////////////////

class DriveBlockBuffer
	{
	public:
		DriveBlockBuffer(void) {};
		virtual ~DriveBlockBuffer(void) {};

		virtual Error SetBlockNum(DWORD blockNum) = 0;
		virtual Error GetBlockNum(DWORD & num) = 0;

		virtual Error SetBlockSize(DWORD blockSize) = 0;
		virtual Error GetBlockSize(DWORD & size) = 0;

		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * blocks) = 0;
		virtual Error UnlockBlocks(DWORD block, DWORD num) = 0;

		virtual Error BlocksLocked(DWORD block, DWORD num, DriveBlockLockMode lockMode, BOOL & locked) = 0;	// LM_NONE indicates "don't care"

		virtual Error GetFreeBlockNum(DWORD & num) = 0;

		virtual Error Flush(void) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  Scatter Drive Block Info Class
//
////////////////////////////////////////////////////////////////////

class ScatterDriveBlockInfo
	{
	public:
		WORD						lockCount;	// Number of times the block was locked
		DriveBlockLockMode	lockMode;	// Lock mode, read or write
		DWORD						succ;			// Number of successors with equal state (locked or free, including itself)
		DWORD						block;		// Index of the block contained in buffer
	};

////////////////////////////////////////////////////////////////////
//
//  Scatter Drive Block Buffer Class
//  NOTE: To create a working implementation of this class, the
//  following things must be implemented:
//  * Constructors which must call those of ScatterDriveBlockBuffer
//  * Destructor which must perform the same action as FreeData()
//  * AllocateData() and FreeData() methods
//
////////////////////////////////////////////////////////////////////

class ScatterDriveBlockBuffer : public DriveBlockBuffer
	{
	protected:
		BYTE * data;												// The buffer itself
		ScatterDriveBlockInfo * info;							// Management structure
		DWORD * indices;											// Indices for unlocking block

		DWORD blockNum;											// Number of blocks in buffer
		DWORD blockSize;											// Size of block in buffer
		DWORD freeBlocks;											// Number of free blocks
		DWORD nextSearchPosition;								// Search position for FindLockedBlock()

		//
		//  Internal management functions
		//

		virtual void AllocateData(DWORD size) = 0;			// This must be overloaded to create the implementation specific buffer
																			// If error occurs, data should be set to NULL
		virtual void FreeData(void) = 0;							// Delete buffer "data"
		Error Resize(DWORD blockNum, DWORD blockSize);		// Internal resize function for data buffer and control info
		void	InitInfo(void);										// Initialize control information

		DWORD FindLockedBlock(DWORD index);
		void	LockFreeBlocks(DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * blocks);
		void	LockBlockSeq(DWORD start, DWORD block, DWORD num, DriveBlockLockMode lockMode, DriveBlock * blocks);
		void  RelockBlocks(DWORD start, DWORD num, DriveBlock * blocks);

#if DBB_ENABLE_CHECK
		void Check(void);
#endif

	public:
		ScatterDriveBlockBuffer(void);
		virtual ~ScatterDriveBlockBuffer(void);

		virtual Error SetBlockNum(DWORD blockNum);			// Set (minimum) track buffer size
		virtual Error GetBlockNum(DWORD & num);				// Get track buffer size (internal size can be higher)

		virtual Error SetBlockSize(DWORD blockSize);			// Set size of a drive block (in bytes)
		virtual Error GetBlockSize(DWORD & size);				// Get size of a drive block (in bytes)

		virtual Error LockBlocks(DWORD block, DWORD num,					// Lock specified amount of blocks
										 DriveBlockLockMode lockMode,				// This one returns the number of contiguously locked
										 DriveBlock * blocks);						// blocks in private0 of the first block of each segment
																							// Blocks that were already locked have private0 set to 0
		virtual Error UnlockBlocks(DWORD block, DWORD num);				// Unlock specified amount of blocks

		virtual Error BlocksLocked(DWORD block, DWORD num, DriveBlockLockMode lockMode, BOOL & locked);	// LM_NONE indicates "don't care"

		virtual Error GetFreeBlockNum(DWORD & num) { num = freeBlocks; GNRAISE_OK; }

		virtual Error Flush(void);
	};

#endif
