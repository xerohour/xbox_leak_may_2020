////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  VCD Stream Server Class
//
////////////////////////////////////////////////////////////////////

#include "VCDStreamServer.h"
#include "Library\Common\vddebug.h"
#include "Library\Common\Prelude.h"


////////////////////////////////////////////////////////////////////
//
// Class: VCDStreamBuffer
//
// Description: Stream buffer management for Video-CD data with
//  lock/unlock mechanism.
//
////////////////////////////////////////////////////////////////////

//
// Constructor
//

VCDStreamBuffer::VCDStreamBuffer(DWORD numBuffers)
	{
	blocks = new DriveBlock[numBuffers];
	blockIndices = new DWORD[numBuffers];

	buffers = new MPEGDataSizePair[numBuffers];
	numBlocks = numBuffers;
	file = NULL;
	for (int i=0; i<CD_FRAME_SIZE; i++)
		dummyBlock[i] = 0;

	this->bufferSize = numBuffers * CD_FRAME_SIZE;
	Flush();
	}

//
// Destructor
//

VCDStreamBuffer::~VCDStreamBuffer(void)
	{
	Flush();

	delete[] blocks;
	delete[] blockIndices;
	delete[] buffers;
	}

//
// Function:
//		Flush
// Description:
//		Resets the variables and unlocks the buffers.
//

void VCDStreamBuffer::Flush(void)
	{
	if (file)
		{
		if (validBlocks)
			ClearBlocks();
		file = NULL;
		}

	bufferStart = bufferEnd = 0;
	dataBlocksInBuffer = 0;
	dataBlocksBackScanRead = 0;
	validBlocks = 0;
	firstBlock = 0;
	lastBlock = 0;
	}

void VCDStreamBuffer::ClearBlocks(void)
	{
	if (file)
		UnlockBuffers(file, 0, validBlocks);

	validBlocks = 0;
	firstBlock = lastBlock = 0;
	}

//
// Function:
//		ChangeBufferSize
// Description:
//		Not implemented
//

Error VCDStreamBuffer::ChangeBufferSize(DWORD size)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
// Function:
//		UnlockBuffers
// Parameters:
//		file: pointer to a Video-CD data file
//		at: offset to the existing DriveStreamBlockBuffer
//		num: number of blocks to unlock
// Description:
//		Unlocks the existing Drive Stream Block Buffers
//

void VCDStreamBuffer::UnlockBuffers(VCDDataFile * file, DWORD at, DWORD num)
	{
	RequestHandle rh;

	if (file)
		{
		//DP("unlock blocks %d, num %d, at %d", blockIndices[at], num, at);
		file->UnlockBlocks(blockIndices[at], num, blocks + at, DAT_UNLOCK_CLEAN, &rh);
		}

	}

//
// Function:
//		GetCurrentBlock
// Return Value:
//		byte pointer to the current block
// Description:
//		Returns a pointer to the current data block if data is available otherwise a NULL pointer.
//

BYTE * VCDStreamBuffer::GetCurrentBlock(void)
	{
	if(IsDataAvailable())
		return (blocks[firstBlock].data);
	else
		return NULL;
	}

//
//  Advance first block
//

Error VCDStreamBuffer::AdvanceFirstBlock(int amount)
	{
	// the maximum number of blocks that can be unlocked is the current number of blocks that are locked.
	// If 'amount' is greater than the current data blocks (this is mostly the case when in scanning mode)
	// then just Flush all buffers.
	if (amount <= (int)dataBlocksInBuffer)
		{
		dataBlocksInBuffer -= amount;
		}
	else
		{
		//Flush() --> is performed by the caller!!!!
		dataBlocksInBuffer = 0;
		}

	firstBlock = (firstBlock + amount) % numBlocks;

	GNRAISE_OK;
	}

//
//  Backward first block
//

Error VCDStreamBuffer::BackwardFirstBlock(DWORD amount)
	{
	if(firstBlock >= amount)
		{
		firstBlock -= amount;
		dataBlocksInBuffer = lastBlock - firstBlock + 1;
		dataBlocksBackScanRead += amount;
		}
	else
		{
		firstBlock = 0;
		dataBlocksInBuffer = 0;
		dataBlocksBackScanRead = 0;
		}

	GNRAISE_OK;
	}

//
//  Prepare backward scan
//

void VCDStreamBuffer::PrepareBackwardScan(void)
	{
	lastBlock--;
	firstBlock = lastBlock;
	dataBlocksBackScanRead = 0;
	}

//
//  Read into buffer
//

Error VCDStreamBuffer::ReadIntoBuffer(VCDDataFile * file, DWORD block, DWORD at, DWORD num)
	{
	RequestHandle rh;
	Error error;
	int i;

	if(validBlocks)
		{
		ClearBlocks();
		}
	//DP("Lock blocks %d, num %d, at %d\n", block, num, at);
	error = file->LockBlocks(block, num, blocks + at, DAT_LOCK_AND_READ, &rh);

	if (!IS_ERROR(error))
		{
		for (i=0; i<(int)num; i++)
			{
			if (IS_ERROR(blocks[at + i].err))
				{
				DP("error in block %d", i);
				blocks[at+i].data = dummyBlock;
				}
			else
				buffers[at+i].data = blocks[at+i].data;
			buffers[at+i].size = CD_FRAME_SIZE;
			buffers[at+i].timeStamp = -1;
			blockIndices[at + i] = block + i;
			}
		}
	else
		{
		UnlockBuffers(file, at, num);
		}

	GNRAISE(error);
	}

