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
//  VCD Stream Server Classes
//
////////////////////////////////////////////////////////////////////

#ifndef VCDSTSRV_H
#define VCDSTSRV_H

#include "VCDPlayer.h"
#include "VCDHeader.h"

//
//  Constants
//

#define NUM_VCD_BUFFERS			27

//
// Messages.
//
#define STILL_TIME_MSG			2001
#define AUTOPAUSE_REACHED_MSG	2002
#define PLAY_COMPLETED_MSG		2003
#define BREAKPOINT_MSG			2004
#define PLAYBACK_TIME_MSG		2005

////////////////////////////////////////////////////////////////////
//
//  VCD Block Integrity Class
//
////////////////////////////////////////////////////////////////////

class VCDBlockIntegrity
	{
	public:
		virtual BOOL CheckBlockIntegrity(BYTE * sector) = 0;
		virtual void AssureBlockPTSIntegrity(BYTE * sector) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  VCD Stream Buffer Class
//
////////////////////////////////////////////////////////////////////

class VCDStreamBuffer
	{
	protected:
		DriveBlock			*	blocks;
		DWORD					*	blockIndices;
		MPEGDataSizePair	*	buffers;
		DWORD						bufferStart, bufferEnd;		// buffer start and buffer end in bytes
		DWORD						bufferSize;						// size of all buffers in bytes
		DWORD						dataBlocksInBuffer;			// number of blocks currently filled with valid data for normal playback
		DWORD						validBlocks;					// number of blocks which have been locked but not yet unlocked
		DWORD						dataBlocksBackScanRead;		// number of blocks which have been examined yet during backward scanning
		DWORD						firstBlock, lastBlock;		// offset to first and last block
		DWORD						numBlocks;						// number of blocks
		DWORD						blockSize;
		BYTE						dummyBlock[CD_FRAME_SIZE];
		VCDDataFile			*	file;

		void	ClearBlocks(void);
		void	UnlockBuffers(VCDDataFile * file, DWORD at, DWORD num);
		Error ReadIntoBuffer(VCDDataFile * file, DWORD block, DWORD at, DWORD num);

	public:
		VCDStreamBuffer(DWORD bufferSize);
		virtual ~VCDStreamBuffer(void);

		BOOL	IsEmpty(void) {return dataBlocksInBuffer == 0;}					// returns TRUE if no data is currently available otherwise FALSE
		BOOL	IsDataAvailable(void) {return dataBlocksInBuffer > 0;}

		BOOL	AllBlocksRead(void) {return dataBlocksBackScanRead > NUM_VCD_BUFFERS;}

		DWORD	AvailBlocks(void) {return dataBlocksInBuffer;}
		DWORD	AvailData(void) {return AvailBlocks() * blockSize;}

		void	Flush(void);

		Error ChangeBufferSize(DWORD size);

		BYTE *GetCurrentBlock(void);
		Error AdvanceFirstBlock(int amount);
		Error BackwardFirstBlock(DWORD amount = 1);
		int	GetFirstBlock(void) {return firstBlock;}
		void	PrepareBackwardScan(void);
		DWORD	FreeBlocks(void) {return numBlocks - dataBlocksBackScanRead;}

		Error	ReadIntoBuffer(VCDDataFile * file, VCDBlockIntegrity * blockIntegrity, DWORD block, DWORD numBlocksToRead, DWORD &numBlocksDone, BOOL isScanning); // num in blocks
		DWORD	WriteFromBuffer(VCDPlayer * player, DWORD offset, DWORD num); // num in bytes
	};

////////////////////////////////////////////////////////////////////
//
//   VCD Stream Sequencer Class
//
////////////////////////////////////////////////////////////////////

class VCDStreamSequencer : public VCDStreamServer, protected VCDBlockIntegrity, public WinPort
	{
	protected:
		VCDDataFile	*	file;
		VCDPlayer	*	player;
		VDLocalMutex	lock;
		RequestHandle	rh;

		VCDStreamBuffer * streamBuffer;
		DWORD					streamBufferSize;

		DWORD	currentSector;			// current sector that is played
		DWORD scanSector, averageScanDistance;
		DWORD numSectors;				// Number of sectors left in current play item
		BOOL	finalStream;
		BOOL	playRequest;
		BOOL	stillMode;
		BOOL	autoPause;
		BOOL	scanFrameComplete;
		BOOL	scanFramePending;
		BOOL	hiresStill;
		BOOL	isSegment;
		BOOL	followsSVCDStd;

		//
		// a MPEG-2 pack header contains 2 bytes more than a MPEG-1 pack header.
		//
		WORD packHeaderOffset;
		DWORD currentTimeInMilliSecs;

		BOOL	transfering;
		int	transferSemaphore;
		int	scanSpeed;
		DWORD	scanHeaderBuffer;
		int	scanHeaderCount;
		int	startSector;
		DWORD	previousVideoPTS;
		BOOL	suspendTransfer;

		// Fast search algorithm for sequence header.
		int searchSteps[256];

		int firstByte, lastByte;			// Offset to firstBuffer, pointing to first/last byte within this sector

		BOOL	SmallBufferEmpty(void) {return firstByte == lastByte;}		// Block within buffer
		BOOL	FinalSector(void);

		BOOL	IsSequenceHeaderInSector(BYTE * sector);
		BOOL	CheckBlockIntegrity(BYTE * sector);
		void	AssureBlockPTSIntegrity(BYTE * sector);
		DWORD ExtractPTS(BYTE * sector);
		BOOL	HasValidPTS(BYTE * sector);

		DWORD FindTimeSearchSector(VCDDataFile * file, DWORD time);
		Error FindNextScanSector(void);
		void	KillLastFrame(void);

		Error RefillBuffer(void);
		Error SendSmallBuffer(void);
		void	AdvanceFirstBlock(int amount = 1);
		void	ReadFirstBufferInfo(BYTE * sector);

		Error FlushBuffers(void);
		Error ContinueAutoPause(void);

		virtual void TransferCompleted(void) {}
		virtual void PlaybackCompleted(void) {}
		virtual void AutoPauseReached(void) {ContinueAutoPause();}

		// WinPort function.
		virtual void Message (WPARAM wParam, LPARAM lParam);

	public:
		VCDStreamSequencer (WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~VCDStreamSequencer (void);

		Error Init(void);
		Error SetAttributes(BOOL svcdStdDisc);
		Error TransferStream(VCDDataFile * file, DWORD firstSector, DWORD numSectors, BOOL final, int scanSpeed = 0, BOOL hiresStill = FALSE, BOOL toPause = FALSE, BOOL isSegment = FALSE);
		Error AbortTransfer(void);
		Error CompleteStream(void);
		virtual Error Pause(void) = 0;

		// VCDStreamServer functions. Note these functions are called from another thread.
		Error	PerformTransfer(int maxTransfer);
		void	PlayerCallback(DWORD signalPosition);

		DWORD	CurrentLocation(void);
		virtual Error CurrentDuration(DWORD & dur);

		virtual WORD CurrentPlayItem(void) = 0;

		//
		// virtual function. Should be overloaded by VCDDiskPlayerClass::GetDiskType()
		//

		virtual Error GetDiskType(DVDDiskType & type) = 0;

		Error GetPlaybackTime(DVDTime & time);
	};

#endif
