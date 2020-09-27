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

// FILE:       Driver/Dvdpldrv/Common/AVF/AVFFrame.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    3-SEP-2000
//
// PURPOSE:    Generic frame structures for audio and video files.
//
// HISTORY:

#ifndef AVFFRAME_H
#define AVFFRAME_H

#include "Library/Common/Prelude.h"
#include "Library/Files/GenericFile.h"

#define ONLY_EXTERNAL_VISIBLE		// Makes include of mp2eldec.h work
#include "Library/hardware/mpeg2dec/generic/mp2eldec.h"

////////////////////////////////////////////////////////////////////
//
//  Drive Block Node Class
//
////////////////////////////////////////////////////////////////////

class DriveBlockNode
	{
	private:
		int						userCount;
		BOOL						locked;
		GenericFile				*gf;
		RequestHandle			*pRequestHandle;

	public:
		DriveBlock				block;
		HBPTR						pData;	// &block.data + headerSize
		DWORD						relativeBlockNum;

		DriveBlockNode(void);
		~DriveBlockNode(void);

		Error LockBlock(GenericFile *pGF, DWORD relativeBlock, DWORD flags, RequestHandle *pRH, DWORD headerSize);
		Error UnlockBlock(void);

		int	GetUserCount(void) { return(userCount); }
		DWORD	GetRelativeBlockNum(void) { return relativeBlockNum; };

		void	AddUser(void) { userCount++; }
		BOOL	IsLocked(void) { return locked; }
		Error SubtractUser(void)
			{
			if (--userCount < 0)
				{
				userCount++;
				GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
				}
			else
				GNRAISE_OK;
			}

		void Invalidate(void);
	};

////////////////////////////////////////////////////////////////////
//
//  Drive Block Pool Class
//
////////////////////////////////////////////////////////////////////

class DriveBlockPool
	{
	private:
		int		usedIndex;
		int		freeIndex;
		int		usedCount;
	public:
		DriveBlockNode		*blockArray;
		int					arraySize;
		int					maxBlocksPerFrame;
		int					lastRequestIndex;
		int					lastRequestCount;

		DriveBlockPool(void);

		~DriveBlockPool(void);

		void Cleanup(void);

		BOOL ResizeNeeded(int	maxLockedFrames, int maxFrameSize, int dataSize);

		Error Initialize(int	maxLockedFrames, int maxFrameSize, int dataSize);

		int GetUsedCount(void) { return usedCount; }

#ifdef _DEBUG
		int GetUsedIndex(void) { return usedIndex; }
		int GetFreeIndex(void) { return freeIndex; }
		DriveBlockNode		*DebugGetBlockAbsolute(int offset);
#endif
		DriveBlockNode		*GetUsedBlock(int offset);
		DriveBlockNode		*GetUsedBlockInReverse(int offset);

		DriveBlockNode	*GetContiguousFreeBlocks(int count=1);

		Error	UnGetContiguousBlocks(DriveBlockNode *ptr, int count=1);

		Error	ReleaseContiguousBlocks(DriveBlockNode *ptr, int count=1);

		void Invalidate(void); // must release in the proper order
	};


////////////////////////////////////////////////////////////////////
//
//  AVF Frame Segment Node Class
//
////////////////////////////////////////////////////////////////////

class AVFFrameSegmentNode
	{
	public:
		MPEGDataSizePair 		data;
		DriveBlockNode			*pBlockNode;

		AVFFrameSegmentNode(void);
		~AVFFrameSegmentNode(void);

		Error Initialize(DriveBlockNode *pBlockNode, int timestamp);
		void SetFrameSegmentOffsetAndSize(DWORD _offset, DWORD _size);
		void AdjustSegmentSize(DWORD _size);
		void Invalidate(void);
	};

////////////////////////////////////////////////////////////////////
//
//  AVF Frame Segment Pool Class
//
////////////////////////////////////////////////////////////////////

