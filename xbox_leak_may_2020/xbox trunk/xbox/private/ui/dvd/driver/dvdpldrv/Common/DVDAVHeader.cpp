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


#include "DVDAVHeader.h"


//////////////////////////////////////////////////////////////////////
//
//  Program Chain Information
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDPGCI::DVDPGCI(DVDDiskPlayerFactory * factory) : DVDGenericPGCI(factory)
	{
	cmd = NULL;
	}

//
//  Destructor
//

DVDPGCI::~DVDPGCI(void)
	{
	delete[]cmd;
	}

Error DVDPGCI::Init(DVDHeaderFile * file, DWORD start)
	{
	WORD num, program;
	WORD w;

	this->file = file;
	this->start = start;

	GNREASSERT(GetNumberOfPrograms(num));

	GNREASSERT(file->ReadWord(start + 230, w, &rh));
	for(program=1; program<=num; program++)
		{
		GNREASSERT(file->ReadByte(start + w + (program - 1), firstCellOfProgram[program-1], &rh));
		}

	GNRAISE_OK;
	}

Error DVDPGCI::GetPresentationTime(DVDTime & time)
	{
	DWORD d;

	GNREASSERT(file->ReadDWord(start + 4, d, &rh));
	time = DVDTime(d);
	GNRAISE_OK;
	}


Error DVDPGCI::GetUserOperations(DWORD & uop)
	{
	return file->ReadDWord(start + 8, uop, &rh);
	}


Error DVDPGCI::GetAudioStreamControl(int num, WORD & asc)
	{
	if (num > 7)
		{
		asc = 0;
		GNRAISE_OK;
		}
	else
		return file->ReadWord(start + 12 + 2 * num, asc, &rh);
	}


Error DVDPGCI::GetSubPictureStreamControl(int num, DWORD & spsc)
	{
	if (num > 31)
		{
		spsc = 0;
		GNRAISE_OK;
		}
	else
		return file->ReadDWord(start + 28 + 4 * num, spsc, &rh);
	}


Error DVDPGCI::GetNextProgramChain(WORD & next)
	{
	GNREASSERT(file->ReadWord(start + 156, next, &rh));
	next = XTBFW(0, 15, next);
	GNRAISE_OK;
	}


Error DVDPGCI::GetPreviousProgramChain(WORD & prev)
	{
	GNREASSERT(file->ReadWord(start + 156 + 2, prev, &rh));
	prev = XTBFW(0, 15, prev);
	GNRAISE_OK;
	}


Error DVDPGCI::GetUpperProgramChain(WORD & upper)
	{
	return file->ReadWord(start + 156 + 4, upper, &rh);
	}


Error DVDPGCI::GetPlaybackMode(BYTE & mode)
	{
	return file->ReadByte(start + 156 + 6, mode, &rh);
	}


Error DVDPGCI::GetStillTimeValue(BYTE & stillTime)
	{
	return file->ReadByte(start + 156 + 7, stillTime, &rh);
	}


Error DVDPGCI::GetSubPicturePalette(int entry, DWORD & palette)
	{
	GNREASSERT(file->ReadDWord(start + 164 + 4 * entry, palette, &rh));
	palette <<= 8;
	GNRAISE_OK;
	}

//
//  Read Command List
//
//  GNR_OK
//

Error DVDPGCI::ReadCommandList(void)
	{
	RequestHandle rh;
	DWORD position;
	WORD	w;

	if (!cmd)
		{
		GNREASSERT(file->ReadWord(start + 228, w, &rh));
		if (w)
			{
			position = start + w;
			GNREASSERT(file->ReadWordSeq(position, numPreCmd, &rh));
			GNREASSERT(file->ReadWordSeq(position, numPostCmd, &rh));
			GNREASSERT(file->ReadWordSeq(position, w, &rh));
			position += 2;
			cmd = new DVDNavigationCommand[numPreCmd + numPostCmd + w];
			GNREASSERT(file->ReadBytes(position, 8 * (numPreCmd + numPostCmd + w), (HBPTR)cmd, DAF_CACHED, &rh));
			}
		else
			{
			numPreCmd = numPostCmd = 0;
			cmd = NULL;
			}
		}

	GNRAISE_OK;
	}

