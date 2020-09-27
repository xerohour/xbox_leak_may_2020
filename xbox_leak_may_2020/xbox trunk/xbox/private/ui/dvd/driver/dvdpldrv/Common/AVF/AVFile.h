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

// FILE:       Driver/Dvdpldrv/Common/AVF/AVFile.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    8-NOV-2000
//
// PURPOSE:    Generic file reader for audio and video files.
//
// HISTORY:

#ifndef AVFILE_H
#define AVFILE_H

#include "Library/Common/Prelude.h"
#include "Driver/Dvdpldrv/Common/DVDTime.h"
#include "Library/Files/GenericFile.h"
#include "Driver/Dvdpldrv/Common/AVF/AVFFrame.h"
#include "Driver/Dvdpldrv/Common/AVF/AVFFilter.h"

////////////////////////////////////////////////////////////////////
//
//  Drive Block performance Analysis
//
////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include "Library/Lowlevel/Timer.h"
// #define _DEBUG_DRIVE_BLOCK_PERFORMANCE_TEST
#ifdef _DEBUG_DRIVE_BLOCK_PERFORMANCE_TEST
#define AVF_BLOCK_PERFORMANCE_TEST_CACHE_DEPTH		300
#define AVF_BLOCK_PERFORMANCE_TEST_STRLEN				31
#define AVF_BLOCK_PERFORMANCE_STRING_LOCATIONS		16
typedef struct
	{
	DWORD		startTime;
	DWORD		readTime;
	DWORD		blockNum;
	Error		err;
	int		loc;
	}	AVFDebugDriveBlockPerformanceInfo;

class AVFDebugDriveBlockPerformanceTestClass
	{
	public:
		AVFDebugDriveBlockPerformanceTestClass(void);
		void Invalidate(void);
		char	str[AVF_BLOCK_PERFORMANCE_STRING_LOCATIONS][AVF_BLOCK_PERFORMANCE_TEST_STRLEN+1];
		void	AddLocationString(int num, char *strParam);
		int	count;
		AVFDebugDriveBlockPerformanceInfo	info[AVF_BLOCK_PERFORMANCE_TEST_CACHE_DEPTH];
		void OutputSummary(void);
		void StartRead(DWORD blockNum);
		void EndRead(Error errParam, int locParam);
	};

#endif
#endif

////////////////////////////////////////////////////////////////////
//
//	 defines
//
////////////////////////////////////////////////////////////////////

#define AVF_DUMMY_FRAME_RATE	25			// frame rate used in DVDTime construction

////////////////////////////////////////////////////////////////////
//
//  Type declarations
//
////////////////////////////////////////////////////////////////////

enum DriveBlockCleanUpMode
	{
	DBPM_CLEANUP_ALL_BLOCKS,
	DBPM_CLEANUP_ALL_EXCEPT_LAST_BLOCK,
	DBPM_CLEANUP_ALL_UNUSED_BLOCKS,
	DBPM_CLEANUP_ALL_UNUSED_BLOCKS_EXCEPT_THE_LAST_ONE
	};

////////////////////////////////////////////////////////////////////
//
//  AVFileManager Class
//
////////////////////////////////////////////////////////////////////


