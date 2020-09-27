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
//  DVD Stream Server Classes
//
////////////////////////////////////////////////////////////////////


#include "DVDStreamServer.h"
#include "Library/Common/vddebug.h"
#include "Library/Lowlevel/timer.h"
#include "DVDPlayer.h"



////////////////////////////////////////////////////////////////////
//
//  DVD VOBU Class
//
////////////////////////////////////////////////////////////////////

#define BREAKPOINT_DEFERRED	0	// Set this to 1 to have a seamless FFWD breakpoint still/still off


//
//  Constructor
//

DVDVOBU::DVDVOBU (void)
	{
	file = NULL;
	navPackBlock = 0xffffffff;
	}

//
//  Destructor
//

DVDVOBU::~DVDVOBU (void)
	{
	}

//
//  Initialize
//

void DVDVOBU::Init(DVDDataFile * file, DVDCell * cell, DWORD firstBlock, BOOL singleFrame)
	{
	state = VOS_READY;
	this->file = file;
	this->cell = cell;
	this->firstBlock = firstBlock;
	this->singleFrame = singleFrame;
	this->still = FALSE;
	navPackBlock = 0xffffffff;
	}

//
//  Cleanup
//

void DVDVOBU::Cleanup(void)
	{
	if (file && navPackBlock != 0xffffffff)
		{
		file->UnlockBlocks(navPackBlock, 1, &navPackDriveBlock, DAT_UNLOCK_CLEAN | DAF_STREAMING, &rh);
		file = NULL;
		navPackBlock = 0xffffffff;
		}
	state = VOS_FREE;
	}

//
//  Test on non-seamless angle change info
//

BOOL DVDVOBU::HasNonSeamlessAngleInfo(void)
	{
	int i;

	for (i=0; i<9; i++)
		{
		if (GetNVDWord(45 + 60 + 4 * i) != 0)
			return TRUE;
		}
	return FALSE;
	}

//
//  Test on seamless angle change info
//

BOOL DVDVOBU::HasSeamlessAngleInfo(void)
	{
	int i;

	for (i=0; i<9; i++)
		{
		if (GetNVDWord(1024 + 7 + 32 + 148 + 6 * i) != 0)
			return TRUE;
		}
	return FALSE;
	}

//
//  Find next navigation pack
//

Error DVDVOBU::FindNextNavigationPack(DWORD final, WORD angle, BOOL & suspendTransfer)
	{
	Error error = GNR_FILE_READ_ERROR;
	DWORD step, next;

	//
	//  Repeat until end of search area
	//

	DP("Starting search of next nav pack from %d to %d", firstBlock, final);

	step = 64;
	while (error == GNR_FILE_READ_ERROR && firstBlock < final)
		{
		if (suspendTransfer) GNRAISE(GNR_OPERATION_ABORTED);

		DP("In Recover for %08lx until %08lx\n", firstBlock, final);

		//
		//  Find a readable block
		//

		firstBlock += step;
		while (firstBlock <= final && (error = ReadNavPackData(firstBlock)) == GNR_FILE_READ_ERROR)
			{
			DP("Bad block at %08lx\n", firstBlock);
			step *= 2;
			firstBlock += step;
			if (suspendTransfer) GNRAISE(GNR_OPERATION_ABORTED);
			}
		if (firstBlock > final) firstBlock = final;

		//
		//  Look for PCI and DSI data
		//
		while (firstBlock < final && error == GNR_OK && (GetNVDWord(38) != 0x000001bf || GetNVDWord(1024) != 0x000001bf))
			{
			firstBlock++;
			error = ReadNavPackData(firstBlock);
			}

		DP("Found Nav Pack at %08lx\n", firstBlock);

		//
		//  Could not recover inside area
		//

		if (firstBlock == final)
			GNRAISE(GNR_RANGE_VIOLATION);

		//
		//  Ok, we found a nav pack
		//

		if (error == GNR_OK)
			{
			//
			// Skip that VOBU and continue with next
			//

			next = NextVOBU();
			if (next != 0x3fffffff)
				{
				//
				//  Read Nav Pack of next VOBU
				//

				error = ReadNavPackData(next);
				if (error == GNR_OK && GetNVDWord(1024) == 0x000001bf)
					{
					DP("Checking second OK %08lx\n", next);
					error = ReadNavPackData(firstBlock);
					}
				else
					{
					DP("Checking second Failed %08lx\n", next);
					error = GNR_FILE_READ_ERROR;
					}
				}

			//
			// Now find next VOBU for current angle
			//

			if (!IS_ERROR(error))
				{
				if (singleFrame)
					numBlocks = GetDSIDWord(12) + 1;
				else
					numBlocks = GetDSIDWord(8) + 1;
				}
			}
		}

	DP("Now leaving search with error %08lx", error);

	GNRAISE(error);
	}

//
//  Read navigation pack
//

