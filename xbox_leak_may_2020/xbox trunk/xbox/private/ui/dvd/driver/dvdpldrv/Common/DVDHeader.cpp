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
//  DVD Header File Information
//
////////////////////////////////////////////////////////////////////

#include "DVDHeader.h"


//////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information
//
//////////////////////////////////////////////////////////////////////

//
// Constructor
//

DVDCPBI::DVDCPBI(void)
	{

	}

//
// Destructor
//

DVDCPBI::~DVDCPBI(void)
	{

	}

Error DVDCPBI::Init(DVDHeaderFile * file, DWORD start)
	{
	RequestHandle rh;

	GNREASSERT(file->ReadDWordSeq(start, cat, &rh));
	GNREASSERT(file->ReadDWordSeq(start, pbtm, &rh));
	GNREASSERT(file->ReadDWordSeq(start, fvobu_sa, &rh));
	GNREASSERT(file->ReadDWordSeq(start, filvu_ea, &rh));
	GNREASSERT(file->ReadDWordSeq(start, lvobu_sa, &rh));
	return file->ReadDWordSeq(start, lvobu_ea, &rh);
	}

//////////////////////////////////////////////////////////////////////
//
//  Audio Manager Menu Cell Playback Information
//
//////////////////////////////////////////////////////////////////////

//
// Constructor
//

DVDAMGMCPBI::DVDAMGMCPBI(void)
	{

	}

//
// Destructor
//

DVDAMGMCPBI::~DVDAMGMCPBI(void)
	{

	}

Error DVDAMGMCPBI::Init(DVDHeaderFile * file, DWORD start)
	{
	RequestHandle rh;

	GNREASSERT(file->ReadDWordSeq(start, cat, &rh));
	GNREASSERT(file->ReadDWordSeq(start, pbtm, &rh));
	GNREASSERT(file->ReadDWordSeq(start, fvobu_sa, &rh));
	GNREASSERT(file->ReadDWordSeq(start, lvobu_sa, &rh));
	return file->ReadDWordSeq(start, lvobu_ea, &rh);
	}

////////////////////////////////////////////////////////////////////
//
//  Cell Playback Information for Audio Only Title
//
////////////////////////////////////////////////////////////////////

//
// Constructor
//

DVDATSCPBI::DVDATSCPBI(void)
	{

	}

//
// Destructor
//

DVDATSCPBI::~DVDATSCPBI(void)
	{

	}

Error DVDATSCPBI::Init(DVDHeaderFile * file, DWORD start)
	{
	RequestHandle rh;

	GNREASSERT(file->ReadByteSeq(start, ats_ixn, &rh));
	GNREASSERT(file->ReadByteSeq(start, ats_ty, &rh));
	GNREASSERT(file->ReadDWordSeq(start, ats_sa, &rh));
	return file->ReadDWordSeq(start, ats_ea, &rh);
	}

//////////////////////////////////////////////////////////////////////
//
//  Generic Program Chain Information
//
//////////////////////////////////////////////////////////////////////

DVDGenericPGCI::DVDGenericPGCI(DVDDiskPlayerFactory * factory)
	{
	this->factory = factory;
	}

DVDGenericPGCI::~DVDGenericPGCI(void)
	{

	}

Error DVDGenericPGCI::GetNumberOfPrograms(WORD & num)
	{
	DWORD d;

	GNREASSERT(file->ReadDWord(start, d, &rh));
	num = (WORD)XTBF(8, 7, d);
	GNRAISE_OK;
	}

