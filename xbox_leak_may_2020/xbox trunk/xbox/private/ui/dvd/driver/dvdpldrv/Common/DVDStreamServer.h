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
//  DVD Stream Server Module
//
//  Declares DVDCell, DVDVOBU, DVDStreamPlayer, DVDStreamServer
//
////////////////////////////////////////////////////////////////////

#ifndef DVDSTREAMSERVER_H
#define DVDSTREAMSERVER_H

#include "Library/Common/WinPorts.h"
#include "Library/Common/KrnlSync.h"
#include "DVDAVHeader.h"
#include "Breakpoint.h"
#include "DVDSequencer.h"
#include "Library/Hardware/mpeg2dec/generic/mp2eldec.h"

////////////////////////////////////////////////////////////////////
//
//  Defines and types
//
////////////////////////////////////////////////////////////////////

#define VOS_FREE						0		// entry is not used, and available
#define VOS_READY						1		// entry is initialized, but not being read yet
#define VOS_READING					2		// entry is currently being read into the buffer
#define VOS_READCOMPLETE			4		// all data has been read from the disk
#define VOS_TRANSFERING				8		// entry is currently being transfered to decoder
#define VOS_TRANSFERCOMPLETE		16		// the transfer to the decoder is complete
#define VOS_PLAYING					32		// the entry is currently being played
#define VOS_PLAYCOMPLETED			64		// playback is completed

#define VOS_TRANSFERING_PLAYING	(VOS_TRANSFERING | VOS_PLAYING)

#define NUM_VOBU_BUFFERS	100
#define NUM_CELL_BUFFERS	4
#define NUM_DVD_BLOCKS_IN_BUFFER	32

enum CellScanType
	{
	CST_PLAYBACK,
	CST_REVERSEPLAYBACK,
	CST_SCANFORWARD,
	CST_SCANBACKWARD,
	CST_TRICKPLAY
	};

enum AngleChangeType
	{
	ACT_NONE,
	ACT_DEFERRED,
	ACT_SEAMLESS,
	ACT_NON_SEAMLESS
	};

////////////////////////////////////////////////////////////////////
//
//  DVDCell Class
//
//  Represents a DVD Cell
//
////////////////////////////////////////////////////////////////////

class DVDCell
	{
	public:
		DVDCell(void) {succ = NULL; pgci = NULL; state = VOS_FREE;}

		DVDCell		*	succ, * pred;

		DVDCPBI				info;
		DVDATSCPBI			atsInfo;
		DVDGenericPGCI	*	pgci;

		BYTE				state;
		BOOL				still;
		BOOL				final;			// TRUE if this is final cell of program
		CellScanType	scan;
		WORD				scanStart, scanStep;

		WORD				num;

		DVDTime			presentationTime;

		DVDTime			firstScanVOBUTime;
		int				firstScanSystemTime;

		ERSBreakpoint breakpoint;		// This one must be initialized before CellCompleted is called

		BOOL IsScanning(void) {return scan == CST_SCANFORWARD || scan == CST_SCANBACKWARD;}
		BOOL IsMultiAngleCell(void) {return info.BlockType() == CBT_ANGLE_BLOCK;}
	};

////////////////////////////////////////////////////////////////////
//
//  DVDVOBU Class
//
//  Represents a DVD VOBU
//
////////////////////////////////////////////////////////////////////