//
// Function:
//		ReadIntoBuffer
// Parameters:
//		file: pointer to Video-CD data file
//		blockIntegrity: pointer to a Video-CD block integrity class to check the data integrity
//		block: block number that should be read
//		num: number of bytes to read in blocks
// Return Value:
//		an Error value
// Description:
//

Error VCDStreamBuffer::ReadIntoBuffer(VCDDataFile * file, VCDBlockIntegrity * blockIntegrity, DWORD block, DWORD numBlocksToRead, DWORD &numBlocksDone, BOOL isScanning)
	{
	DWORD i;			// loop counter
	DWORD num;
	Error error = GNR_OK;

	this->file = file;

	if (isScanning)
		{
		error = ReadIntoBuffer(file, block, 0, numBlocksToRead);
		if(IS_ERROR(error))
			{
			lastBlock = num = 0;
			}
		else
			{
			lastBlock = num = numBlocksToRead;
			dataBlocksInBuffer += num;
			validBlocks = dataBlocksInBuffer;
			}
		}
	else
		{
		//
		//  Determine number of blocks to read during this repetition
		//

		num = numBlocksToRead >= numBlocks ? numBlocks : numBlocksToRead;

		//
		//	Perform the read
		//

		error = ReadIntoBuffer(file, block, bufferEnd / CD_FRAME_SIZE, num);

		if(!IS_ERROR(error))
			{
			//
			//	Check data integrity
			//

			for (i=0; i<num; i++)
				{
				if (!blockIntegrity->CheckBlockIntegrity(blocks[bufferEnd / CD_FRAME_SIZE + i].data))
					memset(blocks[bufferEnd / CD_FRAME_SIZE + i].data, 0, CD_FRAME_SIZE);
				else
					blockIntegrity->AssureBlockPTSIntegrity(blocks[bufferEnd / CD_FRAME_SIZE + i].data);
				}
			}

		lastBlock += num;
		if (lastBlock >= numBlocks)
			lastBlock = 0;
		dataBlocksInBuffer += num;
		validBlocks = dataBlocksInBuffer;
		}

	numBlocksDone = num;

	GNRAISE(error);
	}

//
// Function:
//		WriteFromBuffer
// Parameters:
//		player: pointer to Video-CD player
//		offset: byte offset within block
//		num: number of bytes to write
// Return Value:
//		 number of bytes that were written
// Description:
//		Sends data to the player.
//

DWORD VCDStreamBuffer::WriteFromBuffer(VCDPlayer * player, DWORD offset, DWORD num)
	{
	DWORD done;

	buffers[firstBlock].data = blocks[firstBlock].data + offset;
	buffers[firstBlock].size = num;

	done = player->SendDataMultiple(buffers+firstBlock, num);

	return done;
	}

////////////////////////////////////////////////////////////////////
//
//  Class: VCDStreamSequencer
//
//  Description:
//
////////////////////////////////////////////////////////////////////

//
// Constructor
//

VCDStreamSequencer::VCDStreamSequencer (WinPortServer *server, UnitSet units, EventDispatcher* pEventDispatcher) : WinPort (server)
	{
	int i;

	playRequest = TRUE;

	for (i = 0;  i < 256;  i++)
		searchSteps[i] = 4;
	searchSteps[0x00] = 2;
	searchSteps[0x01] = 1;
	searchSteps[0xb3] = 10000;
	searchSteps[0xb8] = 10000;

	streamBuffer = NULL;
	packHeaderOffset = 0;

	//
	//  Create player
	//

	player = new VCDPlayer(server, units, pEventDispatcher);
	}

//
//  Destructor
//

VCDStreamSequencer::~VCDStreamSequencer (void)
	{
	delete streamBuffer;
	delete player;
	}

//
//  Initialize
//

Error VCDStreamSequencer::Init(void)
	{
	//
	//  Initialize player
	//

	if (!player)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT(player->Init());
	player->SetRefiller(this);

	streamBuffer = new VCDStreamBuffer(NUM_VCD_BUFFERS);

	FlushBuffers();

	GNRAISE_OK;
	}

//
//  Test if there is a sequencer header in the sector
//

BOOL VCDStreamSequencer::IsSequenceHeaderInSector(BYTE * sector)
	{
	BYTE	*	p;
	int	*	steps;
	BYTE	*	end;

	p = sector;

	if (p[18] == 0x62)
		{
		steps = searchSteps;
		end = p + 2347;

		p += 24 - 1;
		do {
			p += 4;

			while (p < end) p += steps[p[0]];

			p -= 10000;
			} while (p > sector  &&  ((p[0] != 0xb3 && p[0] != 0xb8) ||  p[-1] != 0x01  ||  p[-2] != 0x00  ||  p[-3] != 0x00));

		return p > sector;
		}
	else
		return FALSE;
	}

