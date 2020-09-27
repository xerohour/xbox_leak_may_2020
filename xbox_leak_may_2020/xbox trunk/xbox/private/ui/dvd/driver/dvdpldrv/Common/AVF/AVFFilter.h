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

// FILE:       Driver/Dvdpldrv/Common/AVF/AVFFilter.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    23-OCT-2000
//
// PURPOSE:    Generic file parser for audio and video files
//
// HISTORY:

#ifndef AVFFILTER_H
#define AVFFILTER_H

#include "Library/Common/Prelude.h"
#include "Library/Files/GenericFile.h"
#include "Driver/Dvdpldrv/Common/DVDTime.h"
#include "Driver/Dvdpldrv/Common/AVF/AVFFrame.h"

#ifdef _DEBUG
#include "Library/Support/Shell/ShellToolKit.h"
#endif

#define LOCKBLOCKS_SEQUENTIAL_ACCESS_FLAGS	(DAF_STREAMING | DAT_LOCK_AND_READ)
#define LOCKBLOCKS_RANDOM_ACCESS_FLAGS	(DAF_STREAMING | DAT_LOCK_AND_READ)
// #define LOCKBLOCKS_SEQUENTIAL_ACCESS_FLAGS	(DAT_LOCK_AND_READ)
// #define LOCKBLOCKS_RANDOM_ACCESS_FLAGS			(DAT_LOCK_AND_READ)

#define AVF_MAX_BLOCKS_WITH_NO_HEADER	MPEG_AUDIO_MAX_BLOCKS_WITH_NO_HEADER
	// this does not include any headers

enum AVFileParseState
	{
	AVFPARSE_BEGINNING_OF_FILE,
	AVFPARSE_BEGINNING_FRAME_SEARCH,
	AVFPARSE_IN_FRAME,
	AVFPARSE_AFTER_FRAME
	};

//
// class AVFParseState
//

class AVFParseState
	{
	public:
		AVFileParseState		genericState;
		DWORD						currentBlockUnparsedOffset;
		DWORD						currentFrameOffset;
		BOOL						needToCleanUpBlocks;
		DWORD						currentFrameNum;
		DWORD						relativeBlockNum;
		DWORD						timestamp; // in milliseconds
		DWORD						frameByteCountInFile;
		DWORD						bytesToSkip;

		AVFParseState(void);
		~AVFParseState(void);
	};

enum AVFFilterType
	{
	AVF_FILTER_TYPE_NONE,
	AVF_FILTER_TYPE_UNSUPPORTED,
	AVF_FILTER_TYPE_RAW_BINARY,
	AVF_FILTER_TYPE_MPEG_AUDIO,
	AVF_FILTER_TYPE_TEXT
	};

typedef struct
	{
 	KernelInt64		fileSize;
	DWORD				totalSkippedBytes;
	DWORD				blocksInThisFile;
	DWORD				numLastBlockInFile;
	DWORD				numBytesInLastBlock;
	DWORD				headerSize;
	DWORD				dataSize;
	} AVFileSizeAttributes;

class AVFFilter
	{
	protected:
		DWORD							bytesPerSecond;	// if not variable bitrate
		BOOL							durationIsAvailable;
		BOOL							forwardScanEnabled;
		BOOL							reverseScanEnabled;
		DWORD							totalSeconds;	// aka duration
		DWORD							diskAccessFlags;

	public:
#ifdef _DEBUG
		BOOL							enableLogging;
		void PrintDump(unsigned char *bp, int len, int maxlen=0, BOOL dumpASCII=TRUE);
		ShellToolKit				*pShell;
		void SetShell(ShellToolKit *shell) { pShell = shell; }
#endif
		AVFInfo						fileInfo;	// defined in DVDTime.h

		AVFParseState				genericParseState;
		AVFParseState				preservedParseState;
		GenericFile					*gf;
		AVFileSizeAttributes		sizeInfo;
		DriveBlockNode				randomAccessBlock; // must be used and unlocked within one function call
																 // this is intended for ID3v1 and other nonAudio tags.
		RequestHandle				rh;


		AVFFilter(void);
		virtual ~AVFFilter(void);
		virtual void Cleanup(void);
		virtual void Invalidate(void);
		virtual void InvalidateButPreserveInfo(void);

		virtual Error FindNextFrame(DriveBlockNode *pBlockNode, AVFFrame *pFrame, BOOL lastBlockInFile) = 0;
		virtual Error OpenFileInitialization(GenericFile * gf,
				AVFileSizeAttributes *pSizeInfo, AVFInfo *pInfo = (AVFInfo *) NULL, BOOL streamingEnabled=FALSE);
		BOOL	IsDurationAvailable(void) { return durationIsAvailable; }
		virtual Error DetermineDuration(void) = 0;

		virtual void InitFirstFrameInFile(void);
		virtual void InitNewFrameSameBlock(void);
		virtual void InitSameFrameNewBlock(void);
		virtual void InitNewFrameNewBlock(void);

		DWORD GetLengthInSeconds(void) { return totalSeconds; }
		void SetLengthInSeconds(DWORD sec) { totalSeconds = sec; }
		virtual BOOL IsVariableBitRate(void) { return FALSE; } // this should be overridden
		BOOL IsForwardScanEnabled(void) { return forwardScanEnabled; }
		BOOL IsReverseScanEnabled(void) { return reverseScanEnabled; }
		virtual void PreserveParseState(void) { preservedParseState = genericParseState; }
		virtual void RestoreParseState(void) { genericParseState = preservedParseState; }

		DWORD GetBytesPerSecond(void) { return bytesPerSecond; }
		void SetBytesPerSecond(DWORD bytesPerSec) { bytesPerSecond = bytesPerSec; }
		virtual int GetMaxLockedFrames(void) = 0;
		virtual int GetNumberOfCheckpoints(void) = 0;
		virtual int GetMaxFrameSize(void) = 0;
		virtual int GetMaxFramesToLockEachRefill(void) = 0;
		virtual int GetNumberOfRefillsPerTransfer(void) = 0;
		virtual Error ReadFileInfo(AVFInfo * appFileInfo) = 0;
		virtual BOOL NeedToDampVolumeDuringScan(DWORD minBitrate) { return TRUE; }
		virtual void SetNewFile(void) { durationIsAvailable = FALSE; }
		DWORD	GetDiskAccessFlags(void) { return diskAccessFlags; }
		virtual void SetDiskAccessSequential(void) { diskAccessFlags = LOCKBLOCKS_SEQUENTIAL_ACCESS_FLAGS; }
		virtual void SetDiskAccessRandom(void) { diskAccessFlags = LOCKBLOCKS_RANDOM_ACCESS_FLAGS; }
		virtual DWORD GetRandomAccessDiskFlags(void) { return LOCKBLOCKS_RANDOM_ACCESS_FLAGS; }
	};

#endif