class DVDVOBU
	{
	friend class DVDStreamServer;
	protected:
		RequestHandle rh;
		DVDDataFile * file;
		DWORD navPackBlock;
		DriveBlock navPackDriveBlock;

		Error ReadNavPackData(DWORD block);

	public:
		DVDVOBU	*	succ;
		DWORD			startTransferCount;
		DWORD			endTransferCount;
		DWORD			startReadCount;
		DWORD			endReadCount;

		DWORD			firstBlock;
		DWORD			numBlocks;

		BOOL			still, singleFrame, final;

		BYTE			state;
		DVDCell	*	cell;

		ERSBreakpoint breakpoint;		// This one must be initialized before VOBUCompleted is called

		DVDVOBU (void);
		~DVDVOBU (void);

		void	Init(DVDDataFile * file, DVDCell * cell, DWORD firstBlock, BOOL singleFrame);
		void	Cleanup(void);

		BYTE	GetNVByte(int at) {return navPackDriveBlock.data ? navPackDriveBlock.data[at] : 0;}
		WORD	GetNVWord(int at) {return navPackDriveBlock.data ? MAKEWORD(navPackDriveBlock.data[at+1], navPackDriveBlock.data[at]) : 0;}
		DWORD	GetNVDWord(int at) {return navPackDriveBlock.data ? MAKELONG4(navPackDriveBlock.data[at+3], navPackDriveBlock.data[at+2],
													navPackDriveBlock.data[at+1], navPackDriveBlock.data[at+0]) : 0;}
		void	GetNVBytes(FPTR buff, int at, int num);

		BYTE	GetPCIByte(int at) {return GetNVByte(at + 45);}
		WORD	GetPCIWord(int at) {return GetNVWord(at + 45);}
		DWORD	GetPCIDWord(int at) {return GetNVDWord(at + 45);}
		void	GetPCIBytes(FPTR buff, int at, int num) {GetNVBytes(buff, at + 45, num);}

		BYTE	GetPCIByteSeq(int & at) {at += 1; return GetNVByte(at - 1 + 45);}
		WORD	GetPCIWordSeq(int & at) {at += 2; return GetNVWord(at - 2 + 45);}
		DWORD	GetPCIDWordSeq(int & at) {at += 4; return GetNVDWord(at - 4 + 45);}
		void	GetPCIBytesSeq(FPTR buff, int & at, int num) {at += num; GetNVBytes(buff, at - num + 45, num);}

		BYTE	GetDSIByte(int at) {return GetNVByte(at + 1031);}
		WORD	GetDSIWord(int at) {return GetNVWord(at + 1031);}
		DWORD	GetDSIDWord(int at) {return GetNVDWord(at + 1031);}

		BYTE	GetDSIByteSeq(int & at) {at += 1; return GetNVByte(at - 1 + 1031);}
		WORD	GetDSIWordSeq(int & at) {at += 2; return GetNVWord(at - 2 + 1031);}
		DWORD	GetDSIDWordSeq(int & at) {at += 4; return GetNVDWord(at - 4 + 1031);}

		BOOL	HasNonSeamlessAngleInfo(void);
		BOOL	HasSeamlessAngleInfo(void);
		Error	ReadNavigationPack(void);
		Error	FindNextNavigationPack(DWORD final, WORD angle, BOOL & suspendTransfer);

		DWORD	NextVOBU(void);
		DWORD	PrevVOBU(void);
		BOOL	HasVideoData(void);
		DWORD	SeamlessAngleChangeVOBU(WORD angle);
		DWORD	NonSeamlessAngleChangeVOBU(WORD angle);
		DWORD	NextTemporalVOBU(WORD halfSeconds);
		DWORD	NextLowerTemporalVOBU(WORD halfSeconds);
		DWORD	PrevTemporalVOBU(WORD halfSeconds);
		BOOL	IsLastVOBUinILVU(void);

		DWORD	GetSPTM(void);
		DWORD GetEPTM(void);
		DVDTime GetTimeOffset(void);
		DVDTime GetDuration(void);
	};

////////////////////////////////////////////////////////////////////
//
//  DVDStreamPlayer Class
//
////////////////////////////////////////////////////////////////////

