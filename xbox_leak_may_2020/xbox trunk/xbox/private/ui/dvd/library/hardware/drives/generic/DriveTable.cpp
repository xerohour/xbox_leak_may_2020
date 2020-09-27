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
//  Operating system dependent drive table classes
//
////////////////////////////////////////////////////////////////////

#include "DriveTable.h"


////////////////////////////////////////////////////////////////////
//
//  Drive table base class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

DriveTable::DriveTable(void)
	{
	}

//
//  Destructor
//

DriveTable::~DriveTable(void)
	{
	}

//
//  Cleanup
//

Error DriveTable::Cleanup(void)
	{
	DriveDesc * driveDesc;

	while (!drives.IsEmpty())
		{
		driveDesc = (DriveDesc*)drives.Pop();
		driveDesc->drive->Cleanup();
		delete driveDesc->drive;
		delete driveDesc;
		}

	GNRAISE_OK;
	}

//
//  Move iterator to first drive
//

Error DriveTable::GoFirstDrive(DriveIterator & it)
	{
	it.driveDesc = (DriveDesc*)(drives.First());
	if (it.driveDesc)
		GNRAISE_OK;
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Move iterator to next drive
//

Error DriveTable::GoNextDrive(DriveIterator & it)
	{
	if (!it.driveDesc->IsLast())
		{
		it.driveDesc = (DriveDesc*)it.driveDesc->Succ();
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Test if iterator points to last drive
//

BOOL DriveTable::IsLast(const DriveIterator & it)
	{
	if (it.driveDesc)
		return it.driveDesc->IsLast();
	else
		return TRUE;
	}

//
//  Return pointer to drive
//

Error DriveTable::GetDrive(const DriveIterator & it, GenericDrive * & gd)
	{
	if (it.driveDesc)
		{
		gd = it.driveDesc->drive;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Return drive name
//

Error DriveTable::GetDriveName(const DriveIterator & it, KernelString & driveName)
	{
	if (it.driveDesc)
		{
		driveName = it.driveDesc->driveName;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}



Error WinDriveTable::CreatePotentialDrives()
	{
	DriveDesc * driveDesc;

	driveDesc = new DriveDesc(NULL, "D");
	drives.InsertLast(driveDesc);

	GNRAISE_OK;
	}


////////////////////////////////////////////////////////////////////
//
//  Win 9x Drive Table
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//


////////////////////////////////////////////////////////////////////
//
//  Win 2K Classes
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

W2KDriveTable::W2KDriveTable(void)
	{
	}

//
//  Destructor
//

W2KDriveTable::~W2KDriveTable(void)
	{
	Cleanup();
	}

//
//  Initialize drive table
//

Error W2KDriveTable::Init(GenericProfile * profile)
	{
	DriveDesc * driveDesc, * dd;

	//
	//  Determine potential candidates
	//

	GNREASSERT(CreatePotentialDrives());
	driveDesc = (DriveDesc*)drives.First();

	//
	//  Now try all potential drives
	//

	while (driveDesc)
		{
		driveDesc->drive = new XBOXDVDDrive();
		DriveControlInfo * pControlInfo = new DriveControlInfo(driveDesc->driveName);
		driveDesc->StoreDriveControlInfo(pControlInfo);
		if (IS_ERROR(driveDesc->drive->Init(pControlInfo, profile)))
			{
			dd = (DriveDesc*) driveDesc->Succ();
			drives.Remove(driveDesc);
			delete driveDesc;
			driveDesc = dd;
			}
		else
			{
			driveDesc = (DriveDesc*) driveDesc->Succ();
			}
		}

	if (drives.Num() == 0)
		GNRAISE(GNR_READ_ERROR);

	GNRAISE_OK;
	}

//
//  Cleanup drive table
//

Error W2KDriveTable::Cleanup(void)
	{
	return DriveTable::Cleanup();
	}



////////////////////////////////////////////////////////////////////
//
//  ST20LITE Classes
//
////////////////////////////////////////////////////////////////////

