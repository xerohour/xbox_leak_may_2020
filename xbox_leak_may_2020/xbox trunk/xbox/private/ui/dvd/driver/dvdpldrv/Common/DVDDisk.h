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

#ifndef DVDDISK_H
#define DVDDISK_H

#include "DVDFile.h"
#include "DVDHeader.h"
#include "DVDAVHeader.h"


//////////////////////////////////////////////////////////////////////
//
//  DVD Manager Menu Class
//
//	 Description:
//		DVD Manager Menu Class is an abstract class as well for DVD
//		Video Manager Menu as for DVD Audio Manager Menu
//
//////////////////////////////////////////////////////////////////////

class DVDMGM
	{
	protected:
		RequestHandle				rh;
		DVDHeaderFile			*	hfile;
		DVDDataFile				*	dfile;
		DVDPTLMAI					ptlmai;
		DVDFileSystem			*	dvdfs;
		DVDDiskPlayerFactory	*	factory;

	public:
		DVDMGM(DVDDiskPlayerFactory * factory);
		virtual ~DVDMGM(void);

		virtual Error Init(DVDFileSystem * dvdfs, WORD parentalCountryCode) = 0;

		virtual Error Identify(DVDManagerMenuType & mgmType) = 0;
		virtual Error HasMGMVOBS(BOOL & hasMGMVOBS) = 0;
		virtual Error GetMGMVOBS(DVDMGMVOBS * & mgmvobs) = 0;
		virtual Error HasFirstPlayPGC(BOOL & hasFPP) = 0;
		virtual Error GetFirstPlayPGCI(DVDGenericPGCI * & pgci) = 0;
		virtual Error HasMPGCIUT(BOOL & hasMPGCIUT) = 0;
		virtual Error GetMPGCIUT(DVDPGCIUT & pgciut) = 0;
		virtual Error GetSRPT(DVDSRPT * & srpt) = 0;
		virtual Error GetNumberOfTS(WORD & num);
		virtual Error GetPTLMAI(DVDPTLMAI & ptlmai) { ptlmai = this->ptlmai; GNRAISE_OK; }
		virtual Error GetHeaderFile(DVDHeaderFile * & hfile) { hfile = this->hfile; GNRAISE_OK; }
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Title Set Class
//
//	 Description:
//		Abstract class for Video Title Set and Audio Title Set Class.
//
//////////////////////////////////////////////////////////////////////

class DVDTS
	{
	protected:
		DVDDiskPlayerFactory *	factory;
		DVDHeaderFile			*  hfile;
		DVDDataFile				*	mfile;
		DVDDataFile				*	dfile;
		RequestHandle				rh;

	public:
		DVDTS(DVDDiskPlayerFactory * factory);
		virtual ~DVDTS(void);

		virtual Error Init(DVDFileSystem * dvdfs, WORD ts, DVDPTLMAI ptlmai) = 0;

		virtual Error HasPTT(BOOL & hasPTT) = 0;
		virtual Error GetPTT(DVDPTT & ptt) = 0;
		virtual Error HasMPGCIUT(BOOL & hasMPGCIUT) = 0;
		virtual Error GetMPGCIUT(DVDPGCIUT & pgciut) = 0;
		virtual Error GetPGCIT(DVDPGCIT & pgcit) = 0;
		virtual DVDVTSMVOBS * GetVTSMVOBS(void) {return NULL;}
		virtual DVDVTSVOBS * GetVTSVOBS(void) {return NULL;}
		virtual DVDATSAOTTOBS * GetATSAOBS(void) {return NULL;}
		virtual DVDOBS * GetTSOBS(void) = 0;
	};

//////////////////////////////////////////////////////////////////////
//
//  DVD Disk Class
//
//////////////////////////////////////////////////////////////////////

class DVDDisk
	{
	protected:
		DVDDiskPlayerFactory * factory;
		DVDFileSystem	*	diskfs;
		int					currentTSNum;

	public:
		DVDDisk(DVDDiskPlayerFactory * factory);
		virtual ~DVDDisk(void);

		virtual Error	Init(DVDFileSystem * diskfs, WORD parentalCountryCode) = 0;
		virtual DVDMGM * GetMGM(void) = 0;
		virtual Error	GetTS(WORD num, DVDTS * & titleSet) = 0;
		virtual Error	GetTSExclusive(WORD num, DVDTS * & titleSet, BOOL & deleteIt) = 0;
		virtual DVDFileSystem * GetDiskFileSystem(void) { return diskfs; }
	};

#endif