//
//  Return Pre Commands
//
//  GNR_OK
//

Error DVDPGCI::GetPreCommands(int & num,  DVDNavigationCommand * & cmds)
	{
	GNREASSERT(ReadCommandList());

	if (cmd)
		{
		cmds = cmd;
		num = numPreCmd;
		}
	else
		{
		cmds = NULL;
		num = 0;
		}

	GNRAISE_OK;
	}

//
//  Return Post Commands
//
//  GNR_OK
//

Error DVDPGCI::GetPostCommands(int & num, DVDNavigationCommand * & cmds)
	{
	GNREASSERT(ReadCommandList());

	if (cmd)
		{
		cmds = cmd + numPreCmd;
		num = numPostCmd;
		}
	else
		{
		cmds = NULL;
		num = 0;
		}

	GNRAISE_OK;
	}

//
//  Get Cell Commands
//
//  GNR_OK
//

Error DVDPGCI::GetCellCommands(DVDNavigationCommand * & cmds)
	{
	GNREASSERT(ReadCommandList());

	if (cmd)
		cmds = cmd + numPreCmd + numPostCmd;
	else
		cmds = NULL;

	GNRAISE_OK;
	}


Error DVDPGCI::GetCellInformation(int cell, DVDCPBI & cpbi)
	{
	WORD w;

	GNREASSERT(file->ReadWord(start + 232, w, &rh));
	return cpbi.Init(file, start + w + 24 * (cell - 1));
	}

