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

#include "DVDAVDisk.h"
#include "DVDAVHeader.h"


//////////////////////////////////////////////////////////////////////
//
//  DVD Video Manager Menu Definition
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDVMGM::DVDVMGM(DVDDiskPlayerFactory * factory) : DVDMGM(factory)
	{
	vmgmVobs = NULL;
	}

//
//  Destructor
//

DVDVMGM::~DVDVMGM(void)
	{
	delete vmgmVobs;
	}

//
//  Initialize
//

Error DVDVMGM::Init(DVDFileSystem * dvdfs, WORD parentalCountryCode)
	{
	GenericDiskItem * gdi;
	DWORD	d;

	this->dvdfs = dvdfs;
	if (IS_ERROR(dvdfs->OpenItem("video_ts\\video_ts.ifo", FAT_HEADER, gdi)))
		GNREASSERT(dvdfs->OpenItem("video_ts\\video_ts.inf", FAT_HEADER, gdi));

	hfile = (DVDHeaderFile*)gdi;
	GNREASSERT(hfile->ReadDWord(204, d, &rh));

	if (d)
		{
		DVDPTLMAIT ptlmait = DVDPTLMAIT(hfile, d * DVD_BLOCK_SIZE);
		return ptlmait.GetPTLMAI(parentalCountryCode, ptlmai);
		}

	GNRAISE_OK;
	}

//
// Identifies the Manager Menu. Just returning VMGM should be enough.
// The Video Management Table contains an identifier describing
//	"DVDVIDEO-VMG" in the first 12 bytes. This is currently not
//	considered.
//

Error DVDVMGM::Identify(DVDManagerMenuType & mgmType)
	{
	/*
	DWORD d[3];

	GNREASSERT(hfile->ReadDWord(0, d[0], &rh));
	GNREASSERT(hfile->ReadDWord(4, d[1], &rh));
	GNREASSERT(hfile->ReadDWord(8, d[2], &rh));
	*/

	mgmType = DMT_VMGM;

	GNRAISE_OK;
	}

//
// Returns always TRUE because DVD-Video always contains
// a Video Manager Menu (in contrast to DVD-Audio)
//

Error DVDVMGM::HasMGMVOBS(BOOL & hasMGMVOBS)
	{
	// DVD-Video always returns TRUE
	hasMGMVOBS = TRUE;

	GNRAISE_OK;
	}

//
//  Return MGMVOBS
//

Error DVDVMGM::GetMGMVOBS(DVDMGMVOBS * & mgmVobs)
	{
	GenericDiskItem * gdi;

	if (!this->vmgmVobs)
		{
		if (!IS_ERROR(dvdfs->OpenItem("video_ts\\video_ts.vob", FAT_NONE, gdi)))
			dfile = (DVDDataFile*)gdi;
		else
			dfile = NULL;

		this->vmgmVobs = new DVDVMGMVOBS(hfile, ptlmai, dfile);
		}

	mgmVobs = (DVDMGMVOBS *)this->vmgmVobs;

	GNRAISE_OK;
	}

//
//  Test on First Play PGC
//

Error DVDVMGM::HasFirstPlayPGC(BOOL & hasFPP)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(132, d, &rh));
	hasFPP = d != 0;
	GNRAISE_OK;
	}

//
//  Get First Play PGCI
//

Error DVDVMGM::GetFirstPlayPGCI(DVDGenericPGCI * & pgci)
	{
	Error err;
	DWORD d;

	GNREASSERT(hfile->ReadDWord(132, d, &rh));

	pgci = new DVDPGCI(factory);
	if (IS_ERROR(err = pgci->Init(hfile, d)))
		delete pgci;
	GNRAISE(err);
	}

//
//  Test on MPGCIUT
//

Error DVDVMGM::HasMPGCIUT(BOOL & hasMPGCIUT)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(200, d, &rh));

	hasMPGCIUT = d != 0;
	GNRAISE_OK;
	}

//
//  Return MPGCIUT
//

