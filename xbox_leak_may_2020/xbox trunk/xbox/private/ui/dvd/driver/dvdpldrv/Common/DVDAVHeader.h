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
//  DVD Audio/Video specific Header File Information
//
////////////////////////////////////////////////////////////////////


#ifndef DVDAVHEADER_H
#define DVDAVHEADER_H

#include "DVDHeader.h"


////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Class
//
////////////////////////////////////////////////////////////////////

class DVDPGCI : public DVDGenericPGCI
	{
	protected:
		DVDNavigationCommand	*	cmd;
		WORD							numPreCmd, numPostCmd;
		BYTE							firstCellOfProgram[128];

		Error ReadCommandList(void);

	public:
		DVDPGCI(DVDDiskPlayerFactory * factory);
		virtual ~DVDPGCI(void);

		Error Init(DVDHeaderFile * file, DWORD start);

		Error GetPresentationTime(DVDTime & time);

		Error GetUserOperations(DWORD & uops);
		Error GetAudioStreamControl(int num, WORD & asc);
		Error GetSubPictureStreamControl(int num, DWORD & spsc);

		Error GetNextProgramChain(WORD & next);
		Error GetPreviousProgramChain(WORD & prev);
		Error GetUpperProgramChain(WORD & upper);

		Error GetPlaybackMode(BYTE & mode);
		Error GetStillTimeValue(BYTE & stillTime);

		Error GetSubPicturePalette(int entry, DWORD & palette);

		Error GetPreCommands(int & num,  DVDNavigationCommand * & cmds);
		Error GetPostCommands(int & num, DVDNavigationCommand * & cmds);
		Error GetCellCommands(DVDNavigationCommand * & cmds);

		Error GetCellInformation(int cell, DVDCPBI & cpbi);
		Error GetATSCellInformation(int cell, DVDATSCPBI & atscpbi);
		Error GetFirstCellOfProgram(int program, WORD & first);
		Error GetLastCellOfProgram(int program, WORD & last);
		Error GetProgramOfCell(int cell, WORD & prog);
		Error GetNumberOfCellsInProgram(int program, WORD & num);
	};

///////////////////////////////////////////////////////////////////////
//
//  Audio Title Set Program Chain Information Class
//
//	 Description:
//		The Audio Title Set Program Chain Information Class describes
//		the ATS_PGCI. ATS_PGCI comprises ATS_PGC General Information,
//		ATS Program Information Table, ATS Cell Playback Information
//		Table and ATS_ASV Playback Information Table.
//		Note that the ATS_PGCI is different as well from the DVD-Video
//		Program Chain Information as from the DVD-Audio Manager Menu
//		Program Chain Information.
//
///////////////////////////////////////////////////////////////////////

class DVDATSPGCI : public DVDGenericPGCI
	{
	protected:

	public:
		DVDATSPGCI(DVDDiskPlayerFactory * factory);
		virtual ~DVDATSPGCI(void);

		Error Init(DVDHeaderFile * file, DWORD start);

		Error GetPresentationTime(DVDTime & time);

		Error GetUserOperations(DWORD & uops);
		Error GetAudioStreamControl(int num, WORD & asc);
		Error GetSubPictureStreamControl(int num, DWORD & spsc);

		Error GetNextProgramChain(WORD & next);
		Error GetPreviousProgramChain(WORD & prev);
		Error GetUpperProgramChain(WORD & upper);

		Error GetPlaybackMode(BYTE & mode);
		Error GetStillTimeValue(BYTE & stillTime);

		Error GetSubPicturePalette(int entry, DWORD & palette);

		Error GetPreCommands(int & num,  DVDNavigationCommand * & cmds);
		Error GetPostCommands(int & num, DVDNavigationCommand * & cmds);
		Error GetCellCommands(DVDNavigationCommand * & cmds);

		Error GetCellInformation(int cell, DVDCPBI & cpbi);
		Error GetATSCellInformation(int cell, DVDATSCPBI & atscpbi);
		Error GetFirstCellOfProgram(int program, WORD & first);
		Error GetLastCellOfProgram(int program, WORD & last);
		Error GetProgramOfCell(int cell, WORD & prog);
		Error GetNumberOfCellsInProgram(int program, WORD & num);
	};