Error DVDPGCI::GetATSCellInformation(int cell, DVDATSCPBI & atscpbi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error DVDPGCI::GetFirstCellOfProgram(int program, WORD & first)
	{
	first = firstCellOfProgram[program-1];

	GNRAISE_OK;
	}


Error DVDPGCI::GetLastCellOfProgram(int program, WORD & last)
	{
	WORD first;
	WORD num;

	GNREASSERT(GetFirstCellOfProgram(program, first));
	GNREASSERT(GetNumberOfCellsInProgram(program, num));
	last = first + num - 1;
	GNRAISE_OK;
	}


Error DVDPGCI::GetProgramOfCell(int cell, WORD & prog)
	{
	WORD num;
	WORD first;

	GNREASSERT(GetNumberOfPrograms(num));

	prog = 1;
	GNREASSERT(GetFirstCellOfProgram(prog + 1, first));
	while (prog < num && cell >= first)
		{
		prog++;
		GNREASSERT(GetFirstCellOfProgram(prog + 1, first));
		}

	GNRAISE_OK;
	}


Error DVDPGCI::GetNumberOfCellsInProgram(int program, WORD & num)
	{
	WORD first;

	GNREASSERT(GetFirstCellOfProgram(program, first));
	GNREASSERT(GetNumberOfPrograms(num));

	if (program == num)
		{
		GNREASSERT(GetNumberOfCells(num));
		num = num + 1 - first;
		GNRAISE_OK;
		}
	else
		{
		GNREASSERT(GetFirstCellOfProgram(program + 1, num));
		num -= first;
		GNRAISE_OK;
		}
	}

////////////////////////////////////////////////////////////////////
//
//  Audio Title Set Program Chain Information Class
//
////////////////////////////////////////////////////////////////////

DVDATSPGCI::DVDATSPGCI(DVDDiskPlayerFactory * factory) : DVDGenericPGCI(factory)
	{

	}

DVDATSPGCI::~DVDATSPGCI(void)
	{
	}

Error DVDATSPGCI::Init(DVDHeaderFile * file, DWORD start)
	{
	this->file = file;
	this->start = start;
	GNRAISE_OK;
	}

//
// The total presentation time in ATS_PGCI is measured in the following
//	format:
//		Total presentation time = ATS_PGC_PB_TM[31..0] / 90000 (seconds)
//

Error DVDATSPGCI::GetPresentationTime(DVDTime & time)
	{
	DWORD d;

	GNREASSERT(file->ReadDWord(start + 4, d, &rh));
	time = DVDTime(0, 0, d/90000, 0, 0);
	GNRAISE_OK;
	}

//
// There are no user operation for AOTTs.
//	0b: Corresponding User Operation is permitted.
//	1b: Corresponding User Operation is prohibited.
//

Error DVDATSPGCI::GetUserOperations(DWORD & uops)
	{
	uops = 0;
	GNRAISE_OK;
	}

//
// There is no Audio Stream Control for AOTTs.
//

Error DVDATSPGCI::GetAudioStreamControl(int num, WORD & asc)
	{
	asc = 0;
	GNRAISE_OK;
	}

//
// There is no Sub-Picture Stream Control for AOTTs.
//

Error DVDATSPGCI::GetSubPictureStreamControl(int num, DWORD & spsc)
	{
	spsc = 0;
	GNRAISE_OK;
	}

//
// There is no Navigation Control for AOTTs.
// 0 means the PGC is nonexistent.
//

Error DVDATSPGCI::GetNextProgramChain(WORD & next)
	{
	next = 0;
	GNRAISE_OK;
	}

//
// There is no Navigation Control for AOTTs.
// 0 means the PGC is nonexistent.
//

Error DVDATSPGCI::GetPreviousProgramChain(WORD & prev)
	{
	prev = 0;
	GNRAISE_OK;
	}

//
// There is no Navigation Control for AOTTs.
// 0 means the PGC is nonexistent.
//

Error DVDATSPGCI::GetUpperProgramChain(WORD & upper)
	{
	upper = 0;
	GNRAISE_OK;
	}

//
// There is no Navigation Control for AOTTs.
// 0 means sequential playback.
//

Error DVDATSPGCI::GetPlaybackMode(BYTE & mode)
	{
	mode = 0;
	GNRAISE_OK;
	}

//
// There is no Navigation Control for AOTTs.
//	0 means no still value.
//

Error DVDATSPGCI::GetStillTimeValue(BYTE & stillTime)
	{
	stillTime = 0;
	GNRAISE_OK;
	}

//
// There is no Sub-Picture Palette for AOTTs.
// Return the min values for all colours:
//	16 <= Y <= 235
// 16 <= Cr <= 240
//	16 <= Cb <= 240
//

Error DVDATSPGCI::GetSubPicturePalette(int entry, DWORD & palette)
	{
	palette = 0x00101010;
	GNRAISE_OK;
	}

//
// There is no Command Table for AOTTs.
//

Error DVDATSPGCI::GetPreCommands(int & num,  DVDNavigationCommand * & cmds)
	{
	cmds = NULL;
	num = 0;
	GNRAISE_OK;
	}

//
// There is no Command Table for AOTTs.
//

Error DVDATSPGCI::GetPostCommands(int & num, DVDNavigationCommand * & cmds)
	{
	cmds = NULL;
	num = 0;
	GNRAISE_OK;
	}

//
// There is no Command Table for AOTTs.
//

Error DVDATSPGCI::GetCellCommands(DVDNavigationCommand * & cmds)
	{
	cmds = NULL;
	GNRAISE_OK;
	}

//
//	Only used for DVD-Video or DVD-Audio Manager Menu
//

Error DVDATSPGCI::GetCellInformation(int cell, DVDCPBI & cpbi)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
// Return Audio Title Set Cell Playback Information
//

Error DVDATSPGCI::GetATSCellInformation(int cell, DVDATSCPBI & atscpbi)
	{
	WORD w;

	GNREASSERT(file->ReadWord(start + 12, w, &rh));
	return atscpbi.Init(file, start + w + 12 * (cell - 1));
	}

//
// There is no Program Map for AOTTs.
//

Error DVDATSPGCI::GetFirstCellOfProgram(int program, WORD & first)
	{
	first = 0;
	GNRAISE_OK;
	}

Error DVDATSPGCI::GetLastCellOfProgram(int program, WORD & last)
	{
	last = 0;
	GNRAISE_OK;
	}

Error DVDATSPGCI::GetProgramOfCell(int cell, WORD & prog)
	{
	prog = 0;
	GNRAISE_OK;
	}

Error DVDATSPGCI::GetNumberOfCellsInProgram(int program, WORD & num)
	{
	num = 0;
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Table
//
////////////////////////////////////////////////////////////////////

Error DVDPGCIT::GetNumberOfPGC(WORD & num)
	{
	return file->ReadWord(start, num, &rh);
	}

Error DVDPGCIT::GetParentalID(WORD pgc, WORD & id)
	{
	if (isATSPGCIT)
		id = 0;
	else
		GNREASSERT(file->ReadWord(start + 8 + 8 * (pgc - 1) + 2, id, &rh));

	GNRAISE_OK;
	}

Error DVDPGCIT::GetTitle(WORD pgc, WORD & title)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1), b, &rh));
	title = XTBFB(0, 7, b);
	GNRAISE_OK;
	}

