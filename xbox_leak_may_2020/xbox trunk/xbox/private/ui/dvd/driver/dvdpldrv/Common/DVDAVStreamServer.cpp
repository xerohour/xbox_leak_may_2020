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
//  DVD Audio/Video Stream Server Classes
//
////////////////////////////////////////////////////////////////////


#include "DVDAVStreamServer.h"
#include "Library/Lowlevel/timer.h"
#include "DVDPlayer.h"


////////////////////////////////////////////////////////////////////
//
//  DVD Scattered Stream Buffer Class
//
////////////////////////////////////////////////////////////////////

class DVDScatteredStreamBuffer : public DVDStreamBuffer
	{
	protected:
		DriveBlock					*	blocks;
		DWORD							*	blockIndices;
		MPEGDataSizePair			*	buffers;
		RequestHandle					rh;

		DWORD								numBlocks;
		DVDDataFile					*	file;

		BYTE								dummyBlock[DVD_BLOCK_SIZE];

		DWORD WriteFromBuffer(DVDStreamPlayer * player, DWORD at, DWORD num);
		Error ReadIntoBuffer(DVDDataFile * file, DWORD block, DWORD at, DWORD num);
		void UnlockBuffers(DVDDataFile * file, DWORD at, DWORD num);

	public:
		DVDScatteredStreamBuffer(DWORD bufferSize);
		~DVDScatteredStreamBuffer(void);

		Error ChangeBufferSize(DWORD size);
		void Flush(void);

		Error ReadIntoBuffer(DVDDataFile * file, DVDEStdTimeAdaption * eStdAdaption, DWORD block, DWORD num); // in blocks
		DWORD WriteFromBuffer(DVDStreamPlayer * player, DWORD num); // in bytes

		Error TerminateRequest(void);
	};

//
//  Constructor
//

DVDScatteredStreamBuffer::DVDScatteredStreamBuffer(DWORD bufferSize) : DVDStreamBuffer(bufferSize)
	{
	int i;

	blocks = new DriveBlock[bufferSize];
	blockIndices = new DWORD[bufferSize];
	buffers = new MPEGDataSizePair[bufferSize];
	file = NULL;
	numBlocks = bufferSize;
	for (i=0; i<DVD_BLOCK_SIZE; i++)
		dummyBlock[i] = 0;
	}

//
//  Destructor
//

DVDScatteredStreamBuffer::~DVDScatteredStreamBuffer(void)
	{
	Flush();
	delete[] blocks;
	delete[] blockIndices;
	delete[] buffers;
	}

//
//  Change buffer size
//

Error DVDScatteredStreamBuffer::ChangeBufferSize(DWORD size)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Unlock buffers
//

void DVDScatteredStreamBuffer::UnlockBuffers(DVDDataFile * file, DWORD at, DWORD num)
	{
	DWORD i;

	while (num)
		{
		i = 1;
		while (i < num && blockIndices[at+i] == blockIndices[at] + i) i++;

		file->UnlockBlocks(blockIndices[at], i, blocks + at, DAF_STREAMING | DAT_UNLOCK_CLEAN, &rh);

		at += i;
		num -= i;
		}

//	DP("Unlock %x %d %08lx", blockIndices[at], num, err);
	}

//
//  Flush
//

void DVDScatteredStreamBuffer::Flush(void)
	{
	DWORD s, e;

	if (file)
		{
		s = bufferStart / DVD_BLOCK_SIZE;
		e = bufferEnd / DVD_BLOCK_SIZE;

		if (e < numBlocks)
			{
			UnlockBuffers(file, s, e - s);
			}
		else
			{
			UnlockBuffers(file, s, numBlocks - s);
			UnlockBuffers(file, 0, e - numBlocks);
			}

		file = NULL;
		}

	DVDStreamBuffer::Flush();
	}

//
//  Read into buffer (internal)
//

Error DVDScatteredStreamBuffer::ReadIntoBuffer(DVDDataFile * file, DWORD block, DWORD at, DWORD num)
	{
	Error err;
	int i;

	#ifdef DEVKIT
    if (at + num > numBlocks) {
        OutputDebugStringA("HEAP CORRUPTION ABOUT TO OCCUR: CONTACT TRACYSH\n");
        __asm int 3;
    }
    #endif

	err = file->LockBlocks(block, num, blocks + at, DAF_STREAMING | DAT_LOCK_AND_READ, &rh);

//	DP("Lock %x %d %08lx", block, num, err);

	//
	//  Now initialize buffers with drive blocks
	//  In case of errors replace blocks by dummy block
	//

	for (i=0; i<(int)num; i++)
		{
		if (IS_ERROR(blocks[at + i].err))
			blocks[at + i].data = dummyBlock;

		blockIndices[at + i] = block + i;
		buffers[at + i].data = blocks[at+i].data;
		buffers[at + i].size = DVD_BLOCK_SIZE;
		buffers[at + i].timeStamp = -1;
		}

	GNRAISE(err);
	}

//
//  Read into buffer
//

Error DVDScatteredStreamBuffer::ReadIntoBuffer(DVDDataFile * file, DVDEStdTimeAdaption * eStdAdaption, DWORD block, DWORD num)
	{
	DWORD tnum;
	int i;
	Error err0 = GNR_OK;
	Error err1 = GNR_OK;

	this->file = file;

#ifdef DEVKIT
if (bufferStart > bufferSize) {
    OutputDebugStringA("ReadIntoBuffer assertion #1: CONTACT TRACYSH\n");
    __asm int 3;
}
if (bufferEnd - bufferStart > bufferSize) {
    OutputDebugStringA("ReadIntoBuffer assertion #2: CONTACT TRACYSH\n");
    __asm int 3;
}
#endif


	//
	//  Read data from the end of the current data to the end of the buffer
	//  (Note: to prevent overwriting data at bufferStart num must be smaller
	//  than the number of blocks left)
	//

	if (bufferEnd < bufferSize)
		{
		tnum = (bufferSize - bufferEnd) / DVD_BLOCK_SIZE;
		if (tnum > num) tnum = num;

		err0 = ReadIntoBuffer(file, block, bufferEnd / DVD_BLOCK_SIZE, tnum);
		if (!IS_ERROR(err0))
			{
			for (i=0; i<(int)tnum; i++)
				eStdAdaption->ConvertToEStdTime(blocks[bufferEnd / DVD_BLOCK_SIZE + i].data, 1);
			}

		bufferEnd += tnum * DVD_BLOCK_SIZE;
		num -= tnum;
		block += tnum;
		}

	//
	//  If we still have to read data then we must perform a wraparound
	//

	if (num)
		{
		tnum = (bufferSize + bufferStart - bufferEnd) / DVD_BLOCK_SIZE;

		if (tnum > num) tnum = num;

		err1 = ReadIntoBuffer(file, block, (bufferEnd - bufferSize) / DVD_BLOCK_SIZE, tnum);
		if (!IS_ERROR(err1))
			{
			for (i=0; i<(int)tnum; i++)
				eStdAdaption->ConvertToEStdTime(blocks[(bufferEnd - bufferSize) / DVD_BLOCK_SIZE + i].data, 1);
			}

		bufferEnd += tnum * DVD_BLOCK_SIZE;
		num -= tnum;
		}

	if (bufferStart >= bufferSize)
		{
		bufferStart -= bufferSize;
		bufferEnd -= bufferSize;
		}

	if (err0 == GNR_OPERATION_ABORTED) err0 = GNR_OK;
	if (err1 == GNR_OPERATION_ABORTED) err1 = GNR_OK;

	if (err0) return err0;
	else return err1;
	}

//
//  Write from buffer to player
//

DWORD DVDScatteredStreamBuffer::WriteFromBuffer(DVDStreamPlayer * player, DWORD at, DWORD num)
	{
	DWORD done, blk, offset;
	RequestHandle rh;

	blk = at / DVD_BLOCK_SIZE;
	offset = at % DVD_BLOCK_SIZE;

	buffers[blk].data = blocks[blk].data + offset;
	buffers[blk].size = DVD_BLOCK_SIZE - offset;
	done = player->SendDataMultiple(buffers+blk, num);

	DWORD s = at / DVD_BLOCK_SIZE;
	DWORD e = (at + done) / DVD_BLOCK_SIZE;

	UnlockBuffers(file, s, e - s);
	availData -= (e - s) * DVD_BLOCK_SIZE;

	return done;
	}

//
//  Write from buffer
//  num must be smaller than the amount of data in the buffer
//