////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Table
//
//	 Description:
//		The class implements a table that describes as well the
//		VTS Program Chain Information (VTS_PGCI) as the ATS
//		Program Chain Information (ATS_PGCI). Furthermore the Video
//		Manager Menu Program Chain Information and the Audio Manager Menu
//		Program Chain Information. Program Chain Information
//		is the Navigation Data to control the presentation of PGC.
//		The PGCIT for Audio Title Set is a little bit different in
//		in comparison with the other PGCIT. The reason that we use
//		one class and not two different classes is that we allocate the
//		PGCIT statically and not dynamically.
//		The Bool value 'isATSPGCIT' is used to distinguish between an
//		ATS_PGCIT and other PGCIT.
//
////////////////////////////////////////////////////////////////////

class DVDPGCIT
	{
	protected:
		DVDDiskPlayerFactory * factory;
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;
		BOOL					isATSPGCIT;

	public:
		DVDPGCIT(DVDDiskPlayerFactory * factory, DVDHeaderFile * file, DWORD start)
			{this->factory = factory; this->file = file; this->start = start; isATSPGCIT = FALSE;}
		DVDPGCIT(DVDDiskPlayerFactory * factory, DVDHeaderFile * file, DWORD start, BOOL ats)
			{this->factory = factory; this->file = file; this->start = start; isATSPGCIT = ats;}
		DVDPGCIT(void)
			{factory = NULL; file = NULL; start = 0; isATSPGCIT = FALSE;}

		Error GetNumberOfPGC(WORD & num);
		Error GetParentalID(WORD pgc, WORD & id);
		Error GetTitle(WORD pgc, WORD & title);
		Error GetMenuType(WORD pgc, VTSMenuType & type);

		Error FindMenu(VTSMenuType type, WORD & menu);

		Error GetBlockMode(WORD pgc, PGCBlockMode & mode);
		Error GetBlockType(WORD pgc, PGCBlockType & type);
		Error HasEntryPGC(WORD pgc, BOOL & hasPGC);

		// used only for DVD-Audio Title Set PGCIT
		Error GetAudioChannels(WORD pgc, BOOL & hasMoreThanTwoChannels);
		// used only for DVD-Audio Title Set PGCIT
		Error GetAudioCodingMode(WORD pgc, DVDAudioCodingMode & acm);

		Error GetPGCI(WORD pgc, DVDGenericPGCI * & pgci); // to be deleted by caller
	};

////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Unit Table
//
//	 Description:
//		The Program Chain Information Unit Table is a table that
//		describes both the information on VMG Menu Program Chain
//		Information which presents Video Manager Menu in each
//		language and the information on AMGM Program Chain Infor-
//		mation which presents Audio Manager Menu in each language.
//
////////////////////////////////////////////////////////////////////

class DVDPGCIUT
	{
	protected:
		DVDDiskPlayerFactory * factory;
		DVDHeaderFile	*	file;
		RequestHandle		rh;
		DWORD					start;

	public:
		DVDPGCIUT(DVDDiskPlayerFactory * factory, DVDHeaderFile * file, DWORD start)
			{this->factory = factory; this->file = file; this->start = start;}
		DVDPGCIUT(void) {factory = NULL; file = NULL; start = 0;}

		Error GetNumberOfLanguageUnits(WORD & num);
		Error MenuExists(WORD lu, VTSMenuType type, BOOL & exists);
		Error GetLanguageCode(WORD lu, WORD & lc);
		Error GetPGCIT(WORD lu, DVDPGCIT & pgcit);
	};

//////////////////////////////////////////////////////////////////////
//
//  Video Title Search Pointer Table (in VMGI)
//
//  Description:
//		Describes search information of Video Title under VIDEO_TS
//		directory.
//
//////////////////////////////////////////////////////////////////////

