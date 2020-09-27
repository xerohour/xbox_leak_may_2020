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
//  DVD Audio/Video specific Disk Information
//
////////////////////////////////////////////////////////////////////

#ifndef DVDAVDISK_H
#define DVDAVDISK_H


#include "DVDDisk.h"
#include "DVDAVHeader.h"


//////////////////////////////////////////////////////////////////////
//
//  DVD Video Manager Menu Class
//
//	 Description:
//		The VMGM represents the Title Menu.
//
//////////////////////////////////////////////////////////////////////

class DVDVMGM : public DVDMGM
	{
	protected:
		DVDVMGMVOBS		*	vmgmVobs;

	public:
		DVDVMGM(DVDDiskPlayerFactory * factory);
		virtual ~DVDVMGM(void);

		Error	Init(DVDFileSystem * dvdfs, WORD parentalCountryCode);

		Error Identify(DVDManagerMenuType & mgmType);
		Error	HasFirstPlayPGC(BOOL & hasFPP);
		Error	GetFirstPlayPGCI(DVDGenericPGCI * & pgci);
		Error	HasMPGCIUT(BOOL & hasMPGCIUT);
		Error	GetMPGCIUT(DVDPGCIUT & pgciut);
		Error	GetSRPT(DVDSRPT * & srpt);
		Error HasMGMVOBS(BOOL & hasMGMVOBS);
		Error	GetMGMVOBS(DVDMGMVOBS * & mgmVobs);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Manager Menu Class
//
//	 Description:
//		DVD Audio Mananger Menu Class describes a table of contents for
//		all Audio Title Sets (ATSs) which exist in DVD-Audio zone, and
//		all Video Title Sets (VTSs) for Audio Titles, which exist in
//		DVD-Video zone.
//
//////////////////////////////////////////////////////////////////////

class DVDAMGM : public DVDMGM
	{
	protected:
		DVDAMGMVOBS	*	amgmVobs;

	public:
		DVDAMGM(DVDDiskPlayerFactory * factory);
		virtual ~DVDAMGM(void);

		Error Init(DVDFileSystem * dvdfs, WORD parentalCountryCode);

		Error Identify(DVDManagerMenuType & mgmType);
		Error	HasFirstPlayPGC(BOOL & hasFPP);
		Error	GetFirstPlayPGCI(DVDGenericPGCI * & pgci);
		Error	HasMPGCIUT(BOOL & hasMPGCIUT);
		Error	GetMPGCIUT(DVDPGCIUT & pgciut);
		Error	GetSRPT(DVDSRPT * & srpt);
		Error HasMGMVOBS(BOOL & hasMGMVOBS);
		Error	GetMGMVOBS(DVDMGMVOBS * & mgmvobs);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Title Set Class
//
//	 Description:
//		The Video Title Set Class describes the Video Title Set. This
//		is a collection of Titles and VTSM.
//
//////////////////////////////////////////////////////////////////////

class DVDVTS : public DVDTS
	{
	protected:
		DVDVTSMVOBS		*	vtsmVobs;
		DVDVTSVOBS		*	vtsVobs;

	public:
		DVDVTS(DVDDiskPlayerFactory * factory);
		virtual ~DVDVTS(void);

		Error Init(DVDFileSystem * dvdfs, WORD ts, DVDPTLMAI ptlmai);

		Error HasPTT(BOOL & hasPTT);
		Error GetPTT(DVDPTT & ptt);
		Error HasMPGCIUT(BOOL & hasMPGCIUT);
		Error GetMPGCIUT(DVDPGCIUT & pgciut);
		Error GetPGCIT(DVDPGCIT & pgcit);
		DVDVTSMVOBS * GetVTSMVOBS(void) { return vtsmVobs; }
		DVDVTSVOBS * GetVTSVOBS(void) { return vtsVobs; }
		DVDOBS * GetTSOBS(void) { return vtsVobs; }
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Title Set Class
//
//	 Description:
//		ATS defines the Audio Only Titles (AOTTs) which is defined by
//		the Navigation Data and the Audio Objects (AOBs) in the ATS,
//		or by the Navigation Data in the ATS and the audio part of
//		Video Objects (VOBs) in the VTS.
//
//////////////////////////////////////////////////////////////////////

class DVDATS : public DVDTS
	{
	protected:
		DVDATSAOTTOBS	*	atsAottObs;

	public:
		DVDATS(DVDDiskPlayerFactory * factory);
		virtual ~DVDATS(void);

		Error Init(DVDFileSystem * dvdfs, WORD ts, DVDPTLMAI ptlmai);

		virtual Error HasPTT(BOOL & hasPTT);
		virtual Error GetPTT(DVDPTT & ptt);
		virtual Error HasMPGCIUT(BOOL & hasMPGCIUT);
		virtual Error GetMPGCIUT(DVDPGCIUT & pgciut);
		virtual Error GetPGCIT(DVDPGCIT & pgcit);
		virtual DVDATSAOTTOBS * GetATSAOBS(void) { return atsAottObs; }
		DVDOBS * GetTSOBS(void) { return atsAottObs; }
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Video Disk Class
//
//////////////////////////////////////////////////////////////////////

class DVDVideoDisk : public DVDDisk
	{
	protected:
		DVDVMGM			*  vmgm;
		DVDVTS			*	currentVTS;

	public:
		DVDVideoDisk(DVDDiskPlayerFactory * factory);
		virtual ~DVDVideoDisk(void);

		Error	Init(DVDFileSystem * diskfs, WORD parentalCountryCode);
		DVDMGM * GetMGM(void) { return (DVDMGM *)vmgm; }
		Error	GetTS(WORD num, DVDTS * & titleSet);
		Error	GetTSExclusive(WORD num, DVDTS * & titleSet, BOOL & deleteIt);
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Audio Disk Class
//
//	 Description:
//
//////////////////////////////////////////////////////////////////////

class DVDAudioDisk : public DVDDisk
	{
	protected:
		DVDAMGM			*  amgm;
		DVDATS			*	currentATS;

	public:
		DVDAudioDisk(DVDDiskPlayerFactory * factory);
		virtual ~DVDAudioDisk(void);

		Error	Init(DVDFileSystem * diskfs, WORD parentalCountryCode);
		DVDMGM * GetMGM(void) { return (DVDMGM *)amgm; }
		Error	GetTS(WORD num, DVDTS * & titleSet);
		Error	GetTSExclusive(WORD num, DVDTS * & titleSet, BOOL & deleteIt);
	};


#endif // DVDAVDISK_H