DWORD DVDScatteredStreamBuffer::WriteFromBuffer(DVDStreamPlayer * player, DWORD num)
	{
	DWORD tnum, done;

#ifdef DEVKIT
    static DWORD __bufferStart;
    static DWORD __bufferEnd;
    static DWORD __num;
    __bufferStart = bufferStart;
    __bufferEnd = bufferEnd;
    __num = num;
    if (bufferStart > bufferSize) {
        OutputDebugStringA("WriteFromBuffer assertion #1: CONTACT TRACYSH\n");
        __asm int 3;
    }
    if (bufferEnd - bufferStart > bufferSize) {
        OutputDebugStringA("WriteFromBuffer assertion #3: CONTACT TRACYSH\n");
        __asm int 3;
    }
    if (bufferEnd - bufferStart < num) {
        OutputDebugStringA("WriteFromBuffer assertion #5: CONTACT TRACYSH\n");
        __asm int 3;
    }
#endif



	if (bufferEnd <= bufferSize || bufferStart + num < bufferSize)
		{
		// single data block

		done = WriteFromBuffer(player, bufferStart, num);
		bufferStart += done;

#ifdef DEVKIT
        if (bufferStart > bufferSize) {
            OutputDebugStringA("WriteFromBuffer assertion #8: CONTACT TRACYSH\n");
            __asm int 3;
        }
#endif
		
		}
	else
		{
		// double data block, both needed

		tnum = bufferSize - bufferStart;

		if (tnum)
			{
			done = WriteFromBuffer(player, bufferStart, tnum);
			bufferStart += done;
			num -= done;
 #ifdef DEVKIT
            if (bufferStart > bufferSize) {
                OutputDebugStringA("WriteFromBuffer assertion #6: CONTACT TRACYSH\n");
                __asm mov eax, done;
                __asm mov ecx, tnum;
                __asm mov edx, num;
                __asm int 3;
            }
#endif

			}

		if (bufferStart == bufferSize)
			{
			bufferStart -= bufferSize;
			bufferEnd -= bufferSize;

			if (num && tnum == done)
				{
				done = WriteFromBuffer(player, bufferStart, num);
				bufferStart += done;
#ifdef DEVKIT
                if (bufferStart > bufferSize) {
                    OutputDebugStringA("WriteFromBuffer assertion #7: CONTACT TRACYSH\n");
                    __asm mov eax, done;
                    __asm mov ecx, tnum;
                    __asm mov edx, num;
                    __asm int 3;
                }
#endif
				
				}
			}
		}

//	DP("After send %d vs %d", bufferStart, bufferEnd);

#ifdef DEVKIT
if (bufferStart > bufferSize) {
    OutputDebugStringA("WriteFromBuffer assertion #2: CONTACT TRACYSH\n");
    __asm mov eax, done;
    __asm mov ecx, tnum;
    __asm mov edx, num;
    __asm int 3;
}
if (bufferEnd - bufferStart > bufferSize) {
    OutputDebugStringA("WriteFromBuffer assertion #4: CONTACT TRACYSH\n");
    __asm mov eax, done;
    __asm mov ecx, tnum;
    __asm mov edx, num;
    __asm int 3;
}
#endif



	GNRAISE_OK;
	}

