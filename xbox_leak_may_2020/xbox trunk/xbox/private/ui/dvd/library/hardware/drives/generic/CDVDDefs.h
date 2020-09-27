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

//////////////////////////////////////////////////////////////////////
//
//  General definitions for CD and DVD
//
//////////////////////////////////////////////////////////////////////

#ifndef CDVDDEFS_H
#define CDVDDEFS_H

#include "Library/Common/Prelude.h"
#include "Library/Common/vddebug.h"

////////////////////////////////////////////////////////////////////
//
//  Definitions for DVD
//
////////////////////////////////////////////////////////////////////

#define DVD_BLOCK_SIZE	2048
#define CD_BLOCK_SIZE	2352

enum DVDAuthenticationCommand
	{
	DAC_START_AUTHENTICATION,
	DAC_READ_CHALLENGE_KEY,
	DAC_WRITE_BUS_KEY,
	DAC_WRITE_CHALLENGE_KEY,
	DAC_READ_BUS_KEY,
	DAC_READ_DISK_KEY,
	DAC_READ_TITLE_KEY,
	DAC_COMPLETE_AUTHENTICATION,
	DAC_CANCEL_AUTHENTICATION
	};

//////////////////////////////////////////////////////////////////////
//
//  CD Definitions
//
//////////////////////////////////////////////////////////////////////

//
//  CD in general
//

#define CD_FRAME_RATE				75
#define CD_FRAME_SIZE				2352
#define CD_MAX_TOC_ENTRY_NUM		100

//
//  Audio CD

#define CDDA_SUBCHANNEL_SIZE		98
#define CDDA_FRAME_SIZE				(CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE)

//
//  CD ROM
//

#define CDROM_FORM1_HEADER			16
#define CDROM_FORM2_HEADER			16
#define CDROM_FORM1_DATA			2048
#define CDROM_FORM2_DATA			2336

//
//  CD ROM XA (also CDI)
//

#define CDROM_XAFORM1_HEADER		24
#define CDROM_XAFORM2_HEADER		24
#define CDROM_XAFORM1_DATA			2048
#define CDROM_XAFORM2_DATA			2324

//
//  Subchannel names
//

enum CDSubChannel
	{
	CSC_CHANNEL_P,
	CSC_CHANNEL_Q,
	CSC_CHANNEL_R,
	CSC_CHANNEL_S,
	CSC_CHANNEL_T,
	CSC_CHANNEL_U,
	CSC_CHANNEL_V,
	CSC_CHANNEL_W
	};

//
//  Q Subchannel information
//

enum CDSubChannelQ
	{
	CSQ_CONTROL	= 0,				// including ADR in LSN
	CSQ_ADR		= CSQ_CONTROL,	// including CONTROL in MSN
	CSQ_TNO		= 1,
	CSQ_X			= 2,				// Equivalent to POINT
	CSQ_POINT	= CSQ_X,			// Equivalent to X
	CSQ_MIN		= 3,
	CSQ_SEC		= 4,
	CSQ_FRAME	= 5,
	CSQ_ZERO		= 6,
	CSQ_AMIN		= 7,
	CSQ_ASEC		= 8,
	CSQ_AFRAME	= 9,
	CSQ_CRC1		= 10,
	CSQ_CRC2		= 11
	};

//
//  Type of the track
//

enum CDTrackType
	{
	CTT_NONE,
	CTT_AUDIO,
	CTT_DATA
	};

//
//  Multisession CD
//

enum CDSessionType
	{
	CST_NONE,
	CST_CDDA,		// Might also be CD ROM
	CST_CDI,
	CST_CDROM_XA
	};

//////////////////////////////////////////////////////////////////////
//
//  CD Directory Entry Class
//
//////////////////////////////////////////////////////////////////////

class CDTocEntry
	{
	protected:
		DWORD			startBlock;			// First block of track (absolute on disk)
		DWORD			numberOfBlocks;	// Number of blocks in the track
		CDTrackType trackType;			// Type of track (from TOC)

	public:
		CDTocEntry(void);
		CDTocEntry(DWORD startBlock, DWORD numberOfBlocks, CDTrackType type);
		CDTocEntry(BYTE startMin, BYTE startSec, BYTE startFrame,
					  BYTE playMin,  BYTE playSec,  BYTE playFrame,
					  CDTrackType type);
		CDTocEntry(const CDTocEntry & cdte);
		~CDTocEntry(void) {}

		CDTocEntry & operator=(const CDTocEntry & arg);

		void			SetStartBlock(DWORD startBlock)							{ this->startBlock = startBlock; }
		void			SetStartBlock(BYTE min, BYTE sec, BYTE frame)		{ startBlock = ((min * 60) + sec) * CD_FRAME_RATE + frame; }
		void			SetNumberOfBlocks(DWORD num)								{ this->numberOfBlocks = num; }
		void			SetNumberOfBlocks(BYTE min, BYTE sec, BYTE frame)	{ numberOfBlocks = ((min * 60) + sec) * CD_FRAME_RATE + frame; }
		void			SetTrackType(CDTrackType type)							{ this->trackType = type; }
		DWORD			GetStartBlock(void)											{ return startBlock; }
		DWORD			GetNumberOfBlocks(void)										{ return numberOfBlocks; }
		CDTrackType	GetTrackType(void)											{ return trackType; }
	};

#endif