//
//	Function:
//		FindTimeSearchSector
//	Parameter:
//		file: pointer to a VCDDataFile
//		time: desired time in milliseconds
//	Return Value:
//		sector that belongs to the desired time
// Description:
//		Returns the nearest sector for a given playing time
//		The 'time' parameter must be given in milliseconds!
//

DWORD VCDStreamSequencer::FindTimeSearchSector(VCDDataFile * file, DWORD time)
	{
	BOOL foundValidSector = FALSE;
	BOOL foundSector = FALSE;
	DWORD currentTime = 0;
	DWORD numBlocksRead;
	int blocksAvailable;
	int num;
	DWORD maxSector, sector;
	DWORD desiredTimeInSeconds;
	DWORD numSectors;
	WORD numSearchValidSector = 0;
	WORD numSearchExactSector = 0;
	BOOL readError = FALSE;


	//
	// determine the number of sectors this track contains
	//

	if (IS_ERROR(file->GetNumberOfBlocks(numSectors)))
		numSectors = 0;


	desiredTimeInSeconds = time/1000;

	//
	// calculate a start sector to search for
	// max. bitrate is 2.6 Mbps --> 140 sectors/s
	// bitrate 2.3 Mbps ---> 124 sectors/s
	// bitrate 2 Mbps ---> 108 sectors/s
	//

	// the sector we're searching for cannot be greater than maxSector
	// because of the max bitrate of 2.6 Mbps!
	maxSector = desiredTimeInSeconds * 140;
	if (maxSector > numSectors) maxSector = numSectors;
	sector = desiredTimeInSeconds * 108;


	while(!foundSector && !readError)
		{
		//
		// flush buffers
		//

		streamBuffer->Flush();

		//
		// read new data
		//
		num = min(maxSector - sector, NUM_VCD_BUFFERS);
		streamBuffer->ReadIntoBuffer(file, this, sector, num, numBlocksRead, FALSE);

		foundValidSector = FALSE;
		currentTime = 0;

		//
		//  Now go and find a sector with a valid PTS
		//
		while (!currentTime && !readError)
			{
			//
			//  If buffer is empty, then read new data
			//

			blocksAvailable = streamBuffer->AvailBlocks();

			if (blocksAvailable <= 0)
				{
				streamBuffer->Flush();
				num = min(maxSector - sector, NUM_VCD_BUFFERS);
				streamBuffer->ReadIntoBuffer(file, this, sector, num, numBlocksRead, FALSE);
				}

			if (streamBuffer->IsDataAvailable())
				{
				//
				// extract the PTS in the current sector
				//

				currentTime = ExtractPTS(streamBuffer->GetCurrentBlock());
				}

			if (!currentTime)
				{
				AdvanceFirstBlock();
				sector += 1;
				numSearchValidSector++;
				if (numSearchValidSector >= 140)
					{
					DP("*** READ ERROR: no valid PTS found ***");
					readError = TRUE;
					}
				}
			else numSearchValidSector = 0;
			} // end while (!currentTime && !readError)

		//
		// calculate new sector start address when the sector is not exact enough
		//

		if (time > (currentTime + 750))
			{
			sector = (sector + maxSector)/2;
			foundSector = FALSE;
			}
		else if (time < (currentTime - 250))
			{
			DWORD s = maxSector - sector;
			maxSector = sector;
			if (sector > s)
				sector = sector - s;
			else
				sector = 0;
			foundSector = FALSE;
			}
		else
			foundSector = TRUE;

		numSearchExactSector++;
		if (numSearchExactSector >= 100)		// give up the search after 100 tries. 100 is just an estimated number!
			{
			DP("*** READ ERROR: no precise PTS found ***");
			readError = TRUE;
			sector = 0;
			}
		} // end while(!foundSector && !readError)

	return sector;
	}

//
//  Find the next sector for scanning
//