Error DVDVOBU::ReadNavigationPack(void)
	{
	GNREASSERT(ReadNavPackData(firstBlock));

	if (GetNVDWord(38) != 0x000001bf || GetNVDWord(1024) != 0x000001bf)
		{
		DP("Tertiary Error reading NavPack");
		file->UnlockBlocks(firstBlock, 1, &navPackDriveBlock, DAT_UNLOCK_CLEAN | DAF_STREAMING, &rh);
		navPackBlock = 0xffffffff;

		GNRAISE(GNR_FILE_READ_ERROR);
		}
	else
		{
		if (singleFrame)
			numBlocks = GetDSIDWord(12) + 1;
		else
			numBlocks = GetDSIDWord(8) + 1;

		GNRAISE_OK;
		}
	}

//
//  Read navpack data
//

Error DVDVOBU::ReadNavPackData(DWORD block)
	{
	Error err;

	if (navPackBlock != 0xffffffff)
		{
		file->UnlockBlocks(navPackBlock, 1, &navPackDriveBlock, DAT_UNLOCK_CLEAN | DAF_STREAMING, &rh);
		}

	if (!IS_ERROR(err = file->LockBlocks(block, 1, &navPackDriveBlock, DAT_LOCK_AND_READ | DAF_STREAMING, &rh)))
		{
		if (navPackDriveBlock.data[0] != 0x00 || navPackDriveBlock.data[1] != 0x00 ||
			 navPackDriveBlock.data[2] != 0x01 || navPackDriveBlock.data[3] != 0xba)
			{
			DP("Secondary Error reading NavPack");
			file->UnlockBlocks(block, 1, &navPackDriveBlock, DAT_UNLOCK_CLEAN | DAF_STREAMING, &rh);
			navPackBlock = 0xffffffff;

			err = GNR_FILE_READ_ERROR;
			}
		else
			navPackBlock = block;
		}
	else
		{
		DP("Error reading NavPack");
		file->UnlockBlocks(block, 1, &navPackDriveBlock, DAT_UNLOCK_CLEAN | DAF_STREAMING, &rh);
		navPackBlock = 0xffffffff;
		}

	GNRAISE(err);
	}

//
//  Get Navigation Bytes
//

void DVDVOBU::GetNVBytes(FPTR buff, int at, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		((BYTE *)buff)[i] = navPackDriveBlock.data[at+i];
		}
	}

//
//  Find the VOBU which is closest to the requested time (in the future)
//

DWORD DVDVOBU::NextTemporalVOBU(WORD halfSeconds)
	{
	DWORD succ;

	if (halfSeconds > 180)
		succ = GetDSIDWord(234 + 4) & 0x3fffffff;
	else if (halfSeconds > 90)
		succ = GetDSIDWord(234 + 8) & 0x3fffffff;
	else if (halfSeconds > 40)
		succ = GetDSIDWord(234 + 12) & 0x3fffffff;
	else if (halfSeconds > 17)
		succ = GetDSIDWord(234 + 16) & 0x3fffffff;
	else if (halfSeconds > 15)
		succ = GetDSIDWord(234 + 20) & 0x3fffffff;	// Go back to 15 half seconds
	else if (halfSeconds > 1)
		succ = GetDSIDWord(234 + (20 - halfSeconds) * 4) & 0x3fffffff;
	else
		succ = GetDSIDWord(234) & 0x3fffffff; // Direct successor

	if (succ != 0x3fffffff) succ += firstBlock;

	return succ;
	}

//
//  Find the next VOBU which is before ("lower") the requested point in time
//

DWORD DVDVOBU::NextLowerTemporalVOBU(WORD halfSeconds)
	{
	DWORD succ;

	if (halfSeconds >= 240)
		succ = GetDSIDWord(234 + 4) & 0x3fffffff;
	else if (halfSeconds >= 120)
		succ = GetDSIDWord(234 + 8) & 0x3fffffff;
	else if (halfSeconds >= 60)
		succ = GetDSIDWord(234 + 12) & 0x3fffffff;
	else if (halfSeconds >= 20)
		succ = GetDSIDWord(234 + 16) & 0x3fffffff;
	else if (halfSeconds > 15)
		succ = GetDSIDWord(234 + 20) & 0x3fffffff;	// Go back to 15 half seconds
	else if (halfSeconds >= 1)
		succ = GetDSIDWord(234 + 20 + (15 - halfSeconds) * 4) & 0x3fffffff;
	else
		succ = GetDSIDWord(234) & 0x3fffffff; // Direct successor

	if (succ != 0x3fffffff) succ += firstBlock;

	return succ;
	}

//
//  Find the VOBU which is closest to the requested time (in the past)
//