Error DVDVMGM::GetMPGCIUT(DVDPGCIUT & pgciut)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(200, d, &rh));

	pgciut = DVDPGCIUT(factory, hfile, d * DVD_BLOCK_SIZE);
	GNRAISE_OK;
	}

//
//  Get SRPT
//

Error DVDVMGM::GetSRPT(DVDSRPT * & srpt)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(196, d, &rh));
	srpt = (DVDSRPT *)new DVDVTTSRPT(hfile, d * DVD_BLOCK_SIZE);

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Manager Menu Class
//
//////////////////////////////////////////////////////////////////////

DVDAMGM::DVDAMGM(DVDDiskPlayerFactory * factory) : DVDMGM(factory)
	{
	amgmVobs = NULL;
	}

DVDAMGM::~DVDAMGM(void)
	{
	delete amgmVobs;
	}

Error DVDAMGM::Init(DVDFileSystem * dvdfs, WORD parentalCountryCode)
	{
	GenericDiskItem * gdi;

	this->dvdfs = dvdfs;
	if (IS_ERROR(dvdfs->OpenItem("audio_ts\\audio_ts.ifo", FAT_HEADER, gdi)))
		GNREASSERT(dvdfs->OpenItem("audio_ts\\audio_ts.inf", FAT_HEADER, gdi));

	hfile = (DVDHeaderFile*)gdi;

	GNRAISE_OK;
	}

//
// Identifies the Manager Menu. Just returning VMGM should be enough.
// The Audio Management Table contains an identifier describing
//	"DVDAUDIO-VMG" in the first 12 bytes. This is currently not
//	considered.
//

Error DVDAMGM::Identify(DVDManagerMenuType & mgmType)
	{
	/*
	DWORD d[3];

	GNREASSERT(hfile->ReadDWord(0, d[0], &rh));
	GNREASSERT(hfile->ReadDWord(4, d[1], &rh));
	GNREASSERT(hfile->ReadDWord(8, d[2], &rh));
	*/

	mgmType = DMT_AMGM;

	GNRAISE_OK;
	}

Error	DVDAMGM::HasFirstPlayPGC(BOOL & hasFPP)
	{
	// always returns TRUE. Audio Manager Menu is defined as one PGC.
	// No First Play PGC detection is necessary.
	hasFPP = TRUE;

	GNRAISE_OK;
	}

Error	DVDAMGM::GetFirstPlayPGCI(DVDGenericPGCI * & pgci)
	{
	pgci = NULL;

	GNRAISE_OK;
	}

Error	DVDAMGM::HasMPGCIUT(BOOL & hasMPGCIUT)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(204, d, &rh));

	hasMPGCIUT = d != 0;
	GNRAISE_OK;
	}

Error	DVDAMGM::GetMPGCIUT(DVDPGCIUT & pgciut)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(204, d, &rh));

	pgciut = DVDPGCIUT(factory, hfile, d * DVD_BLOCK_SIZE);
	GNRAISE_OK;
	}

//
// Returns TRUE if DVD-Audio disc has an Audio Manager Menu
// else returns FALSE
//

Error DVDAMGM::HasMGMVOBS(BOOL & hasMGMVOBS)
	{
	GenericDiskItem * gdi;

	if (!IS_ERROR(dvdfs->OpenItem("audio_ts\\audio_ts.vob", FAT_NONE, gdi)))
		{
		hasMGMVOBS = TRUE;
		delete gdi;
		}
	else
		hasMGMVOBS = FALSE;

	GNRAISE_OK;
	}

Error DVDAMGM::GetMGMVOBS(DVDMGMVOBS * & mgmvobs)
	{
	GenericDiskItem * gdi;

	if (!this->amgmVobs)
		{
		if (!IS_ERROR(dvdfs->OpenItem("audio_ts\\audio_ts.vob", FAT_NONE, gdi)))
			{
			dfile = (DVDDataFile*)gdi;
			this->amgmVobs = new DVDAMGMVOBS(hfile, dfile, ptlmai);
			}
		else
			{
			dfile = NULL;
			this->amgmVobs = NULL;
			}
		}

	mgmvobs = (DVDMGMVOBS *)this->amgmVobs;

	GNRAISE_OK;
	}