Error VCDStreamSequencer::FindNextScanSector(void)
	{
	BOOL	found;
	int	num;
	int targetSector;
	DWORD numBlocksRead;
	DWORD blocksAvailable;
	DWORD sectorsToJump;
	int	target;
	DWORD lastSector;

	BOOL isInitial = TRUE;
	found = FALSE;

	if (scanSpeed > 0)
		{
		//
		//  Forward scan: Determine sector to start search at
		//

		targetSector = scanSector + scanSpeed * 75 / 2;
		if (averageScanDistance)
			{
			targetSector = (targetSector + scanSector + averageScanDistance - NUM_VCD_BUFFERS / 2) / 2;
			}

		if ((DWORD)targetSector < currentSector) targetSector = currentSector;

		//
		//  Check if searched sector is already in buffer
		//

		num = streamBuffer->AvailBlocks() - (targetSector - currentSector);

		if (num <= 0)
			{
			sectorsToJump = -num;			// sectorsToJump contains the number of sectors to jump
			streamBuffer->Flush();

			if ((DWORD)numSectors > sectorsToJump)			// jump within current play item !
				{
				numSectors -= sectorsToJump;
				currentSector += sectorsToJump;
				}
			else	// no more sectors left in current play item !
				{
				currentSector += numSectors;
				numSectors = 0;
				}
			}
		else
			{
			AdvanceFirstBlock(targetSector - currentSector);
			}

		//
		//  Now go and find a sequence header
		//

		while (!found)
			{
			if (!numSectors) GNRAISE_OK;

			AdvanceFirstBlock();

			//
			//  If buffer is empty, then read new data
			//

			blocksAvailable = streamBuffer->AvailBlocks();
			if (blocksAvailable <= 0)
				{
				streamBuffer->Flush();

				if (numSectors > NUM_VCD_BUFFERS)
					num = NUM_VCD_BUFFERS;
				else
					num = numSectors;

				if (IS_ERROR(streamBuffer->ReadIntoBuffer(file, this, currentSector, num, numBlocksRead, TRUE)))
					{
					currentSector += num;			// skip sectors
					}
				else
					{
					numSectors -= num;
					currentSector += num;
					}
				}

			//
			//  Now check if we have a sequencer header in firstBuffer
			//

			if (streamBuffer->IsDataAvailable())
				{
				found = IsSequenceHeaderInSector(streamBuffer->GetCurrentBlock());
				}
			} // end while (!found)

		//
		// extract the PTS when scan sector was found
		//

		currentTimeInMilliSecs = ExtractPTS(streamBuffer->GetCurrentBlock());

		startSector = currentSector - streamBuffer->AvailBlocks();

		firstByte = 24;
		lastByte = firstByte + 2324;
		scanHeaderCount = 0;

		if (!averageScanDistance)
			averageScanDistance = currentSector - scanSector;
		else
			averageScanDistance = ((averageScanDistance * 3) + (currentSector - scanSector) + 2) >> 2;

		scanSector = currentSector;

		}
	else if (scanSpeed < 0)
		{

		//
		//  Backward scan: Determine sector to start search at
		//

		targetSector = scanSector + scanSpeed * 75 / 2;
		if (targetSector < 0)				// reached beginning of stream
			targetSector = 0;
		else if (averageScanDistance)
			targetSector = (targetSector + scanSector - averageScanDistance - NUM_VCD_BUFFERS / 2) / 2;

		//
		//	if the target sector is greater than the current sector than set target sector equal to current sector
		//

		if ((DWORD)targetSector > currentSector) targetSector = currentSector;

		//
		//  Check if target sector is already in buffer
		//

		target = streamBuffer->FreeBlocks() - (currentSector - targetSector);

		if (target < 0)	// the target is not in the buffer
			{
			if ((int)currentSector >= -target)			// current sector is within the current stream
				currentSector += target;
			else
				currentSector = 0;							// that means beginning of stream

		   streamBuffer->Flush();
			}
		else // target is in buffer!
			{
			}		// this case usually doesn't appear. Leave it for now! But has to be added later on! (mst 07/21/2000)

		//
		//  Repeat until we find a sequence header (or reached beginning of item)
		//

		while (!found)
			{
			streamBuffer->BackwardFirstBlock();

			//
			// if buffer is empty then read new data
			//

			if (streamBuffer->IsEmpty() || streamBuffer->AllBlocksRead())
				{
				//
				// initialize parameters
				//
				streamBuffer->Flush();

				if (!currentSector)						// if beginning of stream was found
					{
					streamBuffer->Flush();
					startSector = 0;

					GNRAISE_OK;
					}

				if (currentSector >= NUM_VCD_BUFFERS)			// if target is within the stream
					{
					currentSector -= NUM_VCD_BUFFERS;
					GNREASSERT(streamBuffer->ReadIntoBuffer(file, this, currentSector, NUM_VCD_BUFFERS, numBlocksRead, TRUE));
					}
				else	// if current sector is within the stream but the target is beginning of stream
					{
					lastSector = currentSector;		// lastSector contains the last known sector within the current play item
					currentSector = 0;
					GNREASSERT(streamBuffer->ReadIntoBuffer(file, this, currentSector, lastSector, numBlocksRead, TRUE));
					}

				streamBuffer->PrepareBackwardScan();
				} // end if (firstBuf <= 0)

			if (!(streamBuffer->AllBlocksRead()))
				{
				found = IsSequenceHeaderInSector(streamBuffer->GetCurrentBlock());
				}
			} // end while(!found)

		currentTimeInMilliSecs = ExtractPTS(streamBuffer->GetCurrentBlock());

		currentSector += numBlocksRead;
		num = streamBuffer->AvailBlocks() + 1;
		startSector = currentSector - num;
		firstByte = 24;
		lastByte = firstByte + 2324;
		scanHeaderCount = 0;
		GNREASSERT(file->GetNumberOfBlocks(numSectors));
		numSectors -= currentSector;

		if (!averageScanDistance)
			averageScanDistance = scanSector - currentSector;
		else
			averageScanDistance = ((averageScanDistance * 3) + (scanSector - currentSector) + 2) >> 2;

		scanSector = startSector;
		}

	GNRAISE_OK;
	}

//
//  Test if sector is correct
//

BOOL VCDStreamSequencer::CheckBlockIntegrity(BYTE * sector)
	{
	if (sector[0] == 0x00 && sector[11] == 0x00 &&
		 sector[1] == 0xff && sector[2] == 0xff &&
		 sector[3] == 0xff && sector[4] == 0xff &&
		 sector[5] == 0xff && sector[6] == 0xff &&
		 sector[7] == 0xff && sector[8] == 0xff &&
		 sector[9] == 0xff && sector[10] == 0xff)
		{
		if (sector[16] == sector[20] &&
			 sector[17] == sector[21] &&
          sector[18] == sector[22] &&
          sector[19] == sector[23])
			{
			return TRUE;
			}
		else
			return FALSE;
		}
	else
		return FALSE;
	}