DWORD DVDVOBU::PrevTemporalVOBU(WORD halfSeconds)
	{
	DWORD succ;

	if (halfSeconds > 180)
		succ = GetDSIDWord(234 + 160) & 0x3fffffff;
	else if (halfSeconds > 90)
		succ = GetDSIDWord(234 + 156) & 0x3fffffff;
	else if (halfSeconds > 40)
		succ = GetDSIDWord(234 + 152) & 0x3fffffff;
	else if (halfSeconds > 17)
		succ = GetDSIDWord(234 + 148) & 0x3fffffff;
	else if (halfSeconds > 15)
		succ = GetDSIDWord(234 + 144) & 0x3fffffff;	// Go back to 15 half seconds
	else if (halfSeconds > 1)
		succ = GetDSIDWord(234 + (21 + halfSeconds) * 4) & 0x3fffffff;
	else
		succ = GetDSIDWord(234 + 164) & 0x3fffffff; // Direct successor

	if (succ != 0x3fffffff) succ = firstBlock - succ;

	return succ;
	}

BOOL DVDVOBU::IsLastVOBUinILVU(void)
	{
	DWORD next = GetDSIDWord(8);

	return (next == GetDSIDWord(32 + 2) && XTBF(14, GetDSIWord(32)));
	}

DWORD DVDVOBU::NextVOBU(void)
	{
	DWORD next = GetDSIDWord(8);

	if (next == GetDSIDWord(32 + 2) && XTBF(14, GetDSIWord(32)))
		{
		return firstBlock + GetDSIDWord(32 + 6);
		}
	else
		return firstBlock + next + 1;
	}

DWORD DVDVOBU::PrevVOBU(void)
	{
	DWORD succ = GetDSIDWord(398) & 0x3fffffff;

	if (succ != 0x3fffffff) succ = firstBlock - succ;

	return succ;
	}

BOOL DVDVOBU::HasVideoData(void)
	{
	return GetDSIDWord(12) != 0;
	}

DWORD DVDVOBU::SeamlessAngleChangeVOBU(WORD angle)
	{
	DWORD succ;

	succ = GetDSIDWord(180 + 6 * (angle - 1)) & 0x7fffffff;
	if (succ != 0x7fffffff)
		succ += firstBlock;

	return succ;
	}

DWORD DVDVOBU::NonSeamlessAngleChangeVOBU(WORD angle)
	{
	DWORD succ;

	succ = GetPCIDWord(60 + 4 * (angle - 1));
	if (succ)
		{
		if (succ & 0x80000000)
			succ = firstBlock - (succ & 0x7fffffff);
		else
			succ = firstBlock + (succ & 0x7fffffff);
		}

	return succ;
	}

DWORD	DVDVOBU::GetSPTM(void)
	{
	return GetPCIDWord(12);
	}

DWORD DVDVOBU::GetEPTM(void)
	{
	return GetPCIDWord(16);
	}

DVDTime DVDVOBU::GetTimeOffset(void)
	{
	return DVDTime(GetPCIDWord(24));
	}

DVDTime DVDVOBU::GetDuration(void)
	{
	return DVDTime(GetPCIDWord(16) - GetPCIDWord(12), 90, (GetPCIDWord(24) & 0xc) == 0x4 ? 25 : 30);
	}

////////////////////////////////////////////////////////////////////
//
//  DVD Stream Buffer Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDStreamBuffer::DVDStreamBuffer(DWORD bufferSize)
	{
	this->bufferSize = bufferSize * DVD_BLOCK_SIZE;

	Flush();
	}

//
//  Destructor
//

DVDStreamBuffer::~DVDStreamBuffer(void)
	{
	}

//
//  Flush
//

void DVDStreamBuffer::Flush(void)
	{
	bufferStart = 0;
	bufferEnd = 0;
	availData = 0;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Stream Server Class
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDStreamServer::DVDStreamServer (DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: WinPort (server)
	, EventSender(pEventDispatcher)
	{
	int i;

	for(i=0; i<NUM_CELL_BUFFERS; i++)
		{
		cells[i].succ = cells + (i + 1) % NUM_CELL_BUFFERS;
		cells[i].pred = cells + (i + NUM_CELL_BUFFERS - 1) % NUM_CELL_BUFFERS;
		}

	//NOTE: with smaller buffer sizes the chance for buffer underrun is increased when buffer read/consuming
	//thread can't get control often enough (e.g. Win16 mutex is held during decoding or all the CPU power is used up).

	streamBuffer = NULL;
	streamBufferSize = NUM_DVD_BLOCKS_IN_BUFFER;

	/*
	//FlushBuffers();  // Initializes cell buffers

	transferSemaphore = 0;
	positionCache.valid = FALSE;
	*/

	dvdfs = NULL;

	this->sequencer = seq;
	this->server = server;
	this->units = units;

	player = NULL;
	}

//
//  Destructor
//

DVDStreamServer::~DVDStreamServer(void)
	{
	if (player)
		{
		player->DeleteRefiller();
		delete player;
		player = NULL;
		}
	}

Error DVDStreamServer::Init(DVDFileSystem * dvdfs)
	{
	//
	//  Create player if none exist
	//

	if (!player)
		{
		player = new DVDPlayer(server, units, GetEventDispatcher());

		if (!player)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		}

	GNRAISE_OK;
	}

//
// Return stream player
//

DVDStreamPlayer * DVDStreamServer::GetStreamPlayer(void)
	{
	return player;
	}