Error DVDAMGM::GetSRPT(DVDSRPT * & srpt)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(196, d, &rh));
	srpt = (DVDSRPT *)new DVDATTSRPT(hfile, d * DVD_BLOCK_SIZE);

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Title Set Definition
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDVTS::DVDVTS(DVDDiskPlayerFactory * factory) : DVDTS(factory)
	{
	vtsmVobs = NULL;
	vtsVobs = NULL;
	}

//
//  Destructor
//

DVDVTS::~DVDVTS(void)
	{
	delete vtsmVobs;
	delete vtsVobs;
	}

//
//  Initialize
//

Error DVDVTS::Init(DVDFileSystem * dvdfs, WORD ts, DVDPTLMAI ptlmai)
	{
	GenericDiskItem * gdi;
	KernelString s;

	s = KernelString("video_ts\\vts_") + KernelString(ts, 2) + KernelString("_0.ifo");

	if (IS_ERROR(dvdfs->OpenItem(s, FAT_HEADER, gdi)))
		{
		s = KernelString("video_ts\\vts_") + KernelString(ts, 2) + KernelString("_0.inf");
		GNREASSERT(dvdfs->OpenItem(s, FAT_HEADER, gdi));
		}
	hfile = (DVDHeaderFile*)gdi;

	s = KernelString("video_ts\\vts_") + KernelString(ts, 2) + KernelString("_0.vob");
	if (!IS_ERROR(dvdfs->OpenItem(s, FAT_NONE, gdi)))
		{
		mfile = (DVDDataFile*)gdi;
		}
	else
		{
		mfile = NULL;
		}

	s = KernelString("video_ts\\vts_") + KernelString(ts, 2) + KernelString("_1.vob");
	if (!IS_ERROR(dvdfs->OpenItem(s, FAT_CHAIN, gdi)))
		{
		dfile = (DVDDataFile*)gdi;
		}
	else
		{
		dfile = NULL;
		}

	vtsmVobs = new DVDVTSMVOBS(hfile, ptlmai, mfile, ts);
	vtsVobs = new DVDVTSVOBS(hfile, ptlmai, dfile, ts);

	GNRAISE_OK;
	}

//
// Check if we have a Part of Title Table
// Always TRUE for Video Title Set
//

Error DVDVTS::HasPTT(BOOL & hasPTT)
	{
	hasPTT = TRUE;

	GNRAISE_OK;
	}

//
//  Get Part of Title
//

Error DVDVTS::GetPTT(DVDPTT & ptt)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(200, d, &rh));
	ptt = DVDPTT(hfile, d * DVD_BLOCK_SIZE);
	GNRAISE_OK;
	}

//
//  Check if we have a Menu PGC Information Table
//

Error DVDVTS::HasMPGCIUT(BOOL & hasMPGCIUT)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(208, d, &rh));
	hasMPGCIUT = d != 0;
	GNRAISE_OK;
	}

//
//  Get Menu PGC Information Table
//

Error DVDVTS::GetMPGCIUT(DVDPGCIUT & pgciut)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(208, d, &rh));
	pgciut = DVDPGCIUT(factory, hfile, d * DVD_BLOCK_SIZE);
	GNRAISE_OK;
	}

//
//  Get PGC Information Table
//

Error DVDVTS::GetPGCIT(DVDPGCIT & pgcit)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(204, d, &rh));
	pgcit = DVDPGCIT(factory, hfile, d * DVD_BLOCK_SIZE);
	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Title Set Class
//
//////////////////////////////////////////////////////////////////////

DVDATS::DVDATS(DVDDiskPlayerFactory * factory) : DVDTS(factory)
	{
	atsAottObs = NULL;
	}

DVDATS::~DVDATS(void)
	{
	if (atsAottObs)
		{
		delete atsAottObs;
		atsAottObs = NULL;
		}
	}