//
//   Test if PTS of sector is correct
//

void VCDStreamSequencer::AssureBlockPTSIntegrity(BYTE * sector)
	{
	int t, i;
	DWORD pts;

	if (sector[39+packHeaderOffset] == 0xe0)
		{
		t = 42 + packHeaderOffset;
		while (sector[t] == 0xff) t++;				// extract stuffing bytes
		if ((sector[t] & 0xc0) == 0x40) t+= 2;
		if ((sector[t] & 0xf0) == 0x30)
			{
			pts = (((DWORD)(sector[t  ] & 0x0e) >> 1) << 29) |
				   (((DWORD)(sector[t+1]       )     ) << 21) |
					(((DWORD)(sector[t+2] & 0xfe) >> 1) << 14) |
					(((DWORD)(sector[t+3]       )     ) <<  6) |
					(((DWORD)(sector[t+4] & 0xfc) >> 2));

			if (previousVideoPTS && pts > previousVideoPTS + 45000)
				{
//				DP("Fixing PTS from %d , %d", previousVideoPTS, pts);
				previousVideoPTS += 45000;
				for(i=42; i<t+9; i++) sector[i] = 0xff;
				sector[t+9] = 0x0f;
				}
			else
				{
				previousVideoPTS = pts;
				}
			}
		else if ((sector[t] & 0xf0) == 0x20)
			{
			pts = (((DWORD)(sector[t  ] & 0x0e) >> 1) << 29) |
				   (((DWORD)(sector[t+1]       )     ) << 21) |
					(((DWORD)(sector[t+2] & 0xfe) >> 1) << 14) |
					(((DWORD)(sector[t+3]       )     ) <<  6) |
					(((DWORD)(sector[t+4] & 0xfc) >> 2));

			if (previousVideoPTS && pts > previousVideoPTS + 45000)
				{
//				DP("Fixing PTS from %d , %d", previousVideoPTS, pts);
				previousVideoPTS += 45000;
				for(i=42; i<t+4; i++) sector[i] = 0xff;
				sector[t+4] = 0x0f;
				}
			else
				{
				previousVideoPTS = pts;
				}
			}
		}

	}

//
// Function:
//		ExtractPTS
// Parameter:
//		sector: byte pointer that shows on a CD sector
// Return Value:
//		the extracted presentation time stamp in milliseconds
// Description:
//		extracts the PTS of the given sector and returns it in milliseconds.
//		NOTE: this function is only called when playing a S-VCD. This
//		function is only working correctly with MPEG-2 sector format!
//		It hasn't been tested with MPEG-1 sectors!
//

DWORD VCDStreamSequencer::ExtractPTS(BYTE * sector)
	{
	int t;
	BYTE pts_dts_flags = 0x0;
	DWORD pts, scr;

	//
	// first look for a valid PTS
	//

	pts_dts_flags = sector[43+packHeaderOffset] & 0xc0;

	if (pts_dts_flags == 0xc0 || pts_dts_flags == 0x80)
		{
		//
		// 24 bytes CD header
		// + 12 bytes Pack header
		// + 9 bytes offset in the packet header
		//
		t = 45 + packHeaderOffset;

		pts = (((DWORD)(sector[t  ] & 0x0e) >> 1) << 29) |
				(((DWORD)(sector[t+1]       )     ) << 21) |
				(((DWORD)(sector[t+2] & 0xfe) >> 1) << 14) |
				(((DWORD)(sector[t+3]       )     ) <<  6) |
				(((DWORD)(sector[t+4] & 0xfc) >> 2));

		//DP("PTS 0x%x", pts);

		return pts/45;
		}
	//
	// else look for a pack header
	//
	else if (sector[24] == 0x00 && sector[25] == 0x00 && sector[26] == 0x01 && sector[27] == 0xba)
		{
		//
		// no PTS found ---> instead extract the system clock reference (SCR)
		//

		t = 28;

		scr = ((((DWORD)(sector[t  ] & 0x38)	>> 1)	| ((DWORD)(sector[t  ] & 0x03))) << 27) |
				(((DWORD) (sector[t+1]		  )		 )												<<	19) |
				((((DWORD)(sector[t+2] & 0xf8)	>> 1) | ((DWORD)(sector[t+2] & 0x03)))	<< 12) |
				(((DWORD) (sector[t+3]		  )		 )												<<  4) |
				(((DWORD) (sector[t+4] & 0xf8)	>> 4));

		return scr/45;
		}
	else
		return 0;
	}

//
// Function:
//		HasValidPTS
//	Parameter:
//		sector: pointer to the sector to be investigated
//	Return Value:
//		TRUE if there is a valid PTS within the sector
//		FALSE if the sector doesn't contain a valid PTS
//

BOOL VCDStreamSequencer::HasValidPTS(BYTE * sector)
	{
	BYTE pts_dts_flags = 0x0;

	pts_dts_flags = sector[43+packHeaderOffset] & 0xc0;

	if (pts_dts_flags == 0xc0 || pts_dts_flags == 0x80)
		return TRUE;
	else
		return FALSE;
	}