Error DVDPGCIT::GetMenuType(WORD pgc, VTSMenuType & type)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1), b, &rh));

	switch (XTBFB(0, 4, b))
		{
		case 2:
			type = VMT_TITLE_MENU;
			break;
		case 3:
			type = VMT_ROOT_MENU;
			break;
		case 4:
			type = VMT_SUBPICTURE_MENU;
			break;
		case 5:
			type = VMT_AUDIO_MENU;
			break;
		case 6:
			type = VMT_ANGLE_MENU;
			break;
		case 7:
			type = VMT_PTT_MENU;
			break;
		default:
			type = VMT_NO_MENU;
			break;
		}

	GNRAISE_OK;
	}

Error DVDPGCIT::FindMenu(VTSMenuType type, WORD & menu)
	{
	VTSMenuType mt;
	WORD num;
	BOOL hasEntryPGC;

	GNREASSERT(GetNumberOfPGC(num));

	for (menu=1; menu<=num; menu++)
		{
		GNREASSERT(GetMenuType(menu, mt));
		GNREASSERT(HasEntryPGC(menu, hasEntryPGC));
		if (type == mt && hasEntryPGC)
			GNRAISE_OK;
		}

//	GNRAISE(GNR_OBJECT_NOT_FOUND);
	menu = 0;
	GNRAISE_OK;
	}

//
// Get the number of channels of the Audio data contained in an ATS_PGC.
// TRUE: Audio data of more than 2 channels is included
// FALSE: every Audio data is 2 channels or less
//

Error DVDPGCIT::GetAudioChannels(WORD pgc, BOOL & hasMoreThanTwoChannels)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1) + 1, b, &rh));

	hasMoreThanTwoChannels = XTBFB(0, 4, b);

	GNRAISE_OK;
	}

//
//	Get Audio coding mode of the audio data contained in an ATS_PGC
//

Error DVDPGCIT::GetAudioCodingMode(WORD pgc, DVDAudioCodingMode & acm)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1) + 2, b, &rh));

	//
	// Audio coding mode in ATS_PGC_TABLE is defined as follows:
	//		0000 0000b: Linear PCM audio
	//		0000 0001b: Packed PCM audio
	//		0000 0010b: Dolby Digital
	//		0000 0011b: MPEG-2 without extension
	//		0000 0100b: MPEG-2 with extension
	//		0000 0101b: DTS
	//		0000 0110b: SDDS

	switch (b)
		{
		case 0:
		case 1:
			acm = DAM_LPCM;
			break;
		case 2:
			acm = DAM_AC3;
			break;
		case 3:
		case 4:
			acm = DAM_MPEG2;
			break;
		case 5:
			acm = DAM_DTS;
			break;
		case 6:
			acm = DAM_SDDS;
			break;
		default:
			acm = DAM_LPCM;
			break;
		}

	GNRAISE_OK;
	}

Error DVDPGCIT::GetBlockMode(WORD pgc, PGCBlockMode & mode)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1) + 1, b, &rh));
	mode = (PGCBlockMode)XTBFB(6, 2, b);
	GNRAISE_OK;
	}

Error DVDPGCIT::GetBlockType(WORD pgc, PGCBlockType & type)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1) + 1, b, &rh));
	type = (PGCBlockType)XTBFB(4, 2, b);

	// value "1" for the block type is different in the meaning between DVD-Video and DVD-Audio.
	// For DVD-Video it means Parental Block whereas for DVD-Audio it means block for the difference
	// of only "Audio Coding Mode". So we have to adapt the block type!
	if (type == PBT_PARENTAL_BLOCK && isATSPGCIT)
		type = PBT_AUDIO_CODING_DIFF;

	GNRAISE_OK;
	}