class DVDVTTSRPT : public DVDSRPT
	{
	public:
		DVDVTTSRPT(void);
		DVDVTTSRPT(DVDHeaderFile * file, DWORD start);
		DVDVTTSRPT(const DVDVTTSRPT & srpt);
		virtual ~DVDVTTSRPT(void);

		Error GetNumberOfPartOfTitle(WORD title, WORD & num);
		Error GetNumberOfAngles(WORD title, WORD & num);
		Error GetUOPS(WORD title, DWORD & uops);

		Error GetTitle(WORD title, WORD & tsn, WORD & ttn);
		Error GetTitleOfTSTitle(WORD tsn, WORD tsTitle, WORD & title);

		Error IsOneSequentialPGCTitle(WORD title, BOOL & iospt);
	};

//////////////////////////////////////////////////////////////////////
//
//  Audio Title Search Pointer Table (in AMGI)
//
//  Description:
//		The DVDATTSRPT implements the ATT_SRPT table that describes
//		the search information of Audio Titles (ATTs).
//
//////////////////////////////////////////////////////////////////////

class DVDATTSRPT : public DVDSRPT
	{
	protected:
		Error IsAOTT(WORD title, BOOL & isAOTT);

	public:
		DVDATTSRPT(void);
		DVDATTSRPT(DVDHeaderFile * file, DWORD start);
		DVDATTSRPT(const DVDATTSRPT & srpt);
		virtual ~DVDATTSRPT(void);

		Error GetNumberOfPartOfTitle(WORD title, WORD & num);
		Error GetNumberOfAngles(WORD title, WORD & num);
		Error GetUOPS(WORD title, DWORD & uops);

		Error GetTitle(WORD title, WORD & tsn, WORD & ttn);
		Error GetTitleOfTSTitle(WORD tsn, WORD tsTitle, WORD & title);

		Error IsOneSequentialPGCTitle(WORD title, BOOL & iospt);
	};

//////////////////////////////////////////////////////////////////////
//
//  Audio Only Title Search Pointer Table (in AMGI)
//
//  Description:
//		The DVDAOTTSRPT implements AOTT_SRPT that describes the
//		search information of Audio Only Titles (AOTTs), and is
//		used for Audio Only Player.
//		Must be implemented when doing an Audio Only Player.
//		For now Audio Only Player Capability is not integrated.
//		(MST, 11/27/2000)
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
//  Audio Still Video Search Pointer Table
//
//  Description:
//		This class describes the search pointer of ASVOB for every ASV
//		in ASVOBS.
//
//////////////////////////////////////////////////////////////////////

class DVDASVSRPT
	{
	protected:
		DVDHeaderFile * hfile;
		RequestHandle rh;
		DWORD start;

	public:
		DVDASVSRPT(void) { hfile = NULL; start = 0; }
		DVDASVSRPT(DVDHeaderFile * file, DWORD start) {this->hfile = file; this->start = start;}
		virtual ~DVDASVSRPT(void) {}

		Error GetASVStartAddress(int absAsvn, int asvIndex, WORD & asv_sa);
	};

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set for Video Manager Menu
//
//	 Description:
//
//////////////////////////////////////////////////////////////////////

class DVDVMGMVOBS : public DVDMGMVOBS
	{
	public:
		DVDVMGMVOBS(DVDHeaderFile * headerFile, const DVDPTLMAI & ptlmai, DVDDataFile * dataFile);
		virtual ~DVDVMGMVOBS(void);

		Error	GetNumberOfAudioStreams(WORD & num);
		Error	GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo);
	};

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set for Audio Manager Menu
//
//	 Description:
//		An Video Object Set for Audio Manager Menu (AMGM_VOB) contains
//		the Presentation Data and the part of the Navigation Data. The
//		Navigation Data are AMGM Presentation Control Information and
//		AMGM Data Search Information. The Presentation Data are Video
//		data, Sub-picture data and Audio data.
//
//////////////////////////////////////////////////////////////////////

class DVDAMGMVOBS : public DVDMGMVOBS
	{
	public:
		DVDAMGMVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai);
		virtual ~DVDAMGMVOBS(void);

		Error GetNumberOfAudioStreams(WORD & num);
		Error GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo);
	};



#endif // DVDAVHEADER_H