//
//  Refill the buffer
//

Error VCDStreamSequencer::RefillBuffer(void)
	{
	int blocksToRead;			// Total number of blocks to read
	DWORD numBlocksRead = 0;		// number of blocks that were read
	Error error = GNR_OK;

	//
	//  Check if we are already done
	//

	if (FinalSector())
		GNRAISE_OK;

	//
	//  Check if first buffer is empty => advance it
	//

	if (firstByte == lastByte)
		firstByte = lastByte = 0;

	//
	//  Determine number of blocks to read
	//

	blocksToRead = min(numSectors, (int)(NUM_VCD_BUFFERS - streamBuffer->AvailBlocks()));

	while (blocksToRead && !suspendTransfer)
		{
		//
		//  Read data
		//

		error = streamBuffer->ReadIntoBuffer(file, this, currentSector, blocksToRead, numBlocksRead, FALSE);

		//
		//  Advance sector pointers/counters
		//

		blocksToRead -= numBlocksRead;
		currentSector += numBlocksRead;
		numSectors -= numBlocksRead;
		}

	ReadFirstBufferInfo(streamBuffer->GetCurrentBlock());

	GNRAISE_OK;
	}

//
//  Advance first block
//

void VCDStreamSequencer::AdvanceFirstBlock(int amount)
	{
	if(streamBuffer->IsDataAvailable())
		{
		streamBuffer->AdvanceFirstBlock(amount);
		ReadFirstBufferInfo(streamBuffer->GetCurrentBlock());
		}
	}

//
//  Read information of current small buffer (i.e. first buffer)
//

void VCDStreamSequencer::ReadFirstBufferInfo(BYTE * sector)
	{
	BYTE * buf;
	int i;
	WORD playItem;
	DVDDiskType dt;

	//
	//  Find information in CD-ROM XA subheader
	//

	if(sector)
		{
		buf = sector;

		GetDiskType(dt);
		if (dt == DDT_VIDEO_CD || (dt == DDT_SUPER_VIDEO_CD && followsSVCDStd))
			{
			if (XTBF(1, buf[18])  &&  buf[19] != (hiresStill ? 0x1f : 0x3f))	// Video block and no still data
				{

				//
				// the pack header for MPEG-2 contains 2 bytes more than the one for MPEG-1.
				// So the stream id for MPEG-2 can be found two byte fields later.
				//

				//
				// if it is a video block extract the stream id but leave the audio stream id (either 0xC0 or 0xC1)
				//
				if (buf[39+packHeaderOffset] != 0xc0 && buf[39+packHeaderOffset] != 0xc1)
					buf[39+packHeaderOffset] &= ~0x07;					// Extract stream id

				firstByte = 24;					// Skip header (CD-specific information)
				lastByte = firstByte + 2324;	// We have 2324 bytes of user data in sector

				//
				//  Find picture header for scanning
				//

				if (scanSpeed)
					{
					for (i = firstByte;  i < lastByte;  i++)
						{
						scanHeaderBuffer = (scanHeaderBuffer << 8) | buf[i];
						if (scanHeaderBuffer == 0x00000100)		// Picture Header/Picture start code
							scanHeaderCount++;
						}

					//
					//  If we found next picture start code, then picture is complete
					//

					if (scanHeaderCount > 1)
						scanFrameComplete = TRUE;
					}
				}
			else if (XTBF(2, buf[18])  &&  !scanSpeed)	// Audio block
				{
				firstByte = 24;					// Skip header

				// We have 2304 bytes of user data (VCDs seem to have only 2304 bytes of valid audio data.
				// The last 20 bytes seem to be stuffing bytes
				// MST 08/31/2000
				// changed it because some S-VCDs doesn't play with 2304 bytes size (MST, 02/23/2001)
				// NOTE: should be investigated once more!!!
				//lastByte = firstByte + 2304;
				lastByte = firstByte + 2324;
				}

			} // end if (dt == DDT_VIDEO_CD)
		else if (dt == DDT_SUPER_VIDEO_CD)
			{
			playItem = CurrentPlayItem();
			if (isSegment || buf[16] == playItem - 1)	// either still pic or file number (byte 16 in CD header) equals current play item
				{
				if (XTBF(1, buf[18])  &&  buf[19] != (hiresStill ? 0x1f : 0x3f))	// Video block and no still data
					{

					//
					// the pack header for MPEG-2 contains 2 bytes more than the one for MPEG-1.
					// So the stream id for MPEG-2 can be found two byte fields later.
					//

					//
					// if it is a video block extract the stream id but leave the audio stream id (either 0xC0 or 0xC1)
					//
					if (buf[39+packHeaderOffset] != 0xc0 && buf[39+packHeaderOffset] != 0xc1)
						buf[39+packHeaderOffset] &= ~0x07;					// Extract stream id

					firstByte = 24;					// Skip header (CD-specific information)
					lastByte = firstByte + 2324;	// We have 2324 bytes of user data in sector

					//
					//  Find picture header for scanning
					//

					if (scanSpeed)
						{
						for (i = firstByte;  i < lastByte;  i++)
							{
							scanHeaderBuffer = (scanHeaderBuffer << 8) | buf[i];
							if (scanHeaderBuffer == 0x00000100)		// Picture Header/Picture start code
								scanHeaderCount++;
							}

						//
						//  If we found next picture start code, then picture is complete
						//

						if (scanHeaderCount > 1)
							scanFrameComplete = TRUE;
						}
					}
				else if (XTBF(2, buf[18])  &&  !scanSpeed)	// Audio block
					{
					firstByte = 24;					// Skip header

					// We have 2324 bytes of user data for a S-VCD
					lastByte = firstByte + 2324;
					} // end if (XTBF(1, buf[18])  &&  buf[19] != (hiresStill ? 0x1f : 0x3f))	// Video block and no still data

				autoPause = XTBF(4, buf[18]);		// Trigger bit from CD-ROM XA subheader

				} // end if (isSegment || buf[16] == playItem - 1)	// either still pic or file number (byte 16 in CD header) equals current play item
			}
		autoPause = XTBF(4, buf[18]);		// Trigger bit from CD-ROM XA subheader
		} // end if(sector)
	}

