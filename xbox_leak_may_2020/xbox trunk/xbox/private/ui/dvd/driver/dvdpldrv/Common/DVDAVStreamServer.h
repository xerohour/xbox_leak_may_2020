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
//  DVD Audio/Video Stream Server Module
//
//  Declares DVDVideoStreamServer, DVDAudioStreamServer
//
////////////////////////////////////////////////////////////////////

#ifndef DVDAVSTREAMSERVER_H
#define DVDAVSTREAMSERVER_H


#include "DVDStreamServer.h"
#include "Library/Common/WinPorts.h"
#include "Boards/Generic/Vdgendrv.h"
#include "DVDSequencer.h"


////////////////////////////////////////////////////////////////////
//
//  DVDVideoStreamServer Class
//
////////////////////////////////////////////////////////////////////

class DVDVideoStreamServer : public DVDStreamServer
	{
	friend class DVDPGCSequencer;
	protected:
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

		WORD				transferSemaphore;

		DVDVOBS		*	vobs;

		BOOL 				singleFrame, stillMode, finalBuffer, stillPending;
		BOOL				inTransfer, callbackSemaphore;
		WORD				angleChangeRequest;

		ERSBreakpoint breakpoint;			// The breakpoint reached
		BOOL stopAfterNextVOBU;				// TRUE if we have to stop after next VOBU
		BOOL readEveryVOBU;					// If we need to find a special VOBU during scan, set this to TRUE, so every VOBU is read

		struct PositionCache					// To cache some info for TimePlay/Search (TransferCellAt)
			{
			BOOL		valid;
			WORD		cell;				// Number of the cell
			DVDTime	time;				// Presentation time
			DWORD		pgciID;			// Unique ID for pgci
			DWORD		firstBlock;		// First block of VOBU to play
			} positionCache;

		BOOL IsFinalVOBUOfCell(void);

		void ConvertToEStdTime(BYTE * data, int num);

		virtual Error RefillBuffer(BOOL prefetch);
		virtual Error SendBuffer(void);

		virtual Error FlushBuffers(void);

		Error SendVOBUStarted(DVDVOBU * vobu);
		Error SendVOBUCompleted(DVDVOBU * vobu);

		virtual Error SendCellStarted(DVDCell * cell);
		virtual Error SendCellCompleted(DVDCell * cell);
		virtual Error SendCellCorrupt(DVDCell * cell);

		Error SendPlaybackTerminated(Error err);

		virtual void Message(WPARAM wParam, LPARAM dParam);

		Error AdvanceTrickMode(void);

		virtual Error Defrost(void);

	public:
		DVDVideoStreamServer(DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~DVDVideoStreamServer(void);

		virtual Error Init(DVDFileSystem * dvdfs);

		//
		// To be called once in a while
		//
		virtual Error PerformTransfer(int maxTransfer = 0xffff);

		virtual Error TransferCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0);
		virtual Error TransferCellAt(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DVDTime time);
		virtual Error TransferCellScan(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, BOOL forward, WORD first, WORD skip, DWORD vobu);
		virtual Error TransferCellReverse(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu);
		virtual Error TransferCellTrickplay(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu);

//		virtual Error ReplaceCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0);

		virtual Error AbortTransfer(void);
		virtual Error AbortTransferAccurate(void);

		virtual Error StopPlayback(void);

		virtual Error CompleteStill(void);
		virtual Error TerminateStill(void);

		virtual Error GetAngleChangeType(AngleChangeType & type);
		virtual Error SeamlessAngleChange(WORD angle);

		virtual Error SetOBS(DVDOBS * obs);

		virtual Error ReturnVOBU(DVDVOBU * vobu);
		virtual Error ReturnCell(DVDCell * cell);

      virtual void PlayerCallback(DWORD signalPosition);

		virtual Error GetPlaybackTime(DVDTime & time) {return player->GetPlaybackTime(time);}
	};

////////////////////////////////////////////////////////////////////
//
//  DVDAudioStreamServer Class
//
//	 Description:
//		Describes a Stream Server Class for DVD-Audio playback.
//		It is used either for DVD-Audios which contain Audio Only
//		Titles with Audio Still Videos or just Audio Only Titles.
//		Audio Titles with Video (AVTT) are using the
//		DVDVideoStreamServer Class.
//
////////////////////////////////////////////////////////////////////

class DVDAudioStreamServer : public DVDStreamServer
	{
	protected:
		DVDAOBS		*	aobs;

		virtual Error RefillBuffer(BOOL prefetch);
		virtual Error SendBuffer(void);

		virtual Error FlushBuffers(void);

		virtual Error SendCellStarted(DVDCell * cell);
		virtual Error SendCellCompleted(DVDCell * cell);

		void ConvertToEStdTime(BYTE * data, int num);

		virtual void Message(WPARAM wParam, LPARAM dParam);

		virtual Error Defrost(void);

	public:
		DVDAudioStreamServer(DVDSequencer * seq, WinPortServer * server, UnitSet units, EventDispatcher* pEventDispatcher);
		virtual ~DVDAudioStreamServer(void);

		virtual Error Init(DVDFileSystem * dvdfs);

		//
		// To be called once in a while
		//
		virtual Error PerformTransfer(int maxTransfer = 0xffff);

		virtual Error TransferCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0);
		virtual Error TransferCellAt(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DVDTime time);
		virtual Error TransferCellScan(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, BOOL forward, WORD first, WORD skip, DWORD vobu);
		virtual Error TransferCellReverse(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu);
		virtual Error TransferCellTrickplay(DVDGenericPGCI * pgci, WORD cell, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu);

//		virtual Error ReplaceCell(DVDGenericPGCI * pgci, WORD num, BOOL still, BOOL final, DVDTime presentationTime, DWORD vobu = 0, WORD angle = 0);

		virtual Error AbortTransfer(void);
		virtual Error AbortTransferAccurate(void);

		virtual Error StopPlayback(void);

		virtual Error CompleteStill(void);
		virtual Error TerminateStill(void);

		virtual Error GetAngleChangeType(AngleChangeType & type);
		virtual Error SeamlessAngleChange(WORD angle);

		virtual Error SetOBS(DVDOBS * obs);

		virtual Error ReturnVOBU(DVDVOBU * vobu);
		virtual Error ReturnCell(DVDCell * cell);

      virtual void PlayerCallback(DWORD signalPosition);

		virtual Error GetPlaybackTime(DVDTime & time);
	};


#endif // DVDAVSTREAMSERVER_H