Error DVDATS::Init(DVDFileSystem * dvdfs, WORD ts, DVDPTLMAI ptlmai)
	{
	GenericDiskItem * gdi;
	KernelString s;

	s = KernelString("audio_ts\\ats_") + KernelString(ts, 2) + KernelString("_0.ifo");

	if (IS_ERROR(dvdfs->OpenItem(s, FAT_HEADER, gdi)))
		{
		s = KernelString("audio_ts\\ats_") + KernelString(ts, 2) + KernelString("_0.inf");
		GNREASSERT(dvdfs->OpenItem(s, FAT_HEADER, gdi));
		}
	hfile = (DVDHeaderFile*)gdi;

	// Open first Audio Object Set for Titles
	s = KernelString("audio_ts\\ats_") + KernelString(ts, 2) + KernelString("_1.aob");
	if (!IS_ERROR(dvdfs->OpenItem(s, FAT_NONE, gdi)))
		{
		dfile = (DVDDataFile*)gdi;
		}
	else
		{
		dfile = NULL;
		}

	// DVD-Audio doesn't contain a Title Set Menu!
	mfile = NULL;

	atsAottObs = new DVDATSAOTTOBS(hfile, dfile);

	GNRAISE_OK;
	}

//
// Check if we have a Part of Title Information Table
//	Not available for Audio Title Set
//

Error DVDATS::HasPTT(BOOL & hasPTT)
	{
	hasPTT = FALSE;

	GNRAISE_OK;
	}

//
//  Get Part of Title
//	 Not available for Audio Title Set
//

Error DVDATS::GetPTT(DVDPTT & ptt)
	{
	GNRAISE_OK;
	}

//
//  Check if we have a Menu PGC Information Table
//	 Not available for Audio Title Set
//

Error DVDATS::HasMPGCIUT(BOOL & hasMPGCIUT)
	{
	hasMPGCIUT = FALSE;

	GNRAISE_OK;
	}

//
//  Get Menu PGC Information Table
//	 Not available for Audio Title Set
//

Error DVDATS::GetMPGCIUT(DVDPGCIUT & pgciut)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

//
//  Get PGC Information Table
//