//
//  Send data to the player
//	 Only one block at a time is sent!
//

Error VCDStreamSequencer::SendSmallBuffer(void)
	{
	if (!SmallBufferEmpty())
		firstByte += streamBuffer->WriteFromBuffer(player, firstByte, lastByte - firstByte);

	GNRAISE_OK;
	}


//
//  Flush the buffer
//

Error VCDStreamSequencer::FlushBuffers(void)
	{
	lock.Enter();

	firstByte = lastByte = 0;
	if(streamBuffer)
		streamBuffer->Flush();
	numSectors = 0;
	playRequest = TRUE;
	autoPause = FALSE;
	startSector = 0;
	currentSector = 0;
	transfering = FALSE;
	previousVideoPTS = 0;
	file = NULL;
	suspendTransfer = FALSE;
	currentTimeInMilliSecs = 0;

	lock.Leave();

	GNRAISE_OK;
	}

//
//  Process messages
//

void VCDStreamSequencer::Message (WPARAM wParam, LPARAM lParam)
	{
	switch (wParam)
		{
		case STILL_TIME_MSG:
		case PLAYBACK_TIME_MSG:
			// Nothing to do.
			break;
		case AUTOPAUSE_REACHED_MSG:
			AutoPauseReached ();
			break;
		case PLAY_COMPLETED_MSG:
			file = NULL;
			playRequest = TRUE;
			PlaybackCompleted();
			break;
		}
	}

//
//	Set specific attributes that are needed for playback
//

Error VCDStreamSequencer::SetAttributes(BOOL svcdStdDisc)
	{
	this->followsSVCDStd = svcdStdDisc;

	GNRAISE_OK;
	}

//
//  Transfer stream and start playback if necessary
//

