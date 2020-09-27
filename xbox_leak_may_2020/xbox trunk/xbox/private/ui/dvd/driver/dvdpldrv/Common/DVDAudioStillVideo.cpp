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
//  DVD Audio Still Video Module
//
////////////////////////////////////////////////////////////////////

#include "DVDAudioStillVideo.h"

////////////////////////////////////////////////////////////////////
//
//  DVD Video Object Set for Audio Still Video Set Class
//
////////////////////////////////////////////////////////////////////

DVDASVOBS::DVDASVOBS(DVDHeaderFile * headerFile, DVDDataFile * dataFile, const DVDPTLMAI & ptlmai)
	: DVDVOBS(dataFile, ptlmai)
	{
	this->headerFile = headerFile;
	}

DVDASVOBS::~DVDASVOBS(void)
	{

	}

Error DVDASVOBS::GetNumberOfStillVideosInUnit(int item, BYTE & num)
	{
	GNREASSERT(headerFile->ReadByte(96 + 8 * item, num, &rh));

	GNRAISE_OK;
	}

Error DVDASVOBS::GetStillVideoAttribute(int item, WORD & attrib)
	{
	return headerFile->ReadWord(24 + 2 * item, attrib, &rh);
	}

////////////////////////////////////////////////////////////////////
//
//  DVD Audio Still Video Set Class
//
////////////////////////////////////////////////////////////////////

DVDASVS::DVDASVS(void)
	{
	hfile = NULL;
	dfile = NULL;
	asVobs = NULL;
	}

DVDASVS::~DVDASVS(void)
	{
	if (asVobs)
		{
		delete asVobs;
		asVobs = NULL;
		}

	if (hfile)
		{
		hfile->Close();
		delete hfile;
		hfile = NULL;
		}

	if (dfile)
		{
		dfile->Close();
		delete dfile;
		dfile = NULL;
		}
	}

Error DVDASVS::Init(DVDFileSystem * dvdfs)
	{
	GenericDiskItem * gdi;

	this->dvdfs = dvdfs;
	if (IS_ERROR(dvdfs->OpenItem("audio_ts\\audio_sv.ifo", FAT_HEADER, gdi)))
		GNREASSERT(dvdfs->OpenItem("audio_ts\\audio_sv.inf", FAT_HEADER, gdi));

	hfile = (DVDHeaderFile*)gdi;

	GNRAISE_OK;
	}

Error DVDASVS::GetASVOBS(DVDASVOBS * & asVobs)
	{
	GenericDiskItem * gdi;

	if (!this->asVobs)
		{
		if (!IS_ERROR(dvdfs->OpenItem("audio_ts\\audio_sv.vob", FAT_NONE, gdi)))
			dfile = (DVDDataFile*)gdi;
		else
			dfile = NULL;

		this->asVobs = new DVDASVOBS(hfile, dfile, ptlmai);
		}

	asVobs = this->asVobs;

	GNRAISE_OK;
	}

Error DVDASVS::GetNumberOfASVU(WORD & num)
	{
	GNREASSERT(hfile->ReadWord(12, num, &rh));
	num = XTBFW(0, 7, num);

	GNRAISE_OK;
	}