Error DVDATS::GetPGCIT(DVDPGCIT & pgcit)
	{
	DWORD d;

	GNREASSERT(hfile->ReadDWord(204, d, &rh));
	pgcit = DVDPGCIT(factory, hfile, d * DVD_BLOCK_SIZE, TRUE);

	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Disk Class Definition
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDVideoDisk::DVDVideoDisk(DVDDiskPlayerFactory * factory) : DVDDisk(factory)
	{
	vmgm = NULL;
	currentVTS = NULL;
	}

//
//  Destructor
//

DVDVideoDisk::~DVDVideoDisk(void)
	{
	if (vmgm)
		{
		delete vmgm;
		vmgm = NULL;
		}

	if (currentVTS)
		{
		delete currentVTS;
		currentVTS = NULL;
		}
	}

//
//  Initialize
//

Error DVDVideoDisk::Init(DVDFileSystem * diskfs, WORD parentalCountryCode)
	{
	this->diskfs = diskfs;
	vmgm = new DVDVMGM(factory);
	if (!vmgm)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	return vmgm->Init(diskfs, parentalCountryCode);
	}

//
//  Return pointer to a certain Video Title Set (Cached!!!)
//

Error DVDVideoDisk::GetTS(WORD num, DVDTS * & titleSet)
	{
	Error err;

	titleSet = NULL;

	//
	//  Return current VTS if that is the one requested, otherwise delete it
	//

	if (currentVTS != NULL)
		{
		if (num == currentTSNum)
			{
			titleSet = (DVDTS *)currentVTS;
			GNRAISE_OK;
			}
		else
			{
			delete currentVTS;
			currentVTS = NULL;
			}
		}

	//
	//  Create new VTS if there is none
	//

	if (currentVTS == NULL)
		{
		DVDPTLMAI ptlmai;

		GNREASSERT(vmgm->GetPTLMAI(ptlmai));
		currentVTS = new DVDVTS(factory);
		if (!currentVTS)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		if (IS_ERROR(err = currentVTS->Init(diskfs, num, ptlmai)))
			{
			delete currentVTS;
			currentVTS = NULL;
			GNRAISE(err);
			}
		currentTSNum = num;
		}

	titleSet = (DVDTS *)currentVTS;
	GNRAISE_OK;
	}

//
//  Return pointer to a certain Video Title Set (must be delete by caller)
//

Error DVDVideoDisk::GetTSExclusive(WORD num, DVDTS * & titleSet, BOOL & deleteIt)
	{
	DVDPTLMAI ptlmai;
	Error err;

	//
	//  Return current VTS if that is the one requested
	//

	if (currentVTS != NULL && num == currentTSNum)
		{
		titleSet = (DVDTS *)currentVTS;
		deleteIt = FALSE;
		GNRAISE_OK;
		}

	//
	//  Create new VTS if it is a different one
	//

	deleteIt = TRUE;
	GNREASSERT(vmgm->GetPTLMAI(ptlmai));

	titleSet = new DVDVTS(factory);
	if (!titleSet)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	if (IS_ERROR(err = titleSet->Init(diskfs, num, ptlmai)))
		{
		delete titleSet;
		titleSet = NULL;
		}

	GNRAISE(err);
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Disk Class Definition
//
//////////////////////////////////////////////////////////////////////

DVDAudioDisk::DVDAudioDisk(DVDDiskPlayerFactory * factory) : DVDDisk(factory)
	{
	amgm = NULL;
	currentATS = NULL;
	}

DVDAudioDisk::~DVDAudioDisk(void)
	{
	if (currentATS)
		{
		delete currentATS;
		currentATS = NULL;
		}

	if (amgm)
		{
		delete amgm;
		amgm = NULL;
		}
	}

Error	DVDAudioDisk::Init(DVDFileSystem * diskfs, WORD parentalCountryCode)
	{
	this->diskfs = diskfs;
	amgm = new DVDAMGM(factory);
	if (!amgm)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	return amgm->Init(diskfs, parentalCountryCode);
	}

Error	DVDAudioDisk::GetTS(WORD num, DVDTS * & titleSet)
	{
	Error err;

	titleSet = NULL;

	//
	//  Return current TS if that is the one requested, otherwise delete it
	//

	if (currentATS != NULL)
		{
		if (num == currentTSNum)
			{
			titleSet = (DVDTS *)currentATS;
			GNRAISE_OK;
			}
		else
			{
			delete currentATS;
			currentATS = NULL;
			}
		}

	//
	//  Create new VTS if there is none
	//

	if (currentATS == NULL)
		{
		DVDPTLMAI ptlmai;

		GNREASSERT(amgm->GetPTLMAI(ptlmai));
		currentATS = new DVDATS(factory);
		if (!currentATS)
			GNRAISE(GNR_NOT_ENOUGH_MEMORY);
		if (IS_ERROR(err = currentATS->Init(diskfs, num, ptlmai)))
			{
			delete currentATS;
			currentATS = NULL;
			GNRAISE(err);
			}
		currentTSNum = num;
		}

	titleSet = (DVDTS *)currentATS;

	GNRAISE_OK;
	}

Error	DVDAudioDisk::GetTSExclusive(WORD num, DVDTS * & titleSet, BOOL & deleteIt)
	{
	DVDPTLMAI ptlmai;
	Error err;

	//
	//  Return current VTS if that is the one requested
	//

	if (currentATS != NULL && num == currentTSNum)
		{
		titleSet = (DVDTS *)currentATS;
		deleteIt = FALSE;
		GNRAISE_OK;
		}

	//
	//  Create new VTS if it is a different one
	//

	deleteIt = TRUE;
	GNREASSERT(amgm->GetPTLMAI(ptlmai));

	titleSet = new DVDATS(factory);
	if (!titleSet)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	if (IS_ERROR(err = titleSet->Init(diskfs, num, ptlmai)))
		{
		delete titleSet;
		titleSet = NULL;
		}

	GNRAISE(err);
	}