Error VCDStreamSequencer::TransferStream(VCDDataFile * file, DWORD firstSector, DWORD numSectors, BOOL final, int scanSpeed, BOOL hiresStill, BOOL toPause, BOOL isSegment)
	{
	DVDDiskType diskType;

	lock.Enter();

	if (firstSector & 0x80000000)
		BREAKPOINT;

//	DP("Start : num %d size %d first %d", numSectors, file->NumberOfBlocks(), firstSector);

	//
	//  Initialize stream sequencer
	//

	GNREASSERT(GetDiskType(diskType));
	if (!this->numSectors)
		{
		if (!numSectors)
			{
			GNREASSERT(file->GetNumberOfBlocks(numSectors));
			numSectors -= firstSector;
			}

		this->file = file;
		this->currentSector = firstSector;
		this->numSectors = numSectors;
		this->finalStream = final;
		this->scanSpeed = scanSpeed;
		this->hiresStill = hiresStill && player->SupportsHiResStills();
		if (diskType == DDT_SUPER_VIDEO_CD)	// S-VCD --> assume MPEG-2 (contains 2 bytes more in the pack header)
			packHeaderOffset = 2;
		else	// VCD
			packHeaderOffset = 0;
		this->isSegment = isSegment;

		scanHeaderCount = 0;
		scanHeaderBuffer = 0;
		scanFrameComplete = FALSE;
		scanFramePending = FALSE;
		scanSector = currentSector;
		averageScanDistance = 0;

		stillMode = FALSE;

		startSector = currentSector;

		if (scanSpeed) player->StartScan();

		transfering = TRUE;
		PerformTransfer(256);

		if (playRequest)
			{
			if (!scanSpeed) player->StartPlayback();
			playRequest = FALSE;
			}

		if (toPause)
			Pause();

		lock.Leave();

		GNRAISE_OK;
		}
	else
		{
		lock.Leave();

		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Abort current transfer
//

Error VCDStreamSequencer::AbortTransfer(void)
	{
	suspendTransfer = TRUE;
	if(file)
		file->TerminateRequest(&rh);
	lock.Enter();
	suspendTransfer = FALSE;

	player->CancelCallback();
	player->CancelData();
	FlushBuffers();

	lock.Leave();

	GNRAISE_OK;
	}

//
//  Perform transfer(s)
//  This function is called from another thread.
//

Error VCDStreamSequencer::PerformTransfer(int maxTransfer)
	{
	int transfer;
	Error error;

	//
	// Perform several transfers
	//

	transfer = maxTransfer;
	while (transfer && !suspendTransfer)
		{
		lock.Enter();

		transfer--;

		if (transfering && !autoPause && !scanFramePending && !suspendTransfer &&
			 (!stillMode || player->IsCompleted() &&  (!streamBuffer->IsEmpty() || !FinalSector())))
			{
			//
			//  Send data and go to next small buffer if current is empty
			//

			SendSmallBuffer();
			if (SmallBufferEmpty() && !scanFrameComplete)
				AdvanceFirstBlock();
			else
				transfer = 0;

			//
			//  Refill buffer
			//

			if (streamBuffer->IsEmpty())
				{
				if (IS_ERROR(error = RefillBuffer()))
					{
					player->TerminatePlayback();
					lock.Leave();

					GNRAISE(error);
					}
				}

			//
			//  Perform misc. actions for different playback situations
			//

			if (autoPause)
				{
				//
				//  Perform autopause
				//

				player->SetCallback(player->transferCount, this);
				player->CompleteData();

				transfer = 0;
				}
			else if (scanFrameComplete && SmallBufferEmpty())
				{
				//
				//  Perform scan
				//

				player->SetCallback(0xffffffff, this);
				player->CompleteData();
				scanFramePending = TRUE;
				if (IS_ERROR(error = FindNextScanSector()))
					{
					player->TerminatePlayback();
					lock.Leave();

					GNRAISE(error);
					}

				if (FinalSector())
					{
					scanFramePending = FALSE;
					scanSpeed = 0;
					stillMode = TRUE;
					TransferCompleted();
					}

				transfer = 0;
				}
			else if (streamBuffer->IsEmpty() && FinalSector())
				{
				//
				//  Finish transfer at end of data
				//

				if (finalStream)
					{
					player->SetCallback(player->transferCount, this);
					player->CompleteData();
					stillMode = TRUE;
					}

				TransferCompleted();

				transfer = 0;
				}
			}

		lock.Leave();
		}

	//
	//  Test on breakpoints
	//

	SendMessage(BREAKPOINT_MSG, 0);
	GNRAISE_OK;
}

//
//  Complete current stream
//

Error VCDStreamSequencer::CompleteStream(void)
	{
	lock.Enter();

	finalStream = TRUE;

	if (!stillMode)
		GNREASSERT(PerformTransfer(0xffff));

	lock.Leave();

	GNRAISE_OK;
	}

//
//  Continue after autopause
//

Error VCDStreamSequencer::ContinueAutoPause(void)
	{
	lock.Enter();

	autoPause = FALSE;

	player->CancelData();
	startSector = currentSector;
	GNREASSERT(PerformTransfer(0xffff));

	player->StartPlayback();

	lock.Leave();

	GNRAISE_OK;
	}

//
//  Player callback
//  This function is called from another thread.
//

void VCDStreamSequencer::PlayerCallback(DWORD signalPosition)
	{
	lock.Enter();

	//
	//  Test on misc playback situations
	//

	if (autoPause)
		{
		// Since we run on another thread, we cannot directly call AutoPauseReached().
		SendMessage (AUTOPAUSE_REACHED_MSG, 0);
		}
	else if (scanFramePending)
		{
		scanFramePending = FALSE;
		scanFrameComplete = FALSE;
		lock.Leave();

		PerformTransfer(0xffff);

		lock.Enter();
		}
	else
		{
		// Since we run on another thread, we cannot directly call PlaybackCompleted().
		SendMessage (PLAY_COMPLETED_MSG, 0);
		}

	lock.Leave();
	}

//
//  Return current location as sector number
//

DWORD VCDStreamSequencer::CurrentLocation(void)
	{
	if (player->transferCount)
		{
		return (startSector +
				  ScaleLong(player->CurrentLocation(), player->transferCount,
								currentSector - startSector - streamBuffer->AvailBlocks())) * CD_FRAME_SIZE;
		}
	else
		return startSector * CD_FRAME_SIZE;
	}

//
//  Return current duration (in sectors)
//

Error VCDStreamSequencer::CurrentDuration(DWORD & dur)
	{
	if (file)
		{
		GNREASSERT(file->GetNumberOfBlocks(dur));
		dur *= CD_FRAME_SIZE;
		}
	else
		dur = 0;
	GNRAISE_OK;
	}

//
//  Test if we reached the last sector
//

BOOL VCDStreamSequencer::FinalSector(void)
	{
	return scanSpeed >= 0 ? !numSectors : !currentSector;
	}

//
// Return current playback time
//

Error VCDStreamSequencer::GetPlaybackTime(DVDTime & time)
	{
	DWORD secs;

	//
	// playback
	//

	if (!scanSpeed)
		{
		GNREASSERT(player->GetPlaybackTime(time));
		}

	//
	// scanning
	//

	else if (scanSpeed < 0 || scanSpeed > 0)
		{
		//
		// time in seconds is exactly enough for Video-CD
		//

		secs = currentTimeInMilliSecs / 1000;
		time = DVDTime(0, 0, secs, 0, 25);
		}

	GNRAISE_OK;
	}