Error DVDPGCIT::HasEntryPGC(WORD pgc, BOOL & entryPGC)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + 8 * (pgc - 1), b, &rh));
	entryPGC = (BOOL)XTBF(7, b);
	GNRAISE_OK;
	}

Error DVDPGCIT::GetPGCI(WORD pgc, DVDGenericPGCI * & pgci)
	{
	Error err;
	DWORD d;

	GNREASSERT(file->ReadDWord(start + 8 + 8 * (pgc - 1) + 4, d, &rh));
	if (isATSPGCIT)
		pgci = new DVDATSPGCI(factory);
	else
		pgci = new DVDPGCI(factory);
	if (IS_ERROR(err = pgci->Init(file, start + d)))
		{
		delete pgci;
		pgci = NULL;
		}

	GNRAISE(err);
	}

//////////////////////////////////////////////////////////////////////
//
//  Program Chain Information Unit Table
//
//////////////////////////////////////////////////////////////////////

//
//  Return the number of language units
//

Error DVDPGCIUT::GetNumberOfLanguageUnits(WORD & num)
	{
	return file->ReadWord(start, num, &rh);
	}

//
//  Check if menu exists
//

Error DVDPGCIUT::MenuExists(WORD lu, VTSMenuType type, BOOL & exists)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 8 + (lu - 1) * 8 + 3, b, &rh));

	switch (type)
		{
		case VMT_TITLE_MENU:
			exists = XTBF(7, b);
			break;
		case VMT_ROOT_MENU:
			exists = XTBF(7, b);
			break;
		case VMT_SUBPICTURE_MENU:
			exists = XTBF(6, b);
			break;
		case VMT_AUDIO_MENU:
			exists = XTBF(5, b);
			break;
		case VMT_ANGLE_MENU:
			exists = XTBF(4, b);
			break;
		case VMT_PTT_MENU:
			exists = XTBF(3, b);
			break;
		default:
			GNRAISE(GNR_INVALID_PARAMETERS);
		}

	GNRAISE_OK;
	}

//
//  Return the language code
//

Error DVDPGCIUT::GetLanguageCode(WORD lu, WORD & lc)
	{
	return file->ReadWord(start + 8 + (lu - 1) * 8, lc, &rh);
	}

//
//  Initialize the PGCIT
//