class AVFFrameSegmentPool
	{
	public:
		AVFFrameSegmentNode	*segmentArray;
		int		arraySize;
		int		usedIndex;
		int		usedCount;
		int		freeIndex;
		int		maxSegmentsPerFrame;
		int		lastRequestIndex;
		int		lastRequestCount;

		AVFFrameSegmentPool(void);
		~AVFFrameSegmentPool(void);
		void Cleanup(void);

		BOOL		ResizeNeeded(int maxLockedFrames, int maxFrameSize, int dataSize);
		Error		Initialize(int	maxLockedFrames, int maxFrameSize, int dataSize);
	};

////////////////////////////////////////////////////////////////////
//
//  AVFFrame Classes
//
////////////////////////////////////////////////////////////////////

class AVFFrame
	{
	private:
		int								arraySize;
		int								actualSegments;
		DWORD								frameNum;
		BOOL								isDummy;
		DWORD								startTimestamp;
		DWORD								startBlock;
		DWORD								blockOffset;
	public:
		AVFFrameSegmentNode			*segmentArray;
		DWORD								bytesPerSecond; // this might change for each frame
		// DWORD							byteOffsetInFile;
		DWORD								frameByteOffsetInFile;
		HBPTR								pDetails;	// attach subclass data here for now

		AVFFrame();
		~AVFFrame();

		Error Initialize(AVFFrameSegmentNode *pSegmentArray, int segmentCount);

		Error AddNewSegment(DriveBlockNode *pBlockNode, int timestamp, DWORD offset, DWORD size);

		AVFFrameSegmentNode	*GetNextFreeSegment(void);

		MPEGDataSizePair	*GetSegmentDataSizePair(int segment);

		DWORD	GetStartTimestamp(void) { return startTimestamp; }
		void	SetStartTimestamp(DWORD timestamp) { startTimestamp = timestamp; }
		int	GetActualSegmentCount(void) { return actualSegments; }
		void	ResetActualSegmentCount(void) { actualSegments = 0; }	// Used only with dummyScanFrame!
		int	GetFrameSize(void);
		int	GetFrameNum(void) { return frameNum; }
		DWORD GetBytesPerSecond(void) { return bytesPerSecond; }
		BOOL	IsDummy(void) { return isDummy; }
		void	SetStartBlockAndOffset(DWORD blockNum, DWORD offset)	// used for dummy frames in DetermineDuration()
			{
			startBlock = blockNum;
			blockOffset = offset;
			}
		Error GetStartBlock(DWORD &block);
		Error GetStartOffset(DWORD &offset);
		void	SetFrameNum(DWORD frameNum);
		void	Invalidate(void);
#ifdef _DEBUG
		BOOL DebugIntegrityCheck(BYTE firstByte);
#endif
	};

////////////////////////////////////////////////////////////////////
//
//  AVF Frame Pool Class
//
////////////////////////////////////////////////////////////////////

class AVFFramePool
	{
	private:
		int		usedIndex;
		int		freeIndex;
		int		usedCount;
		int		lastRequestIndex;
		int		lastRequestCount;
	protected:
		AVFFrameSegmentPool	segmentList;
	public:
		AVFFrame	*frameArray;
		int		arraySize;
		int		maxSegmentsPerFrame;

		AVFFramePool(void);

		~AVFFramePool(void);

		void Cleanup(void);

		BOOL	ResizeNeeded(int maxLockedFrames, int maxFrameSize, int dataSize);
		Error	Initialize(int	maxLockedFrames, int maxFrameSize, int dataSize);

		Error AttachSubClassFrameData(HBPTR ptr, int elementSize);
		Error InvalidateSubClassFrameData(void);

		int	GetLockedFrameCount(void);
		int	GetAvailableFrameCount(void);

		Error ReverseLastSeveralFrames(int n);	// this is used during reverse playback

		AVFFrame	*GetLockedFrame(int offset);
		// AVFFrame	*GetLockedFrameInReverse(int offset);

		AVFFrame	*GetContiguousFreeFrames(int count=1); // count should remain 1

		Error	ReleaseContiguousFrames(AVFFrame *ptr, int count=1);

		Error	UnGetContiguousFrames(AVFFrame *ptr, int count=1);

		DWORD LastFrameNumberRead(void);

		void Invalidate(void);
#ifdef _DEBUG
		void DumpLockedFrameList(void);
#endif
	};

////////////////////////////////////////////////////////////////////
//
//  AVFFrameLocator Classes
//
////////////////////////////////////////////////////////////////////