class DVDStreamPlayer : public EventSender
	{
	public:
		DWORD transferCount, transferCountRemainder;

		DVDStreamPlayer(EventDispatcher* pEventDispatcher) : EventSender(pEventDispatcher) {}
		virtual ~DVDStreamPlayer(void) {}

		virtual DWORD SendData(HBPTR data, DWORD size) = 0;
		virtual DWORD SendDataMultiple(MPEGDataSizePair * data, DWORD size) = 0;
		virtual void  CompleteData(void) = 0;
		virtual void  CancelData(void) = 0;

		virtual void  SetCallback(DWORD position, DVDStreamServer * callback) = 0;
		virtual void  CancelCallback(void) = 0;
		virtual void  WaitForCallbackCanceled(void) = 0;
		virtual DWORD CurrentLocation(void) = 0;

		virtual BOOL  Completed(void) = 0;
		virtual void  StartPlayback(void) = 0;
		virtual void  StartScan(void) = 0;
		virtual void  StartTrickplay(void) = 0;
		virtual void  SetReversePlayback(BOOL reverse) = 0;
		virtual void  SetStillFrameSequenceMode(BOOL stillFrameSequence) = 0;
		virtual void  TerminatePlayback(void) = 0;

		virtual void  SetRefiller(DVDStreamServer * refiller) = 0;
		virtual void  DeleteRefiller(void) = 0;
		virtual void  RequestRefill(void) = 0;

		virtual Error SetAnalogCopyProtectionMode(int mode) = 0;

		virtual void  SetTimeBase(DVDTime timeBase) = 0;
		virtual Error GetPlaybackTime(DVDTime & time) = 0;
		virtual DWORD GetRequestedVOBUID(void) = 0;
		virtual void  SetFirstAvailableVOBUID(DWORD id) = 0;
		virtual void  SetLastAvailableVOBUID(DWORD id) = 0;
		virtual int   GetCurrentFrameRate(void) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  DVD EStd Time Adaption
//
////////////////////////////////////////////////////////////////////

class DVDEStdTimeAdaption
	{
	public:
		virtual void ConvertToEStdTime(BYTE * data, int num) = 0;
	};

////////////////////////////////////////////////////////////////////
//
//  DVD Stream Buffer
//
////////////////////////////////////////////////////////////////////

class DVDStreamBuffer
	{
	protected:
		DWORD	bufferStart, bufferEnd, bufferSize, availData;

	public:
		DVDStreamBuffer(DWORD bufferSize);
		virtual ~DVDStreamBuffer(void);

//		DWORD AvailData(void) {return availData;}
//		DWORD AvailSpace(void) {return bufferSize - availData;}
		DWORD AvailData(void) {return bufferEnd - bufferStart;}
		DWORD AvailSpace(void) {return bufferSize - AvailData();}

		BOOL IsEmpty(void) {return AvailData() == 0;}
		BOOL IsFull(void) {return AvailSpace() == 0;}

		virtual void Flush(void);

		virtual Error ChangeBufferSize(DWORD size) = 0;

		virtual Error ReadIntoBuffer(DVDDataFile * file, DVDEStdTimeAdaption * eStdAdaption, DWORD block, DWORD num) = 0; // in blocks
		virtual DWORD WriteFromBuffer(DVDStreamPlayer * player, DWORD num) = 0; // in bytes

		virtual Error TerminateRequest(void) = 0;
	};


////////////////////////////////////////////////////////////////////
//
//  DVDStreamServer Class
//
////////////////////////////////////////////////////////////////////

class DVDStreamServer : protected WinPort, virtual protected EventSender, protected DVDEStdTimeAdaption
	{
	friend class DVDPGCSequencer;
	protected:
		DVDFileSystem		* dvdfs;
		DVDSequencer		* sequencer;
		DVDStreamPlayer	* player;

		WinPortServer		* server;
		UnitSet				units;

		VDLocalMutex	lock;

		DVDStreamBuffer *	streamBuffer;
		DWORD					streamBufferSize;

		/*
		DWORD				nextVOBUBlock;
		DWORD				numVOBUBlocks;

		DWORD				currentVOBUID;
		DWORD				endPTM, ptsOffset;
		DWORD				translatedStartPTM, translatedEndPTM;

		DVDVOBU			vobus[NUM_VOBU_BUFFERS];
		DVDVOBU		*  firstFreeVOBU;					// First free VOBU in VOBU buffer
		DVDVOBU		*	predReadingVOBU;				// Predecessor of VOBU being currently read
		DVDVOBU		*	readingVOBU;					// VOBU currently being read
		DVDVOBU		*	transferingVOBU;				// VOBU currently being transfered
		DVDVOBU		*	playingVOBU;					// VOBU currently playing
		DWORD				angleChangeVOBU;				// VOBU for which angle change event is to be send
		WORD				angleChangeAngle;				// New angle to send with event

		DWORD				currentReadCount;
		*/

		DVDCell			cells[NUM_CELL_BUFFERS];
		DVDCell		*	firstFreeCell;
		DVDCell		*	readingCell;
		DVDCell		*	transferingCell;
		DVDCell		*	playingCell;

		//WORD				transferSemaphore;

		DVDDataFile	*	file;

		//BOOL 				singleFrame, stillMode, finalBuffer, stillPending;
		BOOL				inTransfer, callbackSemaphore, playRequest, suspendTransfer;
		/*
		WORD				angleChangeRequest;

		ERSBreakpoint breakpoint;			// The breakpoint reached
		BOOL stopAfterNextVOBU;				// TRUE if we have to stop after next VOBU
		BOOL readEveryVOBU;					// If we need to find a special VOBU during scan, set this to TRUE, so every VOBU is read
		*/
		ERSPlaybackDir playbackDir;

		/*
		struct PositionCache					// To cache some info for TimePlay/Search (TransferCellAt)
			{
			BOOL		valid;
			DVDTime	time;				// Presentation time
			DWORD		pgciID;			// Unique ID for pgci
			DWORD		firstBlock;		// First block of VOBU to play
			} positionCache;
		*/

		virtual Error RefillBuffer(BOOL prefetch) = 0;
		virtual Error SendBuffer(void) = 0;

		virtual Error FlushBuffers(void) = 0;

		virtual Error SendCellStarted(DVDCell * cell) = 0;
		virtual Error SendCellCompleted(DVDCell * cell) = 0;
		virtual Error SendCellCorrupt(DVDCell * cell) = 0;

		virtual void Message(WPARAM wParam, LPARAM dParam) = 0;

		virtual DVDStreamPlayer * GetStreamPlayer(void);

		virtual Error Defrost(void) = 0;

	public:
		DVDStreamServer (DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~DVDStreamServer(void);

		virtual Error Init(DVDFileSystem * dvdfs);

		//
		// To be called once in a while
		//
		virtual Error PerformTransfer(int maxTransfer = 0xffff) = 0;

		virtual Error TransferCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0) = 0;
		virtual Error TransferCellAt(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DVDTime time) = 0;
		virtual Error TransferCellScan(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, BOOL forward, WORD first, WORD skip, DWORD vobu) = 0;
		virtual Error TransferCellReverse(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu) = 0;
		virtual Error TransferCellTrickplay(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu) = 0;

//		virtual Error ReplaceCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0) = 0;

		virtual Error AbortTransfer(void) = 0;
		virtual Error AbortTransferAccurate(void) = 0;

		virtual Error StopPlayback(void) = 0;

		virtual Error CompleteStill(void) = 0;
		virtual Error TerminateStill(void) = 0;

		virtual Error GetAngleChangeType(AngleChangeType & type) = 0;
		virtual Error SeamlessAngleChange(WORD angle) = 0;

		virtual Error SetOBS(DVDOBS * obs) = 0;

		virtual Error ReturnVOBU(DVDVOBU * vobu) = 0;
		virtual Error ReturnCell(DVDCell * cell) = 0;

      virtual void PlayerCallback(DWORD signalPosition) = 0;

		virtual Error GetPlaybackTime(DVDTime & time) = 0;
	};

#endif
