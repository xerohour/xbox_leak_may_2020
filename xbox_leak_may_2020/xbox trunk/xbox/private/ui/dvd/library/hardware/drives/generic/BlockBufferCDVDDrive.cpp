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
//  Block Buffer CDVD Drive Class
//
////////////////////////////////////////////////////////////////////

#include "library/hardware/drives/generic/BlockBufferCDVDDrive.h"

//
//  Constructor
//

BlockBufferCDVDDrive::BlockBufferCDVDDrive(void) : CDVDDrive()
	{
	lastDiskType = PHDT_NONE;
	}

//
//  Destructor
//

BlockBufferCDVDDrive::~BlockBufferCDVDDrive(void)
	{
	delete blockBuffer;
	}

//
//  Init
//

Error BlockBufferCDVDDrive::Init(DriveControlInfo * info, GenericProfile * profile)
	{
	GNREASSERT(CDVDDrive::Init(info, profile));
	blockBuffer = CreateDriveBlockBuffer();
	if (!blockBuffer)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	GNRAISE_OK;
	}

//
//  Cleanup
//

Error BlockBufferCDVDDrive::Cleanup(void)
	{
	delete blockBuffer;
	return CDVDDrive::Cleanup();
	}

//
//  Get drive block size
//
//  Output:
//
//  size - current size of the block for sectore type passed in sectorType
//  minSize, maxSize - minimum and maximum size of block that can be found on currently loaded disk type
//  step - 0 (no meaning in CD/DVD case)
//

Error BlockBufferCDVDDrive::GetDriveBlockSize(DWORD sectorType, DWORD & size, DWORD & minSize, DWORD & maxSize, DWORD & step)
	{
	VDAutoMutex mutex(&monitorMutex);
	PhysicalDiskType type;

	GNREASSERT(GetDiskType(type));

	switch (type)
		{
		case PHDT_CDDA:
			maxSize = minSize = CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE;
			break;
		case PHDT_CDROM:
		case PHDT_CDI:
		case PHDT_CDROM_XA:
			minSize = CD_FRAME_SIZE;
			maxSize = CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE;
			break;
		case PHDT_DVD_ROM:
			maxSize = minSize = DVD_BLOCK_SIZE;
			break;
		case PHDT_NONE:
			GNREASSERT(blockBuffer->GetBlockSize(size));		// No disk, so return whatever we have now
			maxSize = minSize = size;
			break;
		default:
			BREAKPOINT;
		}

	switch (sectorType)
		{
		case DST_NONE:
			size = minSize; //If we don't know sector type, return minimum block size as current size
			break;
		case DST_DVD_ROM:
			size = DVD_BLOCK_SIZE;
			break;
		case DST_CDDA:
			size = CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE;
			break;
		case DST_CDROM_MODE1:
		case DST_CDROM_MODE2:
		case DST_CDROM_MODE2_XA_FORM1:
		case DST_CDROM_MODE2_XA_FORM2:
			size = CD_FRAME_SIZE;
			break;
		case DST_HD:
		default:
			BREAKPOINT;
		}

	//
	//  Make sure the drive block size fits the media type
	//

	if (type != lastDiskType)
		GNREASSERT(blockBuffer->SetBlockSize(maxSize));
	lastDiskType = type;

	step = 0;
	GNRAISE_OK;
	}

//
//  Set drive block size
//

Error BlockBufferCDVDDrive::SetDriveBlockSize(DWORD size)
	{
	VDAutoMutex mutex(&monitorMutex);

	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get number of drive blocks
//

Error BlockBufferCDVDDrive::GetNumberOfDriveBlocks(DWORD & num)
	{
	VDAutoMutex mutex(&monitorMutex);

	if (blockBuffer)
		return blockBuffer->GetBlockNum(num);
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}

//
//  Set number of drive blocks
//

Error BlockBufferCDVDDrive::SetNumberOfDriveBlocks(DWORD num)
	{
	VDAutoMutex mutex(&monitorMutex);

	if (blockBuffer)
		return blockBuffer->SetBlockNum(num);
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}

//
//  Terminate request
//

Error BlockBufferCDVDDrive::TerminateRequest(RequestHandle * rh)
	{
	GNRAISE_OK;
	}

//
//  Flush
//

Error BlockBufferCDVDDrive::Flush(void)
	{
	GNRAISE_OK;
	}

//
//  Lock blocks
//

Error BlockBufferCDVDDrive::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitorMutex);
	Error err = GNR_UNIMPLEMENTED;
	Error helpErr;
	int i = 0;
	int j;

//	DP("Locking Block %d", block);
	rh->Activate();

	if (GD_COMMAND(flags) == DAT_LOCK_AND_READ)
		{
		if (!IS_ERROR(err = blockBuffer->LockBlocks(block, num, LM_READ, blocks)))
			{
			while (i<(int)num)
				{
				//
				//  Win32DriveBlockBuffer returns number of contiguous blocks in private0
				//

				switch (GD_SECTOR_TYPE(flags))
					{
					case DST_DVD_ROM:
						helpErr = ReadDVDBlocks(block + i, blocks[i].private0, blocks[i].data, flags);
						break;
					case DST_CDDA:
					case DST_CDROM_MODE1:
					case DST_CDROM_MODE2:
					case DST_CDROM_MODE2_XA_FORM1:
					case DST_CDROM_MODE2_XA_FORM2:
						helpErr = ReadCDBlocks(block + i, blocks[i].private0, blocks[i].data, flags);
						break;
					default:
						helpErr = GNR_OBJECT_INVALID;
						break;
					}

				//
				//  Mark invalid blocks in case of error
				//

				for (j=0; j<(int)blocks[i].private0; j++)
					{
					if (IS_ERROR(helpErr))
						blocks[i + j].data = NULL;
					blocks[i + j].err = helpErr;
					}

				if (IS_ERROR(helpErr))
					err = helpErr;

				//
				//  Continue until all blocks are read
				//

				if (blocks[i].private0)
					i += blocks[i].private0;
				else
					i++;
				}
			}
		}

	rh->Passivate();

	GNRAISE(err);
	}

//
//  Unlock blocks
//

Error BlockBufferCDVDDrive::UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh)
	{
	VDAutoMutex mutex(&monitorMutex);
	Error err = GNR_UNIMPLEMENTED;

	rh->Activate();

	if (GD_COMMAND(flags) == DAT_UNLOCK_CLEAN)
		{
		err = blockBuffer->UnlockBlocks(block, num);
		}

	rh->Passivate();
//	DP("Unlock result was %lx", err);
	GNRAISE(err);
	}