class AVFFrameLocator
	{
	private:
		int								frameNum;
		DWORD								size;
	public:
		DWORD								bufferedByteCount;
		DWORD								milliSec;	// from beginning of file
		DWORD								seconds;
		DWORD								bytesPerSecond;
		DWORD								nextFrameBeginningOffset;
		DWORD								bytesFromBeginning;

		AVFFrameLocator();
		~AVFFrameLocator();

		Error Initialize(AVFFrame *pFrame, DWORD sentBytes, DWORD frameBeginSeconds);

		DWORD	GetFrameSize(void) { return size; }
		int	GetFrameNum(void) { return frameNum; }
	};

////////////////////////////////////////////////////////////////////
//
//  AVF Frame Locator Pool Class
//
////////////////////////////////////////////////////////////////////

class AVFFrameLocatorPool
	{
	private:
		int		usedIndex;
		int		freeIndex;
		int		usedCount;
		int		lastRequestIndex;
		int		lastRequestCount;
		DWORD		totalBufferedBytes;
		DWORD		currentSeconds;
	public:
		AVFFrameLocator	*frameLocatorArray;
		int					arraySize;

		AVFFrameLocatorPool(void);

		~AVFFrameLocatorPool(void);

		void Cleanup(void);

		BOOL	ResizeNeeded(int	maxLockedFrames);
		Error	Initialize(int	maxLockedFrames);

		int	GetFrameLocatorCount(void);

		void	SetBufferedByteCount(DWORD count) { totalBufferedBytes = count; }
		DWORD	GetBufferedByteCount(void) { return totalBufferedBytes; }
		void	AddToBufferedByteCount(DWORD count) { totalBufferedBytes += count; }

		void	SetCurrentSeconds(DWORD sec) { currentSeconds = sec; }
		DWORD	GetCurrentSeconds(void) { return currentSeconds; }

		AVFFrameLocator	*GetFrameLocator(int offset);

		AVFFrameLocator	*GetFreeFrameLocators(int count=1); // count should remain 1

		Error	ReleaseFrameLocators(AVFFrameLocator *ptr, int count=1);

		Error	UnGetFrameLocators(AVFFrameLocator *ptr, int count=1);

		void Invalidate(void);
	};

////////////////////////////////////////////////////////////////////
//
//  Drive Block Check Point Node Class
//
////////////////////////////////////////////////////////////////////

class DriveBlockCheckPointNode
	{
	public:
		// DWORD		checkPointNum;
		DWORD		frameNum;
		DWORD		blockNum;
		DWORD		offset;
		DWORD		timestamp;
		DWORD		frameByteOffset;
	};

////////////////////////////////////////////////////////////////////
//
//  Drive Block Check Point List Class
//
////////////////////////////////////////////////////////////////////

class DriveBlockCheckPointList
	{
	public:
		DriveBlockCheckPointNode	*checkPointArray;
		int								arraySize;
		int								actualCheckPoints;
		BOOL								enabled;
		DWORD								blocksPerCheckPoint;
		DriveBlockCheckPointList(void);
		~DriveBlockCheckPointList(void);

		Error Initialize(int	checkPointsPerFile);
		BOOL	ResizeNeeded(int checkPoints);

		void	EnableCheckPointing(void) { enabled = TRUE; }
		void	DisableCheckPointing(void) { enabled = FALSE; actualCheckPoints = 0; }
		BOOL	CheckPointingIsEnabled(void) { return(enabled); }

		void	AddCheckPoint(int index, DWORD frameNum, DWORD blockNum,
								  DWORD offset, DWORD timestamp, DWORD fileOffset);
		// void	InsertAt(int index, DWORD blockNum, DWORD frameNum, DWORD offset, DWORD timestamp, DWORD fileOffset);

		DriveBlockCheckPointNode *GetFrameCheckPoint(DWORD frameNum);
		DriveBlockCheckPointNode *GetFrameCheckPointTimestamp(DWORD milliSec);
		DWORD GetLastCheckPointTimeStamp(void);
		DWORD GetLastCheckPointFrameNum(void);
		void	Invalidate(void);
		void	Cleanup(void);
	};

#endif