Error DVDPGCIUT::GetPGCIT(WORD lu, DVDPGCIT & pgcit)
	{
	DWORD d;

	GNREASSERT(file->ReadDWord(start + 8 + (lu - 1) * 8 + 4, d, &rh));
	pgcit = DVDPGCIT(factory, file, start + d);
	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Title Search Pointer Table (in VMGI)
//
//////////////////////////////////////////////////////////////////////

DVDVTTSRPT::DVDVTTSRPT(void) : DVDSRPT()
	{

	}

DVDVTTSRPT::DVDVTTSRPT(DVDHeaderFile * file, DWORD start) : DVDSRPT(file, start)
	{

	}

DVDVTTSRPT::DVDVTTSRPT(const DVDVTTSRPT & srpt) : DVDSRPT(srpt)
	{

	}

DVDVTTSRPT::~DVDVTTSRPT(void)
	{

	}

//
//  Return number of Part Of Titles of Title 'title'
//

Error DVDVTTSRPT::GetNumberOfPartOfTitle(WORD title, WORD & num)
	{
	return file->ReadWord(start + 8 + (title - 1) * 12 + 2, num, &rh);
	}

//
//  Return number of Angles of Title 'title'
//

Error DVDVTTSRPT::GetNumberOfAngles(WORD title, WORD & num)
	{
	BYTE b;
	Error err;

	if (title)
		{
		err = file->ReadByte(start + 8 + (title - 1) * 12 + 1, b, &rh);
		num = b;
		GNRAISE(err);
		}
	else
		{
		num = 1;
		GNRAISE_OK;
		}
	}

//
//  Get UOPs
//

Error DVDVTTSRPT::GetUOPS(WORD title, DWORD & uops)
	{
	BYTE stat;

	GNREASSERT(file->ReadByte(start + 8 + (title - 1) * 12 + 0, stat, &rh));
	uops = (DWORD)stat & 3;
	GNRAISE_OK;
	}

//
//  Get Video Title Set and Video Title Set Title number from global Title Number
//

Error DVDVTTSRPT::GetTitle(WORD title, WORD & tsn, WORD & ttn)
	{
	BYTE vtsNumber, vtsTitleNumber;
	DWORD pos = start + 8 + (title - 1) * 12;

	pos += 6;

	GNREASSERT(file->ReadByteSeq(pos, vtsNumber, &rh));
	GNREASSERT(file->ReadByteSeq(pos, vtsTitleNumber, &rh));

	tsn = vtsNumber;
	ttn = vtsTitleNumber;

	GNRAISE_OK;
	}

//
//	 Get title from Video Title Set Number and Video Title Set Title Number
//

Error DVDVTTSRPT::GetTitleOfTSTitle(WORD tsn, WORD tsTitle, WORD & title)
	{
	BYTE vtsNumber, vtsTitleNumber;
	DWORD pos;
	WORD num;

	GNREASSERT(GetNumberOfTitles(num));
	for(title = 1; title <= num; title++)
		{
		pos = start + 8 + (title - 1) * 12 + 6;

		//
		// Check Video Title Set number
		//

		GNREASSERT(file->ReadByteSeq(pos, vtsNumber, &rh));
		if (vtsNumber == tsn)
			{
			//
			//  Check Video Title Set Title Number
			//

			GNREASSERT(file->ReadByteSeq(pos, vtsTitleNumber, &rh));
			if (vtsTitleNumber == tsTitle)
				GNRAISE_OK;
			}
		}

	title = 0;

	GNRAISE(GNR_RANGE_VIOLATION);
	}

//
//  Check if title is "OneSequentialPGCTitle"
//

Error DVDVTTSRPT::IsOneSequentialPGCTitle(WORD title, BOOL & iospt)
	{
	BYTE titlePlaybackType = 0x40;	// This makes sure that breakpoints are not allowed in case of error

	GNREASSERT(file->ReadByte(start + 8 + 12 * (title - 1), titlePlaybackType, &rh));
	iospt = !(titlePlaybackType & 0x40);
	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Audio Title Search Pointer Table (in AMGI)
//
//////////////////////////////////////////////////////////////////////

DVDATTSRPT::DVDATTSRPT(void) : DVDSRPT()
	{

	}

DVDATTSRPT::DVDATTSRPT(DVDHeaderFile * file, DWORD start) : DVDSRPT(file, start)
	{

	}

DVDATTSRPT::DVDATTSRPT(const DVDATTSRPT & srpt) : DVDSRPT(srpt)
	{

	}

DVDATTSRPT::~DVDATTSRPT(void)
	{

	}

//
// Describes whether this ATT is AOTT or AVTT
//	0b: AVTT, 1b: AOTT
//

Error DVDATTSRPT::IsAOTT(WORD title, BOOL & isAOTT)
	{
	BYTE b;

	GNREASSERT(file->ReadByte(start + 4 + (title - 1) * 14 + 0, b, &rh));
	if (b & 0x80) isAOTT = TRUE;
	else isAOTT = FALSE;

	GNRAISE_OK;
	}

Error DVDATTSRPT::GetNumberOfPartOfTitle(WORD title, WORD & num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Return number of Angles of Title 'title' when this is a AVTT
//	 else return unimplemented
//

Error DVDATTSRPT::GetNumberOfAngles(WORD title, WORD & num)
	{
	BYTE b;
	Error err;
	BOOL isAOTT;

	GNREASSERT(IsAOTT(title, isAOTT));

	if (!isAOTT)
		{
		err = file->ReadByte(start + 4 + (title - 1) * 14 + 2, b, &rh);
		num = b;

		GNRAISE(err);
		}
	else
		num = 0;

	GNRAISE_OK;
	}

Error DVDATTSRPT::GetUOPS(WORD title, DWORD & uops)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

Error DVDATTSRPT::GetTitle(WORD title, WORD & tsn, WORD & ttn)
	{
	BYTE atsNumber, atsTrackNumber;
	DWORD pos = start + 4 + (title - 1) * 14;

	pos += 8;

	GNREASSERT(file->ReadByteSeq(pos, atsNumber, &rh));
	GNREASSERT(file->ReadByteSeq(pos, atsTrackNumber, &rh));

	tsn = atsNumber;
	ttn = atsTrackNumber;

	GNRAISE_OK;
	}

//
//	 Get title from Audio Title Set Number and Audio Title Set Title Number
//

Error DVDATTSRPT::GetTitleOfTSTitle(WORD tsn, WORD tsTitle, WORD & title)
	{
	BYTE atsNumber, atsTitleNumber;
	DWORD pos;
	WORD num;

	GNREASSERT(GetNumberOfTitles(num));
	for(title = 1; title <= num; title++)
		{
		pos = start + 4 + (title - 1) * 14 + 8;

		//
		// Check Audio Title Set number
		//

		GNREASSERT(file->ReadByteSeq(pos, atsNumber, &rh));
		if (atsNumber == tsn)
			{
			//
			//  Check Audio Title Set Title Number
			//

			GNREASSERT(file->ReadByteSeq(pos, atsTitleNumber, &rh));
			if (atsTitleNumber == tsTitle)
				GNRAISE_OK;
			}
		}

	title = 0;

	GNRAISE(GNR_RANGE_VIOLATION);
	}

Error DVDATTSRPT::IsOneSequentialPGCTitle(WORD title, BOOL & iospt)
	{
	//
	// An Audio with Video Title (AVTT) is always defined as One-sequential PGC in the VTS.
	// The AVTT is the ATT accompanied with video contents.
	// So we have to make a distinction if the ATT is an AVTT or AOTT and return the
	// appropriate type.
	// => if (AVTT) iospt = TRUE; else if (AOTT) iospt = FALSE;
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//////////////////////////////////////////////////////////////////////
//
//  Audio Still Video Search Pointer Table
//
//////////////////////////////////////////////////////////////////////

//
// Returns the start address of the ASVOB. Whereas absAsvn describes
//	the first ABS_ASVN of this ASVU and asvIndex describes the
// ASV number.
//

Error DVDASVSRPT::GetASVStartAddress(int absAsvn, int asvIndex, WORD & asv_sa)
	{
	GNREASSERT(hfile->ReadWord(start + absAsvn + 2 * asvIndex, asv_sa, &rh));

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Manager Menu VOBS
//
//////////////////////////////////////////////////////////////////////

DVDVMGMVOBS::DVDVMGMVOBS(DVDHeaderFile * headerFile, const DVDPTLMAI & ptlmai, DVDDataFile * dataFile)
	: DVDMGMVOBS(headerFile, dataFile, ptlmai)
	{

	}

DVDVMGMVOBS::~DVDVMGMVOBS(void)
	{

	}

Error DVDVMGMVOBS::GetNumberOfAudioStreams(WORD & num)
	{
	return headerFile->ReadWord(258, num, &rh);
	}

Error DVDVMGMVOBS::GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo)
	{
	GNREASSERT(headerFile->ReadWord(260 + 8 * num, attrib, &rh));

	//
	// language, extension, and applicationInfo are reserved according to the DVD specification.
	// So initialize them with initial values (MST, 03/07/01)
	//
	language = 0xffff;
	extension = 0x0000;
	applicationInfo = 0x0000;

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set for Audio Manager Menu
//
//////////////////////////////////////////////////////////////////////

DVDAMGMVOBS::DVDAMGMVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai)
	: DVDMGMVOBS(headerFile, dataFile, ptlmai)
	{

	}

DVDAMGMVOBS::~DVDAMGMVOBS(void)
	{

	}

Error DVDAMGMVOBS::GetNumberOfAudioStreams(WORD & num)
	{
	return headerFile->ReadWord(348, num, &rh);
	}

Error DVDAMGMVOBS::GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo)
	{
	GNREASSERT(headerFile->ReadWord(350 + 8 * num, attrib, &rh));

	//
	// language, extension, and applicationInfo are reserved according to the DVD specification.
	// So initialize them with initial values (MST, 03/07/01)
	//
	language = 0xffff;
	extension = 0x0000;
	applicationInfo = 0x0000;

	GNRAISE_OK;
	}
