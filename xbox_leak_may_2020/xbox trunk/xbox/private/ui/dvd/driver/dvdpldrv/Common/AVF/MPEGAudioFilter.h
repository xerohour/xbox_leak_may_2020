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

// FILE:       Driver/Dvdpldrv/Common/AVF/MPEGAudioFilter.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    23-OCT-2000
//
// PURPOSE:    MPEG audio specific file parser for audio and video files
//
// HISTORY:

#ifndef MPEGAUDIOFILTER_H
#define MPEGAUDIOFILTER_H

#include "Driver/Dvdpldrv/Common/AVF/AVFFilter.h"
#include "Driver/Dvdpldrv/Common/AVF/MPEGAudioHeader.h"

#define MPEG_AUDIO_MAX_LOCKED_FRAMES				50 // 20 is OK for 320 kbps, need more
																	// for lower bitrates
#define MPEG_AUDIO_MAX_CHECKPOINTS					200
#define MPEG_AUDIO_MAX_FRAME_SIZE					6913
	// worst case for padded 384 kbps layer 2 @ 8 kHz
	// ((144 * (384000)) / 8000) + 1;

#define MPEG_AUDIO_MAX_FRAMES_PER_REFILL			40 // don't have to fill the queue each time
#define MPEG_AUDIO_NUM_REFILLS_PER_TRANSFER		5 // don't have to fill the queue each time

#define MPEG_AUDIO_MAX_BLOCKS_WITH_NO_HEADER		12	// this does not count any headers
	// e.g. ID3v2 can be 64K long at the beginning of a file

////////////////////////////////////////////////////////////////////
//
//  MPEG Audio Frame Header List Class
//
////////////////////////////////////////////////////////////////////

class MPEGAudioFrameHeaderList
	{
	public:
		MPEGAudioFrameHeader	*pHeaderArray;
		int						arraySize;
		MPEGAudioFrameHeaderList(void);
		~MPEGAudioFrameHeaderList(void);
		void Cleanup(void);
		Error Initialize(int	maxLockedFrames);
	};

////////////////////////////////////////////////////////////////////
//
//  MPEG Audio Filter Class
//
////////////////////////////////////////////////////////////////////

class MPEGAudioFilter : public AVFFilter
	{
	private:
		// Parse state information (defined in MpegAudioHeader.h)
		MPEGAudioParseState			parseState;		// also includes non-audio frame info
		// DWORD								optimizedStartBlock;
		// DWORD								optimizedStartBlockOffset;
		BOOL							nonAudioInfoIsAvailable; // non-audio info
		BOOL							nonAudioInfoReadAttempted; // only check once per file

		Error LoadFirstFrameInfo(MPEGAudioFrameHeader *pHeader, BOOL forceVBR=FALSE);
		void InvalidateFirstFrameInfo(void);
		Error	ID3v1ByteFilter(BYTE b, DWORD &bytesToSkip, MPEGNonAudioFrameType &type,
						 			 BOOL &frameDone, MPEGNonAudioParseMode mode);

		// virtual Error InitializeDataStructures(int maxLockedFrames, int maxFrameSize);

	public:
		MPEGAudioFilter(void);
		virtual ~MPEGAudioFilter(void);

		MPEGAudioFrameHeaderList	frameHeaderList;
		Error FindNextFrame(DriveBlockNode *pBlockNode, AVFFrame *pFrame, BOOL lastBlockInFile);
		virtual Error DetermineDuration(void);
		virtual BOOL NeedToDampVolumeDuringScan(DWORD minBitrate);
		DWORD CalculateLengthInSeconds(void);
		Error LoadID3v1Info(void);
		virtual Error OpenFileInitialization(GenericFile * gf,
									AVFileSizeAttributes *pSizeInfo,
									AVFInfo *pInfo = (AVFInfo *) NULL,
									BOOL streamingIsEnabled = FALSE);
		DWORD GetBytesPerSecond(void);
		BOOL IsVariableBitRate(void);
		BOOL	IsInfoAvailable(void) { return nonAudioInfoIsAvailable; }
		virtual void SetNewFile(void);
		Error ReadFileInfo(AVFInfo * appFileInfo);

		virtual void Cleanup(void);
		virtual void Invalidate(void);

#ifdef _DEBUG
		void DebugDumpParseState(MPEGAudioParseState &parseState);
		void DebugDumpHeaderInfo(MPEGAudioFrameHeader *pHeader);
		void DebugDumpID3v1Info(void);
#endif

		void InitFirstFrameInFile(void);
		void InitNewFrameSameBlock(void);
		void InitSameFrameNewBlock(void);
		void InitNewFrameNewBlock(void);

		virtual int GetMaxLockedFrames(void) { return MPEG_AUDIO_MAX_LOCKED_FRAMES; }
		virtual int GetNumberOfCheckpoints(void) { return MPEG_AUDIO_MAX_CHECKPOINTS; }
		virtual int GetMaxFrameSize(void) { return MPEG_AUDIO_MAX_FRAME_SIZE; }
		virtual int GetMaxFramesToLockEachRefill(void) { return MPEG_AUDIO_MAX_FRAMES_PER_REFILL; }
		virtual int GetNumberOfRefillsPerTransfer(void) {return MPEG_AUDIO_NUM_REFILLS_PER_TRANSFER; }
	};

#endif