Error DVDGenericPGCI::GetNumberOfCells(WORD & num)
	{
	DWORD d;

	GNREASSERT(file->ReadDWord(start, d, &rh));
	num = (WORD)XTBF(0, 8, d);
	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Part of Title
//
//////////////////////////////////////////////////////////////////////

Error DVDPTT::GetNumberOfTitleUnits(WORD & num)
	{
	if (file)
		return file->ReadWord(start, num, &rh);
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}


Error DVDPTT::GetNumberOfPartOfTitle(WORD unit, WORD & num)
	{
	DWORD d1, d2;
	WORD w;

	if (file)
		{
		GNREASSERT(file->ReadWord(start, w, &rh));  // Number of Title Units
		GNREASSERT(file->ReadDWord(start + 8 + 4 * (unit - 1), d1, &rh));

		if  (unit == w)
			{
			GNREASSERT(file->ReadDWord(start + 4, d2, &rh));
			d2++;
			}
		else
			{
			GNREASSERT(file->ReadDWord(start + 8 + 4 * (unit - 1) + 4, d2, &rh));
			}

		num = (WORD)((d2 - d1) / 4);
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}

//
//  Get Program Chain and Program Number from Part Of Title and Title Unit
//

Error DVDPTT::GetPartOfTitlePGC(WORD unit, WORD ptt, WORD & pgc, WORD & pg)
	{
	DWORD d;

	if (file)
		{
		GNREASSERT(file->ReadDWord(start + 8 + 4 * (unit - 1), d, &rh));
		GNREASSERT(file->ReadDWord(start + d + 4 * (ptt - 1), d, &rh));

		pgc = (WORD)XTBF(16, 16, d);
		pg  = (WORD)XTBF(0, 8, d);
		}
	else
		{
		pgc =  0;
		pg = 0;
		}

	GNRAISE_OK;
	}

//
//  Get Part Of Title from PGC, Program and Title
//

Error DVDPTT::GetPartAndTitle(WORD pgcn, WORD pgn, WORD title, WORD & ptt)
	{
	WORD hptt, pnum;
   WORD hpgcn, hpg;

	if (file)
		{
		if (title)
			{
			ptt = 1;
			GNREASSERT(GetNumberOfPartOfTitle(title, pnum));
			for(hptt = 1; hptt <= pnum; hptt++)
				{
				GNREASSERT(GetPartOfTitlePGC(title, hptt, hpgcn, hpg));

				if (hpgcn == pgcn)
					{
					for(;hptt <= pnum; hptt++)
						{
						GNREASSERT(GetPartOfTitlePGC(title, hptt, hpgcn, hpg));

						if (hpgcn != pgcn || hpg > pgn)
							GNRAISE_OK;

						ptt = hptt;
						}

					GNRAISE_OK;
					}

				ptt = hptt;
				}
			}
		else
			ptt = 0;

		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_ALLOCATED);
	}

//
//  Get part of title
//

Error DVDPTT::GetPartOfTitle(WORD title, WORD pgcn, WORD pg, WORD & pptt)
	{
	WORD ptt, pnum, hpgcn, hpg;

	pptt = 1;
	GNREASSERT(GetNumberOfPartOfTitle(title, pnum));

	for (ptt=2; ptt<=pnum; ptt++)
		{
		GNREASSERT(GetPartOfTitlePGC(title, ptt, hpgcn, hpg));

		if (hpgcn == pgcn)
			{
			if (hpg > pg)
				GNRAISE_OK;
			pptt = ptt;
			}
		}

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Title Search Pointer Table (in VMGI or AMGI)
//
//////////////////////////////////////////////////////////////////////

//
// Return number of titles
//

Error DVDSRPT::GetNumberOfTitles(WORD & num)
	{
	return file->ReadWord(start, num, &rh);
	}

//////////////////////////////////////////////////////////////////////
//
//  Parental Management Information Table
//
//////////////////////////////////////////////////////////////////////

Error DVDPTLMAIT::GetNumberOfVTS(WORD & num)
	{
	GNREASSERT(file->ReadWord(start + 2, num, &rh));
	num &= 0x7f;
	GNRAISE_OK;
	}

Error DVDPTLMAIT::GetNumberOfCountries(WORD & num)
	{
	return file->ReadWord(start, num, &rh);
	}

Error DVDPTLMAIT::GetCountryCode(WORD entry, WORD & cc)
	{
	return file->ReadWord(start + 8 + 8 * (entry - 1), cc, &rh);
	}

Error DVDPTLMAIT::GetPTLMAI(WORD countryCode, DVDPTLMAI & ptlmai)
	{
	WORD w, i, numVTS;
	WORD cc;

	GNREASSERT(GetNumberOfCountries(i));
	GNREASSERT(GetNumberOfVTS(numVTS));

	while (i > 1)
		{
		GNREASSERT(GetCountryCode(i, cc));
		if (i == cc)
			break;
		i--;
		}

	GNREASSERT(file->ReadWord(start + 8 + 8 * (i - 1) + 4, w, &rh));

	ptlmai = DVDPTLMAI(file, start + w, numVTS);
	GNRAISE_OK;
	}

////////////////////////////////////////////////////////////////////
//
//  Parental Management Information
//
////////////////////////////////////////////////////////////////////

Error DVDPTLMAI::GetVMGId(WORD level, WORD & id)
	{
	if (!file || level < 1 || level > 8)
		{
		id = 0xffff;
		GNRAISE_OK;
		}
	else
		return file->ReadWord(start + (8-level) * (numVTS + 1) * 2, id, &rh);
	}

Error DVDPTLMAI::GetVTSId(WORD vtsn, WORD level, WORD & id)
	{
	if (!file || level < 1 || level > 8)
		{
		id = 0xffff;
		GNRAISE_OK;
		}
	else
		return file->ReadWord(start + (8-level) * (numVTS + 1 ) * 2 + 2 * vtsn, id, &rh);
	}

//////////////////////////////////////////////////////////////////////
//
//  Object Set
//
//////////////////////////////////////////////////////////////////////

DWORD DVDOBS::uniqueKeys;

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set
//
//////////////////////////////////////////////////////////////////////

//
//  Return video attributes decoded
//

Error DVDVOBS::GetVideoAttributes(VideoStreamFormat & videoStream)
	{
	WORD attr;

	GNREASSERT(GetVideoAttributes(attr));

	//
	//  Get video compression mode
	//

	switch (XTBF(14, 2, attr))
		{
		case 0: videoStream.compressionMode = VCM_MPEG1; break;
		case 1: videoStream.compressionMode = VCM_MPEG2; break;
		default: videoStream.compressionMode = VCM_UNKNOWN; break;
		}

	//
	//  Get video standard
	//

	switch (XTBF(12, 2, attr))
		{
		case 0: videoStream.videoStandard = VSTD_NTSC; break;
		case 1: videoStream.videoStandard = VSTD_PAL; break;
		default: videoStream.videoStandard = VSTD_UNKNOWN; break;
		}

	//
	//  Get aspect ratio
	//

	switch (XTBF(10, 2, attr))
		{
		case 0: videoStream.sourceAspectRatio = DPM_4BY3; break;
		case 3: videoStream.sourceAspectRatio = DPM_16BY9; break;
		default: videoStream.sourceAspectRatio = DPM_NONE; break;
		}

	//
	//  Get display mode on 4 by 3 display
	//

	switch (XTBF(8, 2, attr))
		{
		case 0: videoStream.panScanOn4By3 = videoStream.letterboxedOn4By3 = TRUE; break;
		case 1: videoStream.panScanOn4By3 = TRUE; videoStream.letterboxedOn4By3 = FALSE; break;
		case 2: videoStream.panScanOn4By3 = FALSE; videoStream.letterboxedOn4By3 = TRUE; break;
		default: videoStream.panScanOn4By3 = videoStream.letterboxedOn4By3 = FALSE; break;
		}

	//
	//  Get line 21 information
	//

	switch (XTBF(6, 2, attr))
		{
		case 0: videoStream.line21Mode = L21M_NO_DATA; break;
		case 1: videoStream.line21Mode = L21M_SECOND_FIELD; break;
		case 2: videoStream.line21Mode = L21M_FIRST_FIELD; break;
		case 3: videoStream.line21Mode = L21M_BOTH_FIELDS; break;
		}

	//
	//  Get source picture resolution
	//

	if (videoStream.videoStandard == VSTD_NTSC)
		videoStream.sourceHeight = 480;
	else
		videoStream.sourceHeight = 576;

	switch (XTBF(3, 2, attr))
		{
		case 0: videoStream.sourceWidth = 720; break;
		case 1: videoStream.sourceWidth = 704; break;
		case 2: videoStream.sourceWidth = 352; break;
		case 3: videoStream.sourceWidth = 352; videoStream.sourceHeight >>= 1; break;		// Half height only
		default: videoStream.sourceWidth = videoStream.sourceHeight = 0; break;
		}

	//
	//  Get source letterboxed information
	//

	videoStream.sourceLetterboxed = XTBF(2, attr);

	GNRAISE_OK;
	}

//
//  Get audio stream attributes
//

Error DVDVOBS::GetAudioStreamAttributes(int num, DVDAudioStreamFormat & format)
	{
	WORD attrib;
	WORD language, extension;
	WORD appInfo;

	GNREASSERT(GetAudioStreamAttribute(num, attrib, language, extension, appInfo));

	format.languageCode = language;
	format.languageExtension = extension;

	switch (XTBF(13, 3, attrib))
		{
		case 0: format.codingMode = DAM_AC3;   break;
		case 2: format.codingMode = DAM_MPEG1; break;
		case 3: format.codingMode = DAM_MPEG2; break;
		case 4: format.codingMode = DAM_LPCM;  break;
		case 6: format.codingMode = DAM_DTS;   break;
		case 7: format.codingMode = DAM_SDDS;  break;
		}

	if (format.codingMode == DAM_LPCM)
		format.bitsPerSample = (WORD)XTBF(6,2, attrib) * 4 + 16;
	else
		format.bitsPerSample = 16;

	format.samplesPerSecond = 48000 * (1 + XTBF(4, 2, attrib));
	format.channels = (WORD)XTBF(0, 3, attrib) + 1;

	switch (XTBF(8, 2, attrib))
		{
		case 0: format.applicationMode = DAAM_UNDEFINED;	break;
		case 1: format.applicationMode = DAAM_KARAOKE;		break;
		case 2: format.applicationMode = DAAM_SURROUND;		break;
		default: format.applicationMode = DAAM_UNDEFINED;	break;
		}

	format.channelAssignment = (BYTE)XTBF(4, 3, appInfo);

	if (format.applicationMode == DAAM_KARAOKE)
		{
		format.mcIntro = (BYTE)XTBF(1, 1, appInfo);

		switch (XTBF(0, 1, appInfo))
			{
			case 0: format.soloDuetMode = DKM_SOLO;	break;
			case 1: format.soloDuetMode = DKM_DUET;	break;
			default: format.soloDuetMode = DKM_UNDEFINED;	break;
			}
		}
	else
		{
		format.mcIntro = 0;
		format.soloDuetMode = DKM_UNDEFINED;
		}

	GNRAISE_OK;
	}

//
//  Get sub picture stream attributes
//

Error DVDVOBS::GetSubPictureStreamAttributes(int num, DVDSubPictureStreamFormat & format)
	{
	WORD attrib;
	WORD language;
	WORD extension;

	GNREASSERT(GetSubPictureStreamAttribute(num, attrib, language, extension));

	format.languageCode = language;
	format.languageExtension = extension;

	GNRAISE_OK;
	}

Error DVDVOBS::HasParentalIDs(BOOL & hasIDs)
	{
	WORD id;
	WORD i;

	hasIDs = FALSE;
	for(i=1; i<=8; i++)
		{
		GNREASSERT(GetParentalID(i, id));
		if (id)
			{
			hasIDs = TRUE;
			GNRAISE_OK;
			}
		}

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Object Set for Manager Menu
//
//////////////////////////////////////////////////////////////////////

DVDMGMVOBS::DVDMGMVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai)
	: DVDVOBS(dataFile, ptlmai)
	{
	this->headerFile = headerFile;
	}

DVDMGMVOBS::~DVDMGMVOBS(void)
	{

	}

Error DVDMGMVOBS::GetVideoAttributes(WORD & attrib)
	{
	return headerFile->ReadWord(256, attrib, &rh);
	}

Error DVDMGMVOBS::GetNumberOfSubPictureStreams(WORD & num)
	{
	return headerFile->ReadWord(340, num, &rh);
	}

Error DVDMGMVOBS::GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension)
	{
	GNREASSERT(headerFile->ReadWord(342 + 6 * num, attrib, &rh));
	GNREASSERT(headerFile->ReadWord(342 + 6 * num + 2, language, &rh));
	GNREASSERT(headerFile->ReadWord(342 + 6 * num + 4, extension, &rh));

	extension &= 0xff;

	GNRAISE_OK;
	}

Error DVDMGMVOBS::GetParentalID(WORD level, WORD & id)
	{
	return ptlmai.GetVMGId(level, id);
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Title Set menu VOBS
//
//////////////////////////////////////////////////////////////////////

Error DVDVTSMVOBS::GetVideoAttributes(WORD & attrib)
	{
	return headerFile->ReadWord(256, attrib, &rh);
	}

Error DVDVTSMVOBS::GetNumberOfAudioStreams(WORD & num)
	{
	return headerFile->ReadWord(258, num, &rh);
	}

Error DVDVTSMVOBS::GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo)
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

Error DVDVTSMVOBS::GetNumberOfSubPictureStreams(WORD & num)
	{
	return headerFile->ReadWord(340, num, &rh);
	}

Error DVDVTSMVOBS::GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension)
	{
	GNREASSERT(headerFile->ReadWord(342 + 6 * num, attrib, &rh));
	GNREASSERT(headerFile->ReadWord(342 + 6 * num + 2, language, &rh));
	GNREASSERT(headerFile->ReadWord(342 + 6 * num + 4, extension, &rh));

	extension &= 0xff;

	GNRAISE_OK;
	}

Error DVDVTSMVOBS::GetParentalID(WORD level, WORD & id)
	{
	return ptlmai.GetVTSId(vtsn, level, id);
	}

//////////////////////////////////////////////////////////////////////
//
//  Video Title Set VOBS
//
//////////////////////////////////////////////////////////////////////

Error DVDVTSVOBS::GetVideoAttributes(WORD & attrib)
	{
	return headerFile->ReadWord(512, attrib, &rh);
	}

Error DVDVTSVOBS::GetNumberOfAudioStreams(WORD & num)
	{
	return headerFile->ReadWord(514, num, &rh);
	}

Error DVDVTSVOBS::GetAudioStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension, WORD & applicationInfo)
	{
	GNREASSERT(headerFile->ReadWord(516 + 8 * num, attrib, &rh));
	GNREASSERT(headerFile->ReadWord(516 + 8 * num + 2, language, &rh));
	GNREASSERT(headerFile->ReadWord(516 + 8 * num + 4, extension, &rh));
	GNREASSERT(headerFile->ReadWord(516 + 8 * num + 6, applicationInfo, &rh));

	extension &= 0xff;

	GNRAISE_OK;
	}

Error DVDVTSVOBS::GetNumberOfSubPictureStreams(WORD & num)
	{
	return headerFile->ReadWord(596, num, &rh);
	}

Error DVDVTSVOBS::GetSubPictureStreamAttribute(int num, WORD & attrib, WORD & language, WORD & extension)
	{
	GNREASSERT(headerFile->ReadWord(598 + 6 * num, attrib, &rh));
	GNREASSERT(headerFile->ReadWord(598 + 6 * num + 2, language, &rh));
	GNREASSERT(headerFile->ReadWord(598 + 6 * num + 4, extension, &rh));

	extension &= 0xff;

	GNRAISE_OK;
	}

Error DVDVTSVOBS::GetParentalID(WORD level, WORD & id)
	{
	return ptlmai.GetVTSId(vtsn, level, id);
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Title Set Object Set for Audio Only Title
//
//////////////////////////////////////////////////////////////////////

Error DVDATSAOTTOBS::GetAudioStreamAttribute(int num)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}
