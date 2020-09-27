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

#include "DVDDisk.h"

//////////////////////////////////////////////////////////////////////
//
//  DVD Manager Menu Class
//
//////////////////////////////////////////////////////////////////////

DVDMGM::DVDMGM(DVDDiskPlayerFactory * factory)
	{
	hfile = NULL;
	dfile = NULL;
	this->factory = factory;
	}

DVDMGM::~DVDMGM(void)
	{
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

//
//  Get number of Title Sets
//

Error DVDMGM::GetNumberOfTS(WORD & num)
	{
	GNREASSERT(hfile->ReadWord(62, num, &rh));
	num = XTBFW(0, 7, num);
	GNRAISE_OK;
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Title Set Definition
//
//////////////////////////////////////////////////////////////////////

DVDTS::DVDTS(DVDDiskPlayerFactory * factory)
	{
	this->factory = factory;
	hfile = NULL;
	mfile = NULL;
	dfile = NULL;
	}

DVDTS::~DVDTS(void)
	{
	if (hfile)
		{
		hfile->Close();
		delete hfile;
		hfile = NULL;
		}

	if (mfile)
		{
		mfile->Close();
		delete mfile;
		mfile = NULL;
		}

	if (dfile)
		{
		dfile->Close();
		delete dfile;
		dfile = NULL;
		}
	}

//////////////////////////////////////////////////////////////////////
//
//  DVD Disk Class Definition
//
//////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DVDDisk::DVDDisk(DVDDiskPlayerFactory * factory)
	{
	this->factory = factory;
	}

//
//  Destructor
//

DVDDisk::~DVDDisk(void)
	{

	}

