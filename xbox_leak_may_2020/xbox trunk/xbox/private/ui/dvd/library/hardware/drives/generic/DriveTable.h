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

#ifndef DRIVETABLE_H
#define DRIVETABLE_H

#include "Library/Common/Prelude.h"
#include "Library/Common/krnlstr.h"
#include "Library/general/lists.h"
#include "Library/hardware/drives/generic/GenericDrive.h"

#include "Library/hardware/drives/specific/XBOXDVDDrive.h"

////////////////////////////////////////////////////////////////////
//
//  Generic Drive Table
//
////////////////////////////////////////////////////////////////////

//
//  Description of a drive
//

class DriveDesc : public Node
	{
	public:
		GenericDrive	*	drive;		// The drive itself
		KernelString		driveName;
		//DriveDesc class holds pointer to the instance of DriveControlInfo-based objects.
		//DriveControlInfo-based object is created outside DriveDesc, but DriveDesc is responsible for deleting it.
		DriveControlInfo* driveControlInfo;

		//
		//  Methods
		//

		DriveDesc(GenericDrive * drive, const KernelString & driveName) : Node()
			{ this->drive = drive; this->driveName = driveName; this->driveControlInfo = NULL;}
		virtual ~DriveDesc(void) {if (driveControlInfo) delete driveControlInfo;}

		void StoreDriveControlInfo(DriveControlInfo* driveControlInfo){this->driveControlInfo = driveControlInfo;}
	};

//
//  Iterator over drive table
//

class DriveIterator
	{
	public:
		DriveDesc * driveDesc;

		DriveIterator(void) { driveDesc = NULL; }
		BOOL Valid(void) { return driveDesc != NULL; }
	};

//
//  Drive table base class
//

class DriveTable
	{
	protected:
		List drives;
		DWORD potentialDriveMask;

	public:
		DriveTable(void);
		virtual ~DriveTable(void);

		virtual Error Init(GenericProfile * profile) = 0;
		virtual Error Cleanup(void);

		virtual Error GoFirstDrive(DriveIterator & it);
		virtual Error GoNextDrive(DriveIterator & it);
		virtual BOOL  IsLast(const DriveIterator & it);
		virtual Error GetDrive(const DriveIterator & it, GenericDrive * & gd);
		virtual Error GetDriveName(const DriveIterator & it, KernelString & driveName);
	};


////////////////////////////////////////////////////////////////////
//
//  Windows drive table
//
////////////////////////////////////////////////////////////////////

class WinDriveTable : public DriveTable
	{
	protected:
		Error CreatePotentialDrives(void);
	};

////////////////////////////////////////////////////////////////////
//
//  Win 2K Drive Table
//
////////////////////////////////////////////////////////////////////

//
//  W2K drive table
//

class W2KDriveTable : public WinDriveTable
	{
	public:
		W2KDriveTable(void);
		virtual ~W2KDriveTable(void);

		virtual Error Init(GenericProfile * profile);
		virtual Error Cleanup(void);
	};


////////////////////////////////////////////////////////////////////
//
//  ST20LITE Drive Table
//
////////////////////////////////////////////////////////////////////


#endif