class AVFileManager
	{
	protected:
		AVFFilter		*pFilter;	// the content-specific file parser
		AVFFilterType	filterType;	// file type
		int				maxLockedFrames;				// maximum number of frames in frameList
		int				maxFrameSize;					// maximum size of a single frame in bytes
		int				frameDetailsElementSize;	// content-specific per-frame data size
		BOOL				streamingIsEnabled;			// affects the sizes of the above values
		BOOL				endOfFileReached;				// set to TRUE in ReadNextFrame()

		// Larger data structures, created and deleted in CreateNewFilter,
		// also deleted in destructor
		BOOL								dataStructuresInitialized;
		DriveBlockPool					blockList;
		AVFFramePool					frameList;
		AVFFrameLocatorPool			frameLocatorList;
		DriveBlockCheckPointList	checkPointList;

		// Block-level data
		DriveBlockNode					*pCurrentBlockNode;
		DWORD								lastBlockRead;
		DWORD								lastBlockCheckPointed;
		int								numberOfCheckPoints;
		DWORD								nextCheckPointBlock;
		int								nextCheckPointNumber;
		DWORD								numberOfBytesRead;
		DriveBlockNode					dummyScanBlock;
		DWORD								diskAccessFlags;
		DWORD								dummyBlockNumber;
		DriveBlockNode					*pRealCurrentBlockNode;

		// Frame-level data
		AVFFrame							dummyScanFrame;

		// File and Filesystem data
		GenericFileSystem				*avffs;
		GenericFileSystemIterator	*gfsi;
		GenericDiskItem				*gdi;
		GenericFile						*gf;
		DiskItemType					itemType;
		KernelInt64						itemSize;
		Error								err;
		RequestHandle					rh;

		//
		//  Internal functions
		//

		Error MoveToCheckPoint(DriveBlockCheckPointNode *pCheckPoint);

		//
		//  Playback functions
		//

		Error	OpenDataSource(GenericFileSystemIterator * gfsi, int numberOfCheckPoints = 0,
									BOOL sameFileAsLastTime = FALSE);

		Error CreateNewFilter(AVFFilterType type);

		void	SetEndOfFile(void) { endOfFileReached = TRUE; }
		void	ClearEndOfFile(void) { endOfFileReached = FALSE; }

	public:
		AVFileSizeAttributes		fileSizeInfo;
#ifdef _DEBUG
		void EnableLogging(BOOL b) { if (pFilter) pFilter->enableLogging = b; }
#ifdef _DEBUG_DRIVE_BLOCK_PERFORMANCE_TEST
		AVFDebugDriveBlockPerformanceTestClass	debugDriveTest;
#endif
#endif
		DWORD				driveBlockSize;
		DWORD				headerSize;
		DWORD				dataSize;
		DWORD				dataEnd;

		BOOL				lastBlockInFile;

		AVFileManager(void);
		~AVFileManager(void);

		virtual Error Init(GenericFileSystem * avffs, BOOL enableStreaming = TRUE);

		// DetermineFilterType() can be used to determine if a file is playable or not.
		// It is used internally for finding which format-specific filter to
		// instantiate.
		Error DetermineFilterType(GenericFileSystemIterator * gfsi, AVFFilterType &type);
		AVFFilterType GetFilterType(void) { return filterType; }

		// ReadNextFrame(pFrame); does the actual file reading to pull frames from
		// the file one at a time in sequence.  The pFrame it returns can be used or
		// ignored, since it is managed by the frameList.  If you want to use the
		// PFrame right away, you may call ReleaseFrames(pFrame) immediately when
		// you are through without calling GetNextFrame() or ReadNextFrame() in the
		// interim.
		Error ReadNextFrame(AVFFrame * & pFrame, BOOL useDummyStructures = FALSE);

		// UnGetLastFrame(pFrame) is used when traversing a file at speeds other than
		// normal play speed.  It is similar to ReleaseFrames(pFrame,1), except it
		// frees from the end of the list, rather than from the beginning.
		Error UnGetLastFrames(AVFFrame *pFrame, int count = 1);

		// ReadFrameByNumber() allows random access through a file. taking advantage of
		// the checkPoint mechanism.
		Error ReadFrameByNumber(DWORD requestedFrameNum, AVFFrame * & pFrame);

		// ReadFrameByTimestamp() allows random access through a file. taking advantage of
		// the checkPoint mechanism.  This function will read the first frame whose timestamp
		// is >= the given timeStamp (in milliseconds), subsequent calls to ReadNextFrame()
		// will pick up from this location.
		Error ReadFrameByTimestamp(DWORD milliSec, AVFFrame * & pFrame);

		// GetLockedFrameCount() returns the number of queued frames in the frameList.
		int	GetLockedFrameCount(void) { return(frameList.GetLockedFrameCount()); }

		//	GetNextFrame returns the nth frame from the beginning of the queue.
		// it's called with offset = 0 twice in a row without calling ReleaseFrames(),
		// it will return the same frame pointer.  So typically, you should alternately
		// call pFrame = GetNextFrame(); and ReleaseFrames(pFrame).  Use the defaults
		// of offset=0 and count=1 for simplicity.
		AVFFrame	*GetNextFrame(int offset=0) { return(frameList.GetLockedFrame(offset)); }

		// If you forget to call ReleaseFrames, the frameList buffer will fill up and
		// every call to ReadNextFrame() will fail.
		Error	ReleaseFrames(AVFFrame *pFrame, int count = 1);

		DWORD	LastFrameNumberRead(void) { return(frameList.LastFrameNumberRead()); }
		Error Cleanup(void);

		//
		//  Data source handling
		//

		Error OpenFile(GenericFileSystemIterator * gfsi,
										AVFFilterType &type,
										AVFInfo *pInfo = (AVFInfo *) NULL,
										BOOL sameFileAsLastTime = FALSE);
		Error CloseFile(void);

		// General file attributes added during rewrite:
		BOOL IsFilterValid(void);
		DWORD GetBytesPerSecond(void);
		DWORD GetAvgFileBitrate(void);
		BOOL FileIsOpen(void);
		BOOL IsForwardScanEnabled(void);
		BOOL IsReverseScanEnabled(void);
		BOOL NeedToDampVolumeDuringScan(DWORD thresholdBitrate);
		BOOL IsBeginningOfFile(void);
		BOOL	IsEndOfFile(void);
		DVDAudioCodingMode GetCodingMode(void);
		DWORD GetSampleRate(void);
		WORD GetNumberOfAudioChannels(void);

		// frame locator stuff added during rewrite:
		void SetBufferedByteCount(DWORD num);
		DWORD GetCurrentLocatorSeconds(void);
		void SetCurrentLocatorSeconds(DWORD sec);
		AVFFrameLocator * GetFreeFrameLocators(int num);
		void ReleaseFrameLocator(AVFFrameLocator *pLoc);
		AVFFrameLocator * GetFrameLocator(int num);

		// frameList and parsing stuff added during rewrite:
		int GetAvailableFrameCount(void);
		void InitFirstFrameInFile(void);
		void ReverseLastSeveralFrames(int numFrames);
		void InvalidateFrameBlockAndLocatorLists(void);
		void PreserveParseState(void);

		BOOL IsMatchingFile(GenericFileSystemIterator *gfsiParam);
		BOOL IsStreamingEnabled(void) { return streamingIsEnabled; }
		BOOL IsDurationAvailable(void)
			{
			if (pFilter && pFilter->IsDurationAvailable())
				return TRUE;
			else
				return FALSE;
			}

		DWORD GetValidInfoFlags(void)
			{
			if (pFilter)
				return pFilter->fileInfo.valid;
			else
				return 0;
			}

		void	CleanUpBlockList(DriveBlockCleanUpMode cleanupMode);

		GenericFileSystemIterator *GetCurrentIterator(void) { return gfsi; }

		Error GetCurrentDuration(DVDTime &duration);
		Error GetFileInfo(AVFInfo * appFileInfo);
		DWORD GetFileLengthInSeconds(void)
					{ if (pFilter) return pFilter->GetLengthInSeconds();
					else return 0; }
		int GetMaxFramesToLockEachRefill(void)
					{ if (pFilter) return pFilter->GetMaxFramesToLockEachRefill();
					else return 0; }
		int GetNumberOfRefillsPerTransfer(void)
					{if (pFilter) return pFilter->GetNumberOfRefillsPerTransfer();
					else return 0; }

		DWORD	GetDiskAccessFlags(void) { return diskAccessFlags; }
		virtual void SetDiskAccessSequential(void)
			{ diskAccessFlags = LOCKBLOCKS_SEQUENTIAL_ACCESS_FLAGS;
			if (pFilter) pFilter->SetDiskAccessSequential(); }
		virtual void SetDiskAccessRandom(void)
			{ diskAccessFlags = LOCKBLOCKS_RANDOM_ACCESS_FLAGS;
			if (pFilter) pFilter->SetDiskAccessRandom(); }
		int GetMaxLockedFrames(void) { if (pFilter) return pFilter->GetMaxLockedFrames();
												 else return 0; }

#ifdef _DEBUG
		void DebugDumpBlockList(void);
		void DumpLockedFrameList(void) { frameList.DumpLockedFrameList(); }
		void PrintDump(unsigned char *bp, int len, int maxlen=0, BOOL dumpASCII=TRUE)
			{
			if (pFilter)
				pFilter->PrintDump(bp, len, maxlen, dumpASCII);
			}
#endif
	};

#endif