Error DVDScatteredStreamBuffer::TerminateRequest(void)
	{
	if (file)
		return file->TerminateRequest(&rh);
	else
		GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Stream Server Class
//
//////////////////////////////////////////////////////////////////////

#define ANGLE_CHANGE_NONE 0
#define ANGLE_CHANGE_CELL 0x7fffffff

//
//  Constructor
//

DVDVideoStreamServer::DVDVideoStreamServer(DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: DVDStreamServer(seq, server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	{
	int i;

	for(i=0; i<NUM_VOBU_BUFFERS; i++)
		{
		vobus[i].succ = vobus + (i + 1) % NUM_VOBU_BUFFERS;
		}

	FlushBuffers();  // Initializes cell buffers

	transferSemaphore = 0;
	positionCache.valid = FALSE;
	}

//
//  Destructor
//

DVDVideoStreamServer::~DVDVideoStreamServer(void)
	{
	if (streamBuffer)
		{
		delete streamBuffer;
		streamBuffer = NULL;
		}
	}

//
//  Initialize
//

Error DVDVideoStreamServer::Init(DVDFileSystem * dvdfs)
	{
	DVDStreamServer::Init(dvdfs);

	this->dvdfs = dvdfs;
	player->SetRefiller(this);

	GNRAISE_OK;
	}

//
//  Test, if readingVOBU is the last VOBU of the cell
//

BOOL DVDVideoStreamServer::IsFinalVOBUOfCell(void)
	{
//	DP("************** FirstBlock %d FirstVOBU %d", transferingVOBU->firstBlock, transferingCell->info->LastVOBUStart());
	if (readingCell->scan == CST_REVERSEPLAYBACK)
		return readingVOBU->firstBlock == readingCell->info.FirstVOBUStart();
	else
		return readingVOBU->firstBlock == readingCell->info.LastVOBUStart();
	}

//
//  Adapt PTM Entry
//

static void AdaptPTMEntry(BYTE * data, DWORD ptsOffset)
	{
	DWORD ptm, ptm2;
	ptm = ((DWORD)(data[0]) << 23) |
		   ((DWORD)(data[1]) << 15) |
		   ((DWORD)(data[2]) <<  7) |
		   ((DWORD)(data[3]) >>  1);

	ptm2 = ptm;

	if (ptm < 0x7fffffff && ptm != 0)
		{
		ptm += ptsOffset;
		data[0] =                    (BYTE)((ptm >> 23) & 0xff);
		data[1] =                    (BYTE)((ptm >> 15) & 0xff);
		data[2] =                    (BYTE)((ptm >>  7) & 0xff);
		data[3] = (data[3] & 0x01) | (BYTE)((ptm <<  1) & 0xfe);
		}


	}

//
//  Convert to ESdt time
//

void DVDVideoStreamServer::ConvertToEStdTime(BYTE * data, int num)
	{
	}

//
//  Refill Buffer
//

Error DVDVideoStreamServer::RefillBuffer(BOOL prefetch)
	{
	int cnt = 0;
	int availDVDBlockSpaceInBuffer, numDVDBlocksTransfered;
	Error err;
	ERSBreakpoint bp;

	//
	//  Read Data from disk
	//

	availDVDBlockSpaceInBuffer = streamBuffer->AvailSpace() / DVD_BLOCK_SIZE;
	if (availDVDBlockSpaceInBuffer > 32) availDVDBlockSpaceInBuffer = 32;
	numDVDBlocksTransfered = 0;

	//
	// The stillPending condition prevents additional data from entering the
	// buffer if a VOBU with a still condition is complete inside the buffer
	//

	while (!stillPending && !suspendTransfer &&
		    availDVDBlockSpaceInBuffer &&
			 numDVDBlocksTransfered < 64 &&
			 (prefetch || streamBuffer->IsEmpty()) &&
			 (!stillMode || playingCell->scan != CST_TRICKPLAY))
		{
		cnt ++;
		if (cnt == 40)
			{
			BREAKPOINT;
			GNRAISE(GNR_RANGE_VIOLATION);
			}

		//
		// VOS_FREE
		//

		//
		// Check, whether there is no VOBU being read currently
		//
		if (readingVOBU->state == VOS_FREE)
			{
			//
			// If so, check if there is a cell available that can supply a new VOBU to read
			//
			predReadingVOBU = NULL;
			if (readingCell->state == VOS_FREE) GNRAISE_OK;

			//
			// Check, whether there is no Cell being read currently
			//
			if (readingCell->state == VOS_READY)
				{
				DWORD firstBlock;

				//
				// If so, start reading the next available cell
				//
				readingCell->state = VOS_READING;

				//
				//  Identify the position of the first VOBU in the cell based on playback direction
				//

				if (readingCell->scan == CST_SCANBACKWARD || readingCell->scan == CST_REVERSEPLAYBACK)
					firstBlock = readingCell->info.LastVOBUStart();
				else
					firstBlock = readingCell->info.FirstVOBUStart();

				//
				//  Prepare to read the first VOBU of this cell
				//  If this vobu is played in scan mode, only a single frame of it
				//  will be read and sent
				//

				firstFreeVOBU->Init(file, readingCell, firstBlock, readingCell->scan == CST_SCANFORWARD || readingCell->scan == CST_SCANBACKWARD);
				//
				// Check if there is a scanning offset in this cell
				//
				if (readingCell->scanStart)
					{
					if (readingCell->scan == CST_SCANFORWARD)
						{
						GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
						firstFreeVOBU->firstBlock = firstFreeVOBU->NextTemporalVOBU(readingCell->scanStart);
						readingCell->firstScanVOBUTime = firstFreeVOBU->GetTimeOffset();
						readingCell->firstScanSystemTime = Timer.GetMilliTicks();
						}
					else if (transferingCell->scan == CST_SCANBACKWARD)
						{
						GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
						firstFreeVOBU->firstBlock = firstFreeVOBU->PrevTemporalVOBU(readingCell->scanStart);
						readingCell->firstScanVOBUTime = firstFreeVOBU->GetTimeOffset();
						readingCell->firstScanSystemTime = Timer.GetMilliTicks();
						}
					}
				else
					{
					if (transferingCell->scan == CST_SCANFORWARD)
						{
						GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
						readingCell->firstScanVOBUTime = firstFreeVOBU->GetTimeOffset();
						readingCell->firstScanSystemTime = Timer.GetMilliTicks();
						}
					else if (transferingCell->scan == CST_SCANBACKWARD)
						{
						GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
						readingCell->firstScanVOBUTime = firstFreeVOBU->GetTimeOffset();
						readingCell->firstScanSystemTime = Timer.GetMilliTicks();
						}
					}

				//
				// Advance in the list of VOBUs
				//
				firstFreeVOBU = firstFreeVOBU->succ;

				}
			}

		//
		// VOS_READY
		//

		//
		// Check if we start to read a new VOBU
		//
		if (readingVOBU->state == VOS_READY)
			{
			//
			// If so, check if there are additional VOBU buffers available,
			// and if not, don't start reading
			//
			if (firstFreeVOBU->state != VOS_FREE) 
			    {

                //
                // This is a workaround for the bug where the signal gets lost somewhere,
                // the player callback never gets called and the VOBUs never get released.
                // This is not the correct fix but it should get things going again.
                //
                
                player->SetCallback(playingVOBU->endTransferCount, this);
                
			    GNRAISE_OK;
			    }

			//
			// Set this VOBU to reading state
			//
			predReadingVOBU = readingVOBU;
			readingVOBU->state = VOS_READING;

			//
			//  Read nav pack of VOBU
			//
			err = readingVOBU->ReadNavigationPack();

			if (IS_ERROR(err))
				{
				if (err == GNR_FILE_READ_ERROR)
					{
					//
					//  In case of error find next nav pack within cell
					//

					SendEvent(DNE_READ_ERROR, 0);
					if (IS_ERROR(err = readingVOBU->FindNextNavigationPack(readingCell->info.LastVOBUStart(), sequencer->CurrentAngle(), suspendTransfer)))
						{

						SendEvent(DNE_READ_ERROR, 0);
						if (err == GNR_RANGE_VIOLATION)
							{
							inTransfer = FALSE;
							SendCellCorrupt(readingCell);
							GNRAISE_OK;
//							readingVOBU->Init(file, readingCell, readingCell->info.LastVOBUStart(), readingCell->scan == CST_SCANFORWARD || readingCell->scan == CST_SCANBACKWARD);
//							readingVOBU->numBlocks = 1;
//							readingVOBU->state = VOS_READING;
							}
						else if (err == GNR_OPERATION_ABORTED)
							{
							readingVOBU->state = VOS_READY;
							continue;
							}
						else
							{
							readingVOBU->state = VOS_READY;

							GNRAISE(err);
							}
						}
					}
				else if (err == GNR_OPERATION_ABORTED)
					{
					readingVOBU->state = VOS_READY;
					continue;
					}
				else
					{
					readingVOBU->state = VOS_READY;

					GNRAISE(err);
					}
				}

			//
			// Calculate transfer positions in stream block numbers for this VOBU
			//
			readingVOBU->startReadCount = currentReadCount;
			readingVOBU->endReadCount = currentReadCount + readingVOBU->numBlocks;


//			DP("Transfering blocks %08lx-%08lx", transferingVOBU->firstBlock, transferingVOBU->firstBlock + transferingVOBU->numBlocks);
//			DP("Starting transfer of %d-%d", transferingVOBU->startTransferCount, transferingVOBU->endTransferCount);

			if (readingCell->scan == CST_TRICKPLAY)
				{
				readingVOBU->final = FALSE;
				if (readingVOBU->firstBlock == readingVOBU->cell->info.FirstVOBUStart())
					player->SetFirstAvailableVOBUID(currentVOBUID);
				if (readingVOBU->firstBlock == readingVOBU->cell->info.LastVOBUStart())
					player->SetLastAvailableVOBUID(currentVOBUID);
				}
			else
				{
				readingVOBU->final = FALSE;

				//
				// Check if we are in scanning mode
				//
				if (readingVOBU->singleFrame)
					{
					DWORD firstBlock;

					//
					// If so...
					//
					if (readingCell->scan == CST_SCANFORWARD)
						{
						int timeSkip;
						int timeDist;

						if (!readEveryVOBU)
							{
							//
							// Calculate VOBU offset for a given scan speed in forward scan
							//

							timeDist = (readingVOBU->GetTimeOffset() - readingCell->firstScanVOBUTime).Millisecs();
							timeSkip = Timer.GetMilliTicks() - readingCell->firstScanSystemTime;

							timeDist = (timeSkip * readingVOBU->cell->scanStep - timeDist);	// Delta between "should be" and "real" position

							if (timeDist < 0)
								timeDist = -1;

							if (sequencer->BreakpointContained((readingVOBU->cell->presentationTime + readingVOBU->GetTimeOffset()).Millisecs(),
								timeDist, TRUE, playbackDir, bp))
								{
								timeDist = 0;
								readEveryVOBU = TRUE;
								}
							}
						else
							timeDist = 0;

						if (timeDist < 0)
							firstBlock = readingVOBU->firstBlock;
						else
							firstBlock = readingVOBU->NextTemporalVOBU(timeDist / 500);
						}
					else if (readingCell->scan == CST_SCANBACKWARD)
						{
						int timeSkip;
						int timeDist;

						if (!readEveryVOBU)
							{
							//
							// Calculate VOBU offset for a given scan speed in forward scan
							//

							timeDist = (readingCell->firstScanVOBUTime - readingVOBU->GetTimeOffset()).Millisecs();
							timeSkip = Timer.GetMilliTicks() - readingCell->firstScanSystemTime;

							timeDist = (timeSkip * readingVOBU->cell->scanStep - timeDist);	// Delta between "should be" and "real" position

							if (timeDist < 0)
								timeDist = -1;

							if (sequencer->BreakpointContained((readingVOBU->cell->presentationTime + readingVOBU->GetTimeOffset()).Millisecs() - timeDist,
								timeDist, FALSE, playbackDir, bp))
								{
								timeDist = 0;
								readEveryVOBU = TRUE;
								}
							}
						else
							timeDist = 0;

						if (timeDist < 0)
							firstBlock = readingVOBU->firstBlock;
						else
							firstBlock = readingVOBU->PrevTemporalVOBU(timeDist / 500);
						}

					//
					// Check if the currently read VOBU is the last in the cell while scanning
					//
					if (firstBlock == 0x3fffffff)
						{
						//
						// If so, mark it as final
						//
						readingVOBU->final = TRUE;
						}
					else
						{
						//
						// If not, initialize the next VOBU
						//

						firstFreeVOBU->Init(file, readingVOBU->cell, firstBlock, TRUE);
						firstFreeVOBU = firstFreeVOBU->succ;
						}
					}
				else
					{
					//
					// We are not in scanning mode
					// Check if the current VOBU is not the last one of the cell
					//
					if (!IsFinalVOBUOfCell())
						{
						//
						// If this is the case, initialize the next VOBU (startBlock is done below)
						//

						firstFreeVOBU->Init(file, readingVOBU->cell, 0, FALSE);

						//
						// Check if there is a angle change request and the current VOBU is the last
						// one of the ILVU
						//
						if (angleChangeRequest && readingVOBU->IsLastVOBUinILVU())
							{
							//
							// If so, move to the new angle
							//
							firstFreeVOBU->firstBlock = readingVOBU->SeamlessAngleChangeVOBU(angleChangeRequest);
							angleChangeVOBU = firstFreeVOBU->firstBlock;
							angleChangeAngle = angleChangeRequest;
							angleChangeRequest = 0;
							if (firstFreeVOBU->firstBlock == 0x7fffffff)
								firstFreeVOBU->firstBlock = readingVOBU->NextVOBU();
							}
						else
							{
							//
							// If no pending angle change, just pick the next VOBU based on playback direction
							//
							if (readingCell->scan == CST_REVERSEPLAYBACK)
								firstFreeVOBU->firstBlock = readingVOBU->PrevVOBU();
							else
								firstFreeVOBU->firstBlock = readingVOBU->NextVOBU();

							if (angleChangeRequest)
								{
								angleChangeVOBU = firstFreeVOBU->firstBlock;
								angleChangeAngle = angleChangeRequest;
								}
							}
	//					DP("Preparing transfer of %d", firstFreeVOBU->firstBlock);
						firstFreeVOBU = firstFreeVOBU->succ;
						}
					else
						{
						//
						// If this is the final VOBU of the cell, mark it so
						//
						readingVOBU->final = TRUE;
						}
					}
				}


			//
			// Set read position and size
			//
			nextVOBUBlock = readingVOBU->firstBlock;
			numVOBUBlocks = readingVOBU->numBlocks;
			}

		//
		// Check if there is a VOBU to read from
		//
		if (readingVOBU->state & VOS_READING)
			{
			//
			// And there is still data inside this VOBU which is not yet read
			//
			if (numVOBUBlocks)
				{
				//
				// Check if there is more data inside the VOBU than what would fit into the buffer
				//
				if (numVOBUBlocks > (DWORD)availDVDBlockSpaceInBuffer)
					{
					//
					// If so, just fill the buffer
					//
					if (IS_ERROR(err = streamBuffer->ReadIntoBuffer(file, this, nextVOBUBlock, availDVDBlockSpaceInBuffer)))
						{
						SendEvent(DNE_READ_ERROR, 0);

						if (err != GNR_FILE_READ_ERROR)
							GNRAISE(err);
						else
							err = GNR_OK;
						}

					//
					// Update the buffer levels and read positions
					//
					currentReadCount += availDVDBlockSpaceInBuffer;
					nextVOBUBlock += availDVDBlockSpaceInBuffer;
					numDVDBlocksTransfered += availDVDBlockSpaceInBuffer;
					numVOBUBlocks -= availDVDBlockSpaceInBuffer;
					finalBuffer = FALSE;
					}
				else
					{
					//
					// If the VOBU data fits into the buffer,
					// read all the data into the buffer
					//
					if (IS_ERROR(err = streamBuffer->ReadIntoBuffer(file, this, nextVOBUBlock, numVOBUBlocks)))
						{
						SendEvent(DNE_READ_ERROR, 0);

						if (err != GNR_FILE_READ_ERROR)
							GNRAISE(err);
						else
							err = GNR_OK;
						}

//					DP("Transfering final");

					//
					// Update the buffer levels and read positions
					//
					numDVDBlocksTransfered += numVOBUBlocks;
					currentReadCount += numVOBUBlocks;
					nextVOBUBlock += numVOBUBlocks;
					numVOBUBlocks = 0;

					//
					// Set flag, that is will be the final buffer of this VOBU
					//
					finalBuffer = TRUE;

					//
					// remove the reading attribute from the current VOBU state
					//
					readingVOBU->state &= ~VOS_READING;
					readingVOBU->state |= VOS_READCOMPLETE;

					if (readingVOBU->singleFrame || readingCell->scan == CST_TRICKPLAY)
						{
						stillPending = TRUE;
						}
					else if (readingCell->info.ContinuousPresentation())
						{
						readingVOBU->still = TRUE;
						stillPending = TRUE;
						}

					//
					// If this is the final VOBU of the cell
					//
					if (readingVOBU->final)
						{
						//
						// Finish reading of the cell
						//
						readingCell->state &= ~VOS_READING;
						readingCell->state |= VOS_READCOMPLETE;

//						DP("VOBU final of cell %08lx", readingCell);
						if (readingCell->still)
							{
//							DP("Reading cell is still");
							stillPending = TRUE;
							}

						readingCell = readingCell->succ;
						}

					readingVOBU = readingVOBU->succ;
					}
				}
			else
				{
				//
				// Set flag, that is will be the final buffer of this VOBU
				//
				finalBuffer = TRUE;

				//
				// remove the reading attribute from the current VOBU state
				//
				readingVOBU->state &= ~VOS_READING;
				readingVOBU->state |= VOS_READCOMPLETE;

				if (readingVOBU->singleFrame || readingCell->scan == CST_TRICKPLAY)
					{
					stillPending = TRUE;
					}
				else if (readingCell->info.ContinuousPresentation())
					{
					readingVOBU->still = TRUE;
					stillPending = TRUE;
					}

				//
				// If this is the final VOBU of the cell
				//
				if (readingVOBU->final)
					{
					//
					// Finish reading of the cell
					//
					readingCell->state &= ~VOS_READING;
					readingCell->state |= VOS_READCOMPLETE;

//						DP("VOBU final of cell %08lx", readingCell);
					if (readingCell->still)
						{
//							DP("Reading cell is still");
						stillPending = TRUE;
						}

					readingCell = readingCell->succ;
					}

				readingVOBU = readingVOBU->succ;
				}
			}

		availDVDBlockSpaceInBuffer = streamBuffer->AvailSpace() / DVD_BLOCK_SIZE;
		if (availDVDBlockSpaceInBuffer > 32) availDVDBlockSpaceInBuffer = 32;
		}

	GNRAISE_OK;
	}

//
//  Send buffer to player
//

Error DVDVideoStreamServer::SendBuffer(void)
	{
	DWORD done, num;

	if (!stillMode || playRequest)
		{
		//
		//  Send as much data as we have or as much as the decoder wants
		//

		do {
			//
			//  Look for transfering cell
			//

			if (!(transferingCell->state & VOS_TRANSFERING))
				{
				if (transferingCell->state & (VOS_READING | VOS_READCOMPLETE))
					{
					transferingCell->state |= VOS_TRANSFERING;

					if (transferingCell == playingCell)
						{
						playingCell->state |= VOS_PLAYING;
						SendCellStarted(playingCell);
						}
					}
				else
					GNRAISE_OK;
				}

			//
			//  Look for transfering VOBU
			//

			if (!(transferingVOBU->state & VOS_TRANSFERING))
				{
				if (transferingVOBU->state & (VOS_READING | VOS_READCOMPLETE))
					{
					transferingVOBU->state |= VOS_TRANSFERING;

					transferingVOBU->startTransferCount = player->transferCount;
					transferingVOBU->endTransferCount   = transferingVOBU->startTransferCount + transferingVOBU->numBlocks;

//					DP("*** Mark VOBU from %08lx to %08lx", transferingVOBU->startTransferCount, transferingVOBU->endTransferCount);

					}
				else
					GNRAISE_OK;
				}

			//
			//  Transfer Data
			//

			num = (transferingVOBU->endTransferCount - player->transferCount) * DVD_BLOCK_SIZE - player->transferCountRemainder;
//			DP("*** Num is %08lx buffer %08lx", num, streamBuffer->AvailData());
			if (num > streamBuffer->AvailData()) num = streamBuffer->AvailData();

			if (transferingVOBU == playingVOBU && !(playingVOBU->state & VOS_PLAYING))
				{
				playingVOBU->state |= VOS_PLAYING;

				SendVOBUStarted(playingVOBU);
				player->SetTimeBase(playingVOBU->GetTimeOffset() + playingVOBU->cell->presentationTime);

//				DP("*** First Set Signal at %08lx %x %d", playingVOBU->endTransferCount, playingVOBU->state, playingVOBU->singleFrame);
				if (playingVOBU->singleFrame)
					player->SetCallback(0x3fffffff, this);
				else
					player->SetCallback(playingVOBU->endTransferCount, this);
				}

//			DP("*** Check VOBU till %08lx at %08lx", transferingVOBU->endTransferCount, player->transferCount);

			done = streamBuffer->WriteFromBuffer(player, num);

			//
			// If all data of a VOBU has been transfered go to next cell/vobu
			//
			if (player->transferCount >= transferingVOBU->endTransferCount)
				{
				transferingVOBU->state &= ~VOS_TRANSFERING;
				transferingVOBU->state |= VOS_TRANSFERCOMPLETE;

				if (transferingVOBU->final)
					{
					transferingCell->state &= ~VOS_TRANSFERING;
					transferingCell->state |= VOS_TRANSFERCOMPLETE;

					transferingCell = transferingCell->succ;
					}

				transferingVOBU = transferingVOBU->succ;
				}
			} while (done && !(streamBuffer->IsEmpty()));
		}

//	DP("SB %d %d %d", BufferEmpty(), finalBuffer, stillPending);

	//
	//  If buffer is empty and it is the last one before still, complete data
	//

	if (streamBuffer->IsEmpty() && finalBuffer && stillPending)
		{
//		DP("Buffer finished");
		player->CompleteData();
		stillMode = TRUE;
		stillPending = FALSE;
		finalBuffer = FALSE;
		endPTM = 0;
		ptsOffset = 22500;
		}

	GNRAISE_OK;
	}

//
//  Flush Buffers
//

Error DVDVideoStreamServer::FlushBuffers(void)
	{
	int i;

	for(i=0; i<NUM_VOBU_BUFFERS; i++)
		{
		vobus[i].Cleanup();
		}

	firstFreeVOBU = readingVOBU = transferingVOBU = playingVOBU = vobus;

	for(i=0; i<NUM_CELL_BUFFERS; i++)
		{
		cells[i].state = VOS_FREE;
		}

	firstFreeCell = readingCell = transferingCell = playingCell = cells;

	if (streamBuffer) streamBuffer->Flush();
	numVOBUBlocks = 0;

	stillMode = FALSE;
	stillPending = FALSE;
	finalBuffer = FALSE;
	playRequest = TRUE;
	inTransfer = FALSE;
	suspendTransfer = FALSE;

	callbackSemaphore	= FALSE;
	angleChangeRequest = 0;
	angleChangeVOBU = ANGLE_CHANGE_NONE;

	readEveryVOBU = FALSE;
	stopAfterNextVOBU = FALSE;

	endPTM = 0;
	ptsOffset = 22500;

	currentReadCount = 0;

	FlushMessages();

	GNRAISE_OK;
	}

//
//  Message sending functions
//

#define MSG_VOBU_STARTED			0x2000
#define MSG_VOBU_COMPLETED			0x2001
#define MSG_CELL_STARTED			0x2002
#define MSG_CELL_COMPLETED			0x2003
#define MSG_PLAYBACK_TERMINATED	0x2004
#define MSG_CELL_CORRUPT			0x2005

Error DVDVideoStreamServer::SendPlaybackTerminated(Error err)
	{
	SendMessage(MSG_PLAYBACK_TERMINATED, (DWORD)err);

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::SendVOBUStarted(DVDVOBU * vobu)
	{
	WORD w;

	w = vobu->GetPCIWord(4);

	player->SetAnalogCopyProtectionMode(XTBFW(14, 2, w));

	SendMessage(MSG_VOBU_STARTED, (DWORD)vobu);

	//
	//  Send Angle Change Event
	//

	if (angleChangeVOBU == vobu->firstBlock)
		{
		SendEvent(DNE_ANGLE_CHANGE, angleChangeAngle);
		angleChangeVOBU = ANGLE_CHANGE_NONE;
		sequencer->AngleChanged();
		}

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::SendVOBUCompleted(DVDVOBU * vobu)
	{
	SendMessage(MSG_VOBU_COMPLETED, (DWORD)vobu);

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::SendCellStarted(DVDCell * cell)
	{
	if (playbackDir == EPD_FORWARD)
		sequencer->BeginOfCellBreakpointReached(cell, EPD_FORWARD, cell->breakpoint);

	SendMessage(MSG_CELL_STARTED, (DWORD)cell);
	if (angleChangeVOBU == ANGLE_CHANGE_CELL)
		{
		SendEvent(DNE_ANGLE_CHANGE, angleChangeAngle);
		angleChangeVOBU = ANGLE_CHANGE_NONE;
		sequencer->AngleChanged();
		}

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::SendCellCompleted(DVDCell * cell)
	{
	SendMessage(MSG_CELL_COMPLETED, (DWORD)cell);

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::SendCellCorrupt(DVDCell * cell)
	{
	SendMessage(MSG_CELL_CORRUPT, (DWORD)cell);

	GNRAISE_OK;
	}

void DVDVideoStreamServer::Message(WPARAM wParam, LPARAM dParam)
	{
	switch (wParam)
		{
		case MSG_VOBU_STARTED:
			sequencer->VOBUStarted((DVDVOBU *)dParam);
			break;
		case MSG_VOBU_COMPLETED:
			sequencer->VOBUCompleted((DVDVOBU *)dParam);
			break;
		case MSG_CELL_STARTED:
			sequencer->CellStarted((DVDCell *)dParam);
			break;
		case MSG_CELL_COMPLETED:
			sequencer->CellCompleted((DVDCell *)dParam);
			break;
		case MSG_CELL_CORRUPT:
			sequencer->CellCorrupt((DVDCell *)dParam);
			break;
		case MSG_PLAYBACK_TERMINATED:
			sequencer->PlaybackTerminated((Error)dParam);
			break;
		}
	}

//
//  Advance trick mode
//

// TO BE DONE
Error DVDVideoStreamServer::AdvanceTrickMode(void)
	{
	DWORD req;
	DVDVOBU	*	prevVOBU;
	DWORD finalBlock;

	req = player->GetRequestedVOBUID();

	if (currentVOBUID != req && (playingCell->state & VOS_PLAYING))
		{
		firstFreeVOBU->singleFrame = FALSE;

		prevVOBU = playingVOBU;

		if (req < currentVOBUID)
			finalBlock = playingCell->info.FirstVOBUStart();
		else
			finalBlock = playingCell->info.LastVOBUStart();

		while (req != currentVOBUID)
			{
			if (prevVOBU->firstBlock == finalBlock)
				{
				DP("############# HELP ################\n");

				GNRAISE_OK;
				}
			else
				{
				if (req < currentVOBUID)
					{
					firstFreeVOBU->Init(file, playingCell, prevVOBU->PrevVOBU(), FALSE);

//					firstFreeVOBU->firstBlock = prevVOBU->PrevVOBU();
					if (IS_ERROR(firstFreeVOBU->ReadNavigationPack()))
						SendEvent(DNE_READ_ERROR, 0);
					prevVOBU = firstFreeVOBU;
					currentVOBUID--;
					}
				else
					{
					firstFreeVOBU->Init(file, playingCell, prevVOBU->NextVOBU(), FALSE);

//					firstFreeVOBU->firstBlock = prevVOBU->NextVOBU();
					if (IS_ERROR(firstFreeVOBU->ReadNavigationPack()))
						SendEvent(DNE_READ_ERROR, 0);
					prevVOBU = firstFreeVOBU;
					currentVOBUID++;
					}
				}
			}

		playingVOBU->state = VOS_PLAYCOMPLETED;
		playingVOBU->breakpoint = ERSBreakpoint();
		SendVOBUCompleted(playingVOBU);

//		firstFreeVOBU->Init(file, playingVOBU->cell, 0, FALSE);

		firstFreeVOBU = firstFreeVOBU->succ;

		playingVOBU = playingVOBU->succ;

		stillMode = FALSE;
		}

	GNRAISE_OK;
	}

//
//  Perform Cell Transfer
//

Error DVDVideoStreamServer::PerformTransfer(int maxTransfer)
	{
	int transfer;
	Error error;

	transfer = maxTransfer;
	while (transfer && !suspendTransfer)
		{
		lock.Enter();

		transfer--;

		if (inTransfer && !suspendTransfer)
			{
			if (transferingCell->scan == CST_TRICKPLAY)
				{
				if (stillMode) AdvanceTrickMode();
				}

			if (!stillMode || player->Completed())
				{
				SendBuffer();
				if (streamBuffer->IsEmpty())
					{
					if (IS_ERROR(error = RefillBuffer(TRUE)))
						{
						if (error != GNR_OPERATION_ABORTED)
							{
							player->TerminatePlayback();
							SendPlaybackTerminated(error);
							}

						lock.Leave();

						GNRAISE(error);
						}

					if (streamBuffer->IsEmpty())
						{
						transfer = 0;
						}
					}
				else
					{
					transfer = 0;
					RefillBuffer(TRUE);
					}
				}
			else
				transfer = 0;
			}
		else
			transfer = 0;

		lock.Leave();
		}

	GNRAISE_OK;
	}

//
//  Transfer cell and start at certain time
//

Error DVDVideoStreamServer::TransferCellAt(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DVDTime time)
	{
	DWORD prev, next;
	DVDTime vtime;
	Error err;
	VDAutoMutex	mutex(&lock);

	// solves the problem that some discs (e.g. Shanghai Noon, Blue Streak) hangs when Title Repeat is executed!
	// 02/02/01 (MST)
	FlushBuffers();

	player->SetReversePlayback(FALSE);
	player->SetStillFrameSequenceMode(FALSE);
	playbackDir = EPD_FORWARD;

	//
	// Check if we have space
	//

	if (firstFreeCell->state == VOS_FREE)
		{
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetCellInformation(num, firstFreeCell->info));
		firstFreeCell->pgci = pgci;
		firstFreeCell->num = num;
		firstFreeCell->still = still || firstFreeCell->info.StillTime() ||
		                       firstFreeCell->info.CommandNumber() ||
									  firstFreeCell->info.ContinuousPresentation() ||
									  firstFreeCell->info.LastVOBUStart() == firstFreeCell->info.LastVOBUEnd();

		firstFreeCell->final = final;

		firstFreeCell->scan = CST_PLAYBACK;
		firstFreeCell->breakpoint = ERSBreakpoint();
		firstFreeCell = firstFreeCell->succ;

		// TO BE DONE
		readingCell->state = VOS_READING;

		//
		//  Read first VOBU, or the one from cache
		//

		if (positionCache.valid && num == positionCache.cell && time == positionCache.time &&
			 pgci->GetID() == positionCache.pgciID)
			{
			firstFreeVOBU->Init(file, transferingCell, positionCache.firstBlock, FALSE);
			}
		else
			{
			firstFreeVOBU->Init(file, transferingCell, transferingCell->info.FirstVOBUStart(), FALSE);
			positionCache.valid = FALSE;
			}

		if (IS_ERROR(err = firstFreeVOBU->ReadNavigationPack()))
			{
                if (IS_ERROR(err = firstFreeVOBU->ReadNavigationPack()))
    				{
    				SendEvent(DNE_READ_ERROR, 0);

    				GNRAISE(err);
    				}
			}

		if (!positionCache.valid)
			{
			//
			//  Search for suitable VOBU (with proper presentation time)
			//

			prev = firstFreeVOBU->firstBlock;

			for(;;)
				{
				if (firstFreeVOBU->firstBlock == readingCell->info.LastVOBUStart()) break;
				vtime = firstFreeVOBU->GetTimeOffset();
				if (vtime >= time) break;

				prev = firstFreeVOBU->firstBlock;
				vtime = time - vtime;

				next = firstFreeVOBU->NextLowerTemporalVOBU((WORD)(vtime.Millisecs() / 500));
				if (next != 0x3fffffff)
					firstFreeVOBU->firstBlock = next;
				else
					firstFreeVOBU->firstBlock = firstFreeVOBU->NextVOBU();

				if (next == prev)
					break;

				GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
				}

			if (firstFreeVOBU->GetTimeOffset() > time)
				{
				firstFreeVOBU->firstBlock = prev;
				GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
				}

			//
			//  Search backwards for VOBU with video data
			//

			if (!firstFreeVOBU->HasVideoData())
				{
				firstFreeVOBU->firstBlock = firstFreeVOBU->PrevVOBU();
				if (firstFreeVOBU->firstBlock == 0x3fffffff)
					{
					firstFreeVOBU->firstBlock = transferingCell->info.FirstVOBUStart();
					}
				GNREASSERT_EVENT(firstFreeVOBU->ReadNavigationPack(), DNE_READ_ERROR, 0);
				}
			}

		//
		//  We found a VOBU, so let's save the information
		//

		positionCache.valid = TRUE;
		positionCache.cell = num;
		positionCache.time = time;
		positionCache.pgciID = pgci->GetID();
		positionCache.firstBlock = firstFreeVOBU->firstBlock;
//		DP("ID is %d", positionCache.pgciID);

		firstFreeVOBU = firstFreeVOBU->succ;

//		SendCellStarted(playingCell);

		if (!inTransfer)
			{
			inTransfer = TRUE;
			PerformTransfer(16);
			}

		if (playRequest)
			{
			player->StartPlayback();
			playRequest = FALSE;
			}

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Transfer Cell
//

Error DVDVideoStreamServer::TransferCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu, WORD angle)
	{
	WORD cn;
	Error err;
	VDAutoMutex	mutex(&lock);

	if (!file) GNRAISE(GNR_OBJECT_NOT_FOUND);

	player->SetReversePlayback(FALSE);  // No reverse playback
	playbackDir = EPD_FORWARD;

	if (firstFreeCell->state == VOS_FREE)  // Check if a cell is free
		{
		DP("TransferCell %08lx %d Angle %d", firstFreeCell, num, angle);
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetCellInformation(num, firstFreeCell->info));
		firstFreeCell->pgci = pgci;

		//
		//  Find cell for requested angle
		//

		if (angle)
			{
			if (firstFreeCell->info.BlockType() == CBT_ANGLE_BLOCK)
				{
				while (firstFreeCell->info.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
					{
					num--;
					GNREASSERT(pgci->GetCellInformation(num, firstFreeCell->info));
					}
				cn = 1;
				while (firstFreeCell->info.BlockMode() != CBM_LAST_CELL_IN_BLOCK && cn < angle)
					{
					cn++;
					num++;
					GNREASSERT(pgci->GetCellInformation(num, firstFreeCell->info));
					}
				}
			else
				angle = 0;
			}

		firstFreeCell->num = num;
		firstFreeCell->still = still || firstFreeCell->info.StillTime() ||
		                       firstFreeCell->info.CommandNumber() ||
									  firstFreeCell->info.ContinuousPresentation() ||
									  firstFreeCell->info.LastVOBUStart() == firstFreeCell->info.LastVOBUEnd();

		firstFreeCell->final = final;

		if (firstFreeCell == playingCell)
			{
			player->SetStillFrameSequenceMode(firstFreeCell->info.StillTime() &&
				                               firstFreeCell->info.FirstVOBUStart() == firstFreeCell->info.LastVOBUStart());
			}

		firstFreeCell->scan = CST_PLAYBACK;
		firstFreeCell->breakpoint = ERSBreakpoint();

		firstFreeCell = firstFreeCell->succ;

		//
		//  Read the requested VOBU
		//

		if (vobu)
			{
			readingCell->state = VOS_READING;// | VOS_TRANSFERING | VOS_PLAYING;

			firstFreeVOBU->Init(file, readingCell, vobu, FALSE);
// Does not work, because NVPCK has not been read into the buffer
// yet.  We don't want to read the NVPCK in yet, because this would
// delay navigation.  Time base will be set, as soon as NVPCK is read
// by the pumping thread.
//			player->SetTimeBase(firstFreeVOBU->GetTimeOffset() + firstFreeVOBU->cell->presentationTime);

			//
			//  Perform angle change
			//

			if (angle)
				{
				//
				//  Read Nav Pack
				//

				if (IS_ERROR(err = firstFreeVOBU->ReadNavigationPack()))
					{
					SendEvent(DNE_READ_ERROR, 0);
					GNRAISE(err);
					}

				//
				//  Perform Angle Change
				//

				if (readingCell->info.SeamlessAngleChange())
					{
					//
					//  Perform Seamless Angle Change
					//

					vobu = firstFreeVOBU->SeamlessAngleChangeVOBU(angle);
					if (vobu)
						{
						angleChangeVOBU = vobu;
						angleChangeAngle = angle;
						if (firstFreeVOBU->IsLastVOBUinILVU())
							{
							if (vobu != 0x7fffffff)
								firstFreeVOBU->firstBlock = vobu;
							}
						else
							{
							angleChangeRequest = angle;
							}
						}
					}
				else
					{
					//
					//  Perform Non Seamless Angle Change
					//

					vobu = firstFreeVOBU->NonSeamlessAngleChangeVOBU(angle);
					if (vobu)
						{
						firstFreeVOBU->firstBlock = vobu;
						angleChangeVOBU = vobu;
						angleChangeAngle = angle;
						}
					}
				}

			firstFreeVOBU = firstFreeVOBU->succ;
			}

		DP("Before enabled transfer");
		if (!inTransfer)
			{
			inTransfer = TRUE;
//			PerformTransfer(16);
			}

		DP("Before play starts");
		if (playRequest)
			{
			player->StartPlayback();
			playRequest = FALSE;
			}
		DP("Done starting CELL");

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Transfer cell in scan mode
//

Error DVDVideoStreamServer::TransferCellScan(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, BOOL forward, WORD first, WORD skip, DWORD vobu)
	{
	VDAutoMutex mutex(&lock);
	Error err;

	if (!file) GNRAISE(GNR_OBJECT_NOT_FOUND);

	player->SetStillFrameSequenceMode(FALSE);
	player->SetReversePlayback(FALSE);
	playbackDir = forward ? EPD_FORWARD : EPD_BACKWARD;

	if (firstFreeCell->state == VOS_FREE)
		{
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetCellInformation(cell, firstFreeCell->info));
		firstFreeCell->pgci = pgci;
		firstFreeCell->num = cell;
		firstFreeCell->still = still || firstFreeCell->info.StillTime() ||
									  firstFreeCell->info.ContinuousPresentation() ||
									  firstFreeCell->info.LastVOBUStart() == firstFreeCell->info.LastVOBUEnd();

		firstFreeCell->final = final;

		firstFreeCell->scan = forward ? CST_SCANFORWARD : CST_SCANBACKWARD;
		firstFreeCell->scanStart = first;
		firstFreeCell->scanStep = skip;
		firstFreeCell->breakpoint = ERSBreakpoint();
		firstFreeCell = firstFreeCell->succ;

		if (vobu)
			{
			readingCell->state = VOS_READING;

			firstFreeVOBU->Init(file, readingCell, vobu, TRUE);

			if (IS_ERROR(err = firstFreeVOBU->ReadNavigationPack()))
				{
				SendEvent(DNE_READ_ERROR, 0);
				GNRAISE(err);
				}

			readingCell->firstScanVOBUTime = firstFreeVOBU->GetTimeOffset();
			readingCell->firstScanSystemTime = Timer.GetMilliTicks();

			firstFreeVOBU = firstFreeVOBU->succ;

//			SendCellStarted(playingCell);
			}

		player->StartScan();

		if (!inTransfer)
			{
			inTransfer = TRUE;
			PerformTransfer();
			}

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Transfer cell in reverse playback mode
//

Error DVDVideoStreamServer::TransferCellReverse(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu)
	{
	VDAutoMutex mutex(&lock);

	if (!file) GNRAISE(GNR_OBJECT_NOT_FOUND);

	player->SetStillFrameSequenceMode(FALSE);
	player->SetReversePlayback(TRUE);
	playbackDir = EPD_BACKWARD;

	if (firstFreeCell->state == VOS_FREE)
		{
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetCellInformation(cell, firstFreeCell->info));
		firstFreeCell->pgci = pgci;
		firstFreeCell->num = cell;
		firstFreeCell->still = still;
		firstFreeCell->final = final;
		firstFreeCell->scan = CST_REVERSEPLAYBACK;
		firstFreeCell->breakpoint = ERSBreakpoint();
		firstFreeCell = firstFreeCell->succ;

		if (vobu)
			{
			readingCell->state = VOS_READING;// | VOS_TRANSFERING | VOS_PLAYING;

			firstFreeVOBU->Init(file, readingCell, vobu, FALSE);
			firstFreeVOBU = firstFreeVOBU->succ;

			SendCellStarted(playingCell);
			}

		if (!inTransfer)
			{
			inTransfer = TRUE;
			PerformTransfer(128);
			}

		if (playRequest)
			{
			player->StartPlayback();
			playRequest = FALSE;
			}

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Transfer cell trickplay
//

Error DVDVideoStreamServer::TransferCellTrickplay(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu)
	{
	Error err;
	VDAutoMutex mutex(&lock);

	if (!file) GNRAISE(GNR_OBJECT_NOT_FOUND);

	player->SetStillFrameSequenceMode(FALSE);
	player->SetReversePlayback(FALSE);

	if (firstFreeCell->state == VOS_FREE)
		{
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetCellInformation(cell, firstFreeCell->info));
		firstFreeCell->pgci = pgci;
		firstFreeCell->num = cell;
		firstFreeCell->still = TRUE;
		firstFreeCell->final = final;
		firstFreeCell->scan = CST_TRICKPLAY;
		firstFreeCell->breakpoint = ERSBreakpoint();
		firstFreeCell = firstFreeCell->succ;

		if (vobu)
			{
			readingCell->state = VOS_READING;// | VOS_TRANSFERING | VOS_PLAYING;

			firstFreeVOBU->Init(file, readingCell, vobu, FALSE);

			if (IS_ERROR(err = firstFreeVOBU->ReadNavigationPack()))
				{
				SendEvent(DNE_READ_ERROR, 0);

				GNRAISE(err);
				}

			firstFreeVOBU = firstFreeVOBU->succ;

//			SendCellStarted(playingCell);
			}

		player->StartTrickplay();
		currentVOBUID = player->GetRequestedVOBUID();

		if (!inTransfer)
			{
			inTransfer = TRUE;
			PerformTransfer();
			}

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

//
//  Abort transfer
//

Error DVDVideoStreamServer::AbortTransfer(void)
	{
	suspendTransfer = TRUE;
	streamBuffer->TerminateRequest();
	lock.Enter();
	suspendTransfer = FALSE;

	player->CancelCallback();
//	player->SetAnalogCopyProtectionMode(0);
	player->CancelData();
	FlushBuffers();

	lock.Leave();

	player->WaitForCallbackCanceled();

	GNRAISE_OK;
	}

//
//  Abort transfer accurate
//

Error DVDVideoStreamServer::AbortTransferAccurate(void)
	{
	DWORD signalPosition;

	suspendTransfer = TRUE;
	streamBuffer->TerminateRequest();
	lock.Enter();
	suspendTransfer = FALSE;

	//
	//  Reset Player
	//

	player->CancelCallback();
//	player->SetAnalogCopyProtectionMode(0);
	player->CancelData();

	signalPosition = player->CurrentLocation();

	while (signalPosition >= playingVOBU->startTransferCount &&
		    (playingVOBU->state & VOS_PLAYING) &&
			(!stillMode || !playingVOBU->final ||
			 !playingVOBU->cell->still ||
			 signalPosition == 0xffffffff))
		{
		//
		//  Mark all VOBUs as completed
		//

		playingVOBU->state = VOS_PLAYCOMPLETED;
		playingVOBU->breakpoint = ERSBreakpoint();
		sequencer->VOBUCompleted(playingVOBU);

		//
		//  Complete cell if we reached end of it
		//

		if (playingVOBU->final)
			{
			playingCell->state = VOS_PLAYCOMPLETED;

			sequencer->CellCompleted(playingCell);

			playingCell = playingCell->succ;
			if (playingCell->state & (VOS_TRANSFERING | VOS_TRANSFERCOMPLETE))
				{
				playingCell->state |= VOS_PLAYING;
				sequencer->CellStarted(playingCell);
				}
			}
		//
		//  Go to next VOBU
		//

		playingVOBU = playingVOBU->succ;
		if (playingVOBU->state & (VOS_TRANSFERING | VOS_TRANSFERCOMPLETE))
			{
			playingVOBU->state |= VOS_PLAYING;
			sequencer->VOBUStarted(playingVOBU);

			if (playingCell->IsScanning()) break;
			}
		}

	FlushBuffers();

	lock.Leave();

	player->WaitForCallbackCanceled();

	GNRAISE_OK;
	}

//
//  Stop playback
//

Error DVDVideoStreamServer::StopPlayback(void)
	{
	angleChangeVOBU = 0xffffffff;

	GNRAISE_OK;
	}

//
//  Get angle change type
//

Error DVDVideoStreamServer::GetAngleChangeType(AngleChangeType & type)
	{
	if (playingCell == readingCell)
		{
		if ((playingCell->info.BlockType()) == CBT_NOT_IN_BLOCK && (firstFreeCell->state == VOS_FREE))
			{
//			DPF("Type: Deferred angle change\n");
			type = ACT_DEFERRED;
			}
		else if (playingCell->info.SeamlessAngleChange() && (firstFreeCell->state == VOS_FREE))
			{
//			DPF("Type: Seamless angle change\n");
			type = ACT_SEAMLESS;
			}
		else
			{
//			DPF("Type: Non Seamless angle change\n");
			type = ACT_NON_SEAMLESS;
			}
		}
	else
		{
		if (readingCell->info.BlockType() == CBT_NOT_IN_BLOCK)
			{
//			DPF("Type: Deferred angle change\n");
			type = ACT_DEFERRED;
			}
		else
			{
//			DPF("Type: Non Seamless angle change\n");
			type = ACT_NON_SEAMLESS;
			}
		}

	GNRAISE_OK;
	}

//
//  Seamless angle change
//

Error DVDVideoStreamServer::SeamlessAngleChange(WORD angle)
	{
	VDAutoMutex mutex(&lock);

	DVDVOBU * vobu = readingVOBU->succ;
	DVDCell * cell = readingCell->succ;
	WORD	cn;
	DWORD next;

	angleChangeRequest = 0;

	if (vobu->state == VOS_READY && (readingVOBU->state & VOS_READING))
		{
		//
		//  If successor of readingVOBU is ready to be filled then give it a new angle (cell)
		//

		if (vobu->cell->info.BlockType() == CBT_ANGLE_BLOCK)
			{
			next = readingVOBU->SeamlessAngleChangeVOBU(angle);
			if (next)
				{
				DP("Has seamless angle change");

				angleChangeVOBU = next;
				angleChangeAngle = angle;
				if (readingVOBU->IsLastVOBUinILVU())
					{
					if (next != 0x7fffffff)
						vobu->firstBlock = next;
					}
				else
					angleChangeRequest = angle;

				if (next != 0x7fffffff)
					{
					while (vobu->cell->info.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
						{
						vobu->cell->num--;
						GNREASSERT(vobu->cell->pgci->GetCellInformation(vobu->cell->num, vobu->cell->info));
						}
					cn = 1;
					while (vobu->cell->info.BlockMode() != CBM_LAST_CELL_IN_BLOCK && cn < angle)
						{
						cn++;
						vobu->cell->num++;
						GNREASSERT(vobu->cell->pgci->GetCellInformation(vobu->cell->num, vobu->cell->info));
						}
					}
				}
			}
		}
	else if (vobu->state == VOS_FREE && readingVOBU->state == VOS_READY &&
				(predReadingVOBU->state & VOS_TRANSFERCOMPLETE))
		{
		//
		//  Reading VOBU has not started reading yet, its successor is not allocated then change
		//  readingVOBU's data to point to new angle
		//

		DP("Fallback to previous mode");
		if (readingVOBU->cell->info.BlockType() == CBT_ANGLE_BLOCK)
			{
			next = predReadingVOBU->SeamlessAngleChangeVOBU(angle);
			if (next)
				{
				DP("Has seamless angle change");

				angleChangeVOBU = next;
				angleChangeAngle = angle;
				if (predReadingVOBU->IsLastVOBUinILVU())
					{
					if (next != 0x7fffffff)
						readingVOBU->firstBlock = next;
					}
				else
					{
					angleChangeRequest = angle;
					}

				if (next != 0x7fffffff)
					{
					while (readingVOBU->cell->info.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
						{
						readingVOBU->cell->num--;
						GNREASSERT(readingVOBU->cell->pgci->GetCellInformation(readingVOBU->cell->num, readingVOBU->cell->info));
						}
					cn = 1;
					while (readingVOBU->cell->info.BlockMode() != CBM_LAST_CELL_IN_BLOCK && cn < angle)
						{
						cn++;
						readingVOBU->cell->num++;
						GNREASSERT(readingVOBU->cell->pgci->GetCellInformation(readingVOBU->cell->num, readingVOBU->cell->info));
						}
					}
				}
			}
		}

	//
	//  Check if next cell is ready to be filled. If so then change it to the proper angle
	//

	if (cell->state == VOS_READY && (readingCell->state & VOS_READING))
		{
		DP("Next cell is ready, current is transfering");

		if (cell->info.BlockType() == CBT_ANGLE_BLOCK)
			{
			DP("Current cell is angle block");

			angleChangeVOBU = ANGLE_CHANGE_CELL;
			angleChangeAngle = angle;

			while (cell->info.BlockMode() != CBM_FIRST_CELL_IN_BLOCK)
				{
				cell->num--;
				GNREASSERT(cell->pgci->GetCellInformation(cell->num, cell->info));
				}
			cn = 1;
			while (cell->info.BlockMode() != CBM_LAST_CELL_IN_BLOCK && cn < angle)
				{
				cn++;
				cell->num++;
				GNREASSERT(cell->pgci->GetCellInformation(cell->num, cell->info));
				}
			}
		}

	GNRAISE_OK;
	}

//
//  Set VOBS
//
//  GNR_OK
//

Error DVDVideoStreamServer::SetOBS(DVDOBS * obs)
	{
	this->vobs = (DVDVOBS *)obs;
	GNREASSERT(obs->GetDataFile(file));

	if (!streamBuffer)
		streamBuffer = new DVDScatteredStreamBuffer(streamBufferSize);

	GNRAISE_OK;
	}

//
//  Player Callback
//

void DVDVideoStreamServer::PlayerCallback(DWORD signalPosition)
	{
	BOOL stillCell = FALSE;
	int frameRate = player->GetCurrentFrameRate();
	BOOL bpReached = FALSE;
	ERSBreakpoint bp;
	DVDTime startTime;
	DVDTime endTime;
	VDAutoMutex mutex(&lock);

	//DP("Player callback");

	if (inTransfer)
		{
		if (signalPosition == 0xffffffff && transferingVOBU->state & VOS_TRANSFERING && !playingCell->IsScanning())
			{
			BREAKPOINT;
			}

//		DP("*** Signal at %08lx VOBU %08lx %x %d %d", signalPosition, playingVOBU->endTransferCount, playingVOBU->state, stillMode, playingVOBU->final);

		while (signalPosition >= playingVOBU->endTransferCount && (playingVOBU->state & VOS_PLAYING) &&
				(!stillMode || !playingVOBU->final || !playingVOBU->cell->still || signalPosition == 0xffffffff))
			{
			playingVOBU->state = VOS_PLAYCOMPLETED;

			//
			//  If previous VOBU contained breakpoint go to still (in forward scan)
			//

			if (stopAfterNextVOBU)
				{
				playingVOBU->breakpoint = breakpoint;
				SendVOBUCompleted(playingVOBU);
				stillMode = TRUE;
				stopAfterNextVOBU = FALSE;
				bpReached = TRUE;
				readEveryVOBU = FALSE;
				inTransfer = FALSE;
				}
			else
				{
				//
				//  Test on breakpoint
				//

				if (playingCell->scan != CST_REVERSEPLAYBACK)
					{
					//
					//  Calculate the interval the VOBU covers
					//  NOTE: We don't use DVDVOBU::GetDuration() here since the frame rate might not be correctly
					//  encoded. See: Billy Joel: Greatest Hits Vol. 3
					//

					startTime = playingVOBU->cell->presentationTime + playingVOBU->GetTimeOffset();
					endTime = startTime + DVDTime(playingVOBU->GetEPTM() - playingVOBU->GetSPTM(), 90, player->GetCurrentFrameRate());

					if (bpReached = sequencer->BreakpointReached(startTime, endTime, playbackDir, bp))
						sequencer->SetBreakpointUpcoming(bp.id);

					if (playingVOBU->final && playingVOBU->cell->final)
						{
						if (!bpReached)
							bpReached = sequencer->EndOfCellBreakpointReached(playingVOBU->cell, playbackDir, bp);

						if (!bpReached && playingVOBU->cell->scan == CST_SCANBACKWARD)
							bpReached = sequencer->BeginOfCellBreakpointReached(playingVOBU->cell, playbackDir, bp);
						}

					if (bpReached)
						{
						switch (playingCell->scan)
							{
							case CST_PLAYBACK:
								if (BP_TOSTILL(bp.flags))
									{
									stillMode = TRUE;
									inTransfer = FALSE;
									}
								playingVOBU->breakpoint = bp;
								SendVOBUCompleted(playingVOBU);
								break;
							case CST_SCANFORWARD:
								if (BP_TOSTILL(bp.flags))
									{
#if BREAKPOINT_DEFERRED
									breakpoint = bp;
									stopAfterNextVOBU = TRUE;
									playingVOBU->breakpoint = ERSBreakpoint();
#else
									stillMode = TRUE;
									inTransfer = FALSE;
									playingVOBU->breakpoint = bp;
#endif
									}
								else
									{
									playingVOBU->breakpoint = bp;
									}
								readEveryVOBU = FALSE;
								SendVOBUCompleted(playingVOBU);
								break;
							case CST_SCANBACKWARD:
								if (BP_TOSTILL(bp.flags))
									{
									stillMode = TRUE;
									inTransfer = FALSE;
									}
								readEveryVOBU = FALSE;
								playingVOBU->breakpoint = bp;
								SendVOBUCompleted(playingVOBU);
								break;
							default:
								bpReached = FALSE;
								break;
							}
						}
					else
						{
						playingVOBU->breakpoint = ERSBreakpoint();
						SendVOBUCompleted(playingVOBU);
						}
					}
				else
					{
					SendVOBUCompleted(playingVOBU);
					}
				}

			if (bpReached)
				break;

			//
			//  Complete cell if we're there
			//

			if (playingVOBU->final)
				{
				DP("Final VOBU");
				playingCell->state = VOS_PLAYCOMPLETED;
				stillCell = playingCell->still;

				SendCellCompleted(playingCell);

				playingCell = playingCell->succ;

				if (playingCell->state & (VOS_TRANSFERING | VOS_TRANSFERCOMPLETE))
					{
					playingCell->state |= VOS_PLAYING;
					SendCellStarted(playingCell);
					}
				}

			//
			//  Go to next VOBU
			//

			playingVOBU = playingVOBU->succ;

			if (playingVOBU->state & (VOS_TRANSFERING | VOS_TRANSFERCOMPLETE))
				{
				playingVOBU->state |= VOS_PLAYING;
				SendVOBUStarted(playingVOBU);
				player->SetTimeBase(playingVOBU->GetTimeOffset() + playingVOBU->cell->presentationTime);
				if (playingVOBU->singleFrame)
					player->SetCallback(0x3fffffff, this);
				else
					{
//					DP("*** Set Signal at %08lx %x %d %d", playingVOBU->endTransferCount, playingVOBU->state, stillMode, playingVOBU->final);
					player->SetCallback(playingVOBU->endTransferCount, this);
					}
				}

			//
			//  Check Breakpoints in reverse playback mode
			//

			if (playingCell->scan == CST_REVERSEPLAYBACK && playingVOBU->state != VOS_FREE)
				{
				if (playingVOBU->cell)
					startTime = playingVOBU->cell->presentationTime + playingVOBU->GetTimeOffset();
				else
					startTime = playingVOBU->GetTimeOffset();

				if (bpReached = sequencer->BreakpointReached(startTime, startTime + playingVOBU->GetDuration(), playbackDir, bp))
					sequencer->SetBreakpointUpcoming(bp.id);

				if (playingVOBU->final && playingVOBU->cell && playingVOBU->cell->final)
					{
					if (!bpReached)
						bpReached = sequencer->EndOfCellBreakpointReached(playingVOBU->cell, playbackDir, bp);

					if (!bpReached)
						bpReached = sequencer->BeginOfCellBreakpointReached(playingVOBU->cell, playbackDir, bp);
					}

				if (bpReached)
					{
					bpReached = FALSE;
					if (BP_TOSTILL(bp.flags))
						{
						stillMode = TRUE;
						bpReached = TRUE;
						inTransfer = FALSE;
						}
					playingVOBU->breakpoint = bp;
					SendVOBUCompleted(playingVOBU);
					}
				}
			}

		if (playingVOBU == transferingVOBU->succ)
			{
			BREAKPOINT;
			}

		if (playingCell->IsScanning() && !stillCell && !bpReached)
			{
			stillMode = FALSE;
			PerformTransfer();
			}
		}
	}

//
//  Return VOBU
//

Error DVDVideoStreamServer::ReturnVOBU(DVDVOBU * vobu)
	{
	//
	// There is no lock needed, because the vobu that is freed is outside
	// the active area of the pumping thread
	//
//	lock.Enter();

	vobu->Cleanup();

//	lock.Leave();

	GNRAISE_OK;
	}

//
//  Return cell
//

Error DVDVideoStreamServer::ReturnCell(DVDCell * cell)
	{
	//
	// There is no lock needed, because the cell that is freed is outside
	// the active area of the pumping thread
	//
//	lock.Enter();

	cell->state = VOS_FREE;

//	lock.Leave();

	GNRAISE_OK;
	}

//
//  Complete still
//

Error DVDVideoStreamServer::CompleteStill(void)
	{
	VDAutoMutex mutex(&lock);

	if (stillMode)
		{
		if (playingCell->IsScanning())
			{
			stillMode = FALSE;
			PerformTransfer();
			}
		else
			{
			player->CancelData();
			stillMode = FALSE;
			if (playingCell->state)
				{
				if (playingCell->scan == CST_PLAYBACK)
					PerformTransfer(16);
				else
					player->RequestRefill();

				if (playingCell->state & VOS_PLAYING)
					{
					if (playingCell->scan == CST_PLAYBACK)
						player->StartPlayback();
					playRequest = FALSE;
					}
				else
					playRequest = TRUE;
				}
			else
				playRequest = TRUE;
			}
		}

	GNRAISE_OK;
	}

Error DVDVideoStreamServer::TerminateStill(void)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Defrost
//

Error DVDVideoStreamServer::Defrost(void)
	{
	positionCache.valid = FALSE;
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  DVDAudioStreamServer Class
//
////////////////////////////////////////////////////////////////////

//
// Constructor
//

DVDAudioStreamServer::DVDAudioStreamServer (DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher)
	: DVDStreamServer(seq, server, units, pEventDispatcher)
	, EventSender(pEventDispatcher)
	{
	FlushBuffers();  // Initializes cell buffers

	/*
	transferSemaphore = 0;
	positionCache.valid = FALSE;
	*/
	}

//
// Destructor
//

DVDAudioStreamServer::~DVDAudioStreamServer(void)
	{
	if (streamBuffer)
		{
		delete streamBuffer;
		streamBuffer = NULL;
		}
	}

//
//  Initialize
//

Error DVDAudioStreamServer::Init(DVDFileSystem * dvdfs)
	{
	DVDStreamServer::Init(dvdfs);

	this->dvdfs = dvdfs;
	player->SetRefiller(this);

	GNRAISE_OK;
	}

Error DVDAudioStreamServer::RefillBuffer(BOOL prefetch)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::SendBuffer(void)
	{
	GNRAISE_OK;
	}

//
//  Flush Buffers
//

Error DVDAudioStreamServer::FlushBuffers(void)
	{
	int i;

	for(i=0; i<NUM_CELL_BUFFERS; i++)
		{
		cells[i].state = VOS_FREE;
		}

	firstFreeCell = readingCell = transferingCell = playingCell = cells;

	if (streamBuffer) streamBuffer->Flush();

	/*
	numVOBUBlocks = 0;

	stillMode = FALSE;
	stillPending = FALSE;
	finalBuffer = FALSE;
	playRequest = TRUE;
	inTransfer = FALSE;
	suspendTransfer = FALSE;

	callbackSemaphore	= FALSE;
	angleChangeRequest = 0;
	angleChangeVOBU = ANGLE_CHANGE_NONE;

	readEveryVOBU = FALSE;
	stopAfterNextVOBU = FALSE;

	endPTM = 0;
	ptsOffset = 22500;

	currentReadCount = 0;
	*/

	FlushMessages();

	GNRAISE_OK;
	}

Error DVDAudioStreamServer::SendCellStarted(DVDCell * cell)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::SendCellCompleted(DVDCell * cell)
	{
	GNRAISE_OK;
	}

void DVDAudioStreamServer::ConvertToEStdTime(BYTE * data, int num)
	{
	return;
	}

void DVDAudioStreamServer::Message(WPARAM wParam, LPARAM dParam)
	{
	return;
	}

Error DVDAudioStreamServer::Defrost(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::PerformTransfer(int maxTransfer)
	{
	int transfer;
	Error error;

	transfer = maxTransfer;
	while (transfer && !suspendTransfer)
		{
		lock.Enter();

		transfer--;

		if (inTransfer && !suspendTransfer)
			{
			/*
			if (transferingCell->scan == CST_TRICKPLAY)
				{
				if (stillMode) AdvanceTrickMode();
				}
			*/
			if (player->Completed())
				{
				SendBuffer();
				if (streamBuffer->IsEmpty())
					{
					if (IS_ERROR(error = RefillBuffer(TRUE)))
						{
						if (error != GNR_OPERATION_ABORTED)
							player->TerminatePlayback();

						lock.Leave();

						GNRAISE(error);
						}

					if (streamBuffer->IsEmpty())
						{
						transfer = 0;
						}
					}
				else
					{
					transfer = 0;
					RefillBuffer(TRUE);
					}
				}
			else
				transfer = 0;
			}
		else
			transfer = 0;

		lock.Leave();
		}

	GNRAISE_OK;
	}

//
//  Transfer Cell
//

Error DVDAudioStreamServer::TransferCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu, WORD angle)
	{
	/*
	WORD cn;
	Error err;
	VDAutoMutex	mutex(&lock);
	WORD program;
	WORD lastCell;
	*/

	if (!file) GNRAISE(GNR_OBJECT_NOT_FOUND);

	player->SetReversePlayback(FALSE);  // No reverse playback
	playbackDir = EPD_FORWARD;

	if (firstFreeCell->state == VOS_FREE)  // Check if a cell is free
		{
		firstFreeCell->presentationTime = presentationTime;
		firstFreeCell->state = VOS_READY;
		GNREASSERT(pgci->GetATSCellInformation(num, firstFreeCell->atsInfo));
		firstFreeCell->pgci = pgci;

		firstFreeCell->num = num;

		/*
		firstFreeCell->final = final;
		*/

		firstFreeCell->scan = CST_PLAYBACK;
		firstFreeCell->breakpoint = ERSBreakpoint();

		firstFreeCell = firstFreeCell->succ;

		/*
		//
		//  Read the requested VOBU
		//

		if (vobu)
			{
			readingCell->state = VOS_READING;// | VOS_TRANSFERING | VOS_PLAYING;

			firstFreeVOBU->Init(file, readingCell, vobu, FALSE);
			player->SetTimeBase(firstFreeVOBU->GetTimeOffset() + firstFreeVOBU->cell->presentationTime);

			firstFreeVOBU = firstFreeVOBU->succ;
			}
		*/

		if (!inTransfer)
			{
			inTransfer = TRUE;
//			PerformTransfer(16);
			}

		if (playRequest)
			{
			player->StartPlayback();
			playRequest = FALSE;
			}

		GNRAISE_OK;
		}
	else
		{
		GNRAISE(GNR_OBJECT_FULL);
		}
	}

Error DVDAudioStreamServer::TransferCellAt(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DVDTime time)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::TransferCellScan(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, BOOL forward, WORD first, WORD skip, DWORD vobu)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::TransferCellReverse(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::TransferCellTrickplay(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::AbortTransfer(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::AbortTransferAccurate(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::StopPlayback(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::CompleteStill(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::TerminateStill(void)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::GetAngleChangeType(AngleChangeType & type)
	{
	type = ACT_NONE;
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::SeamlessAngleChange(WORD angle)
	{
	GNRAISE_OK;
	}

//
//  Set AOBS
//
//  GNR_OK
//

Error DVDAudioStreamServer::SetOBS(DVDOBS * obs)
	{
	this->aobs = (DVDAOBS *)obs;
	GNREASSERT(obs->GetDataFile(file));

	if (!streamBuffer)
		streamBuffer = new DVDScatteredStreamBuffer(streamBufferSize);

	GNRAISE_OK;
	}

Error DVDAudioStreamServer::ReturnVOBU(DVDVOBU * vobu)
	{
	GNRAISE_OK;
	}

Error DVDAudioStreamServer::ReturnCell(DVDCell * cell)
	{
	GNRAISE_OK;
	}

void DVDAudioStreamServer::PlayerCallback(DWORD signalPosition)
	{
	return;
	}

Error DVDAudioStreamServer::GetPlaybackTime(DVDTime & time)
	{
	GNRAISE_OK;
	}
