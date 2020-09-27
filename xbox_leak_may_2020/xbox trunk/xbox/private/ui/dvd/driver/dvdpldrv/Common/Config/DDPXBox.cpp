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
//  DVDPLD32: Entry Point For XBox
//
////////////////////////////////////////////////////////////////////

#include "driver/dvdpldrv/common/dvdpld32.h"
#include "library/common/profiles.h"
#include "library/hardware/drives/generic/drivetable.h"

////////////////////////////////////////////////////////////////////
//
//  Helper Functions
//
////////////////////////////////////////////////////////////////////
//
// Tells us what kind of entry it is
//

enum SetupEntryType
	{
	SET_VALUE,
	SET_SECTION,
	SET_END
	};

//
// One entry of the setup array
//

struct SetupEntry
	{
	char				* name;
	SetupEntryType	type;
	DWORD				value;
	};


static SetupEntry config[] =
	{
		{"$", SET_SECTION, 0},

			{"DRIVE", SET_SECTION, 0},
			{"", SET_END, 0},

		{"", SET_END, 0},
	};

static DWORD CalculateID(BOOL section, KernelString name)
	{
	DWORD temp = 0;

	if (name.Length() > 0)
		{
		for (int i = 0; i < name.Length(); i++)
			{
			temp ^= name[i];
			temp = (temp << 3) | (temp >> 29);	// The first 10 characters count
			}

		return (section ? (temp |= 1) : (temp &= ~1));
		}
	else
		return 0;
	}

//
//  Parse Setup
//

static void ParseSetup(SetupEntry * & sentry, ProfileEntry * pentry, int & num)
	{
	int snum;

	num = 0;
	pentry->id = CalculateID(TRUE, sentry->name);
	num++;
	sentry++;
	while (sentry->type != SET_END)
		{
		if (sentry->type == SET_VALUE)
			{
			pentry[num].id = CalculateID(FALSE, sentry->name);
			pentry[num].value = sentry->value;
			num++;
			sentry++;
			}
		else
			{
			ParseSetup(sentry, pentry + num, snum);
			num += snum;
			}
		}
	pentry->value = num;

	sentry++;
	}

////////////////////////////////////////////////////////////////////
//
//  DDP_Initialize
//
////////////////////////////////////////////////////////////////////

DLLCALL Error WINAPI DDP_Initialize(void)
	{
	GenericProfile * profile;
	DriveTable * driveTable;
	int num;
	Error err;

	static ProfileEntry	pentry[200];

	SetupEntry	*	se = config;

	ParseSetup(se, pentry, num);

	profile = new StaticProfile(pentry);

	if (!profile)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	driveTable = new W2KDriveTable();
	if (IS_ERROR(err = driveTable->Init(profile)))
		{
		delete driveTable;
		driveTable = NULL;
		GNRAISE(err);
		}

	//
	//  Perform general initializations
	//

	GNREASSERT(DDP_Init(new WinPortServer(), profile, driveTable,
							  new DVDVideoDiskPlayerFactory(),
							  new DVDAudioDiskPlayerFactory(),
							  new VCDDiskPlayerFactory(),
							  new CDDADiskPlayerFactory(),
							  new AVFDiskPlayerFactory(),
							  new DVDFileSystemFactory(),
							  new FileSystemFactory(),
							  new FileSystemFactory(),
							  new FileSystemFactory(),
							  new FileSystemFactory()));

	GNRAISE_OK;
	}
