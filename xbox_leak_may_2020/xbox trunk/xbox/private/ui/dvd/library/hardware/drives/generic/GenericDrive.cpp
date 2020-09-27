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
//  Generic Drive Classes
//
////////////////////////////////////////////////////////////////////

#include "GenericDrive.h"

////////////////////////////////////////////////////////////////////
//
//  Drive Block Class
//
////////////////////////////////////////////////////////////////////

//
//  Copy data to another drive block
//  WARNING: data is not copied, not the pointer to it
//

void DriveBlock::Copy(DriveBlock * db)
	{
	db->block = block;
	db->lockMode = lockMode;
	db->err = err;
	db->private0 = private0;
	}

////////////////////////////////////////////////////////////////////
//
//  Generic Drive Class
//
////////////////////////////////////////////////////////////////////

//
//  Constructor
//

GenericDrive::GenericDrive(void)
	{
	controlInfo = NULL;
	requestsEnabled = TRUE;
	}

//
//  Destructor
//

GenericDrive::~GenericDrive(void)
	{
	}

//
//  Initialize drive
//

Error GenericDrive::Init(DriveControlInfo * controlInfo, GenericProfile * profile)
	{
	this->controlInfo = controlInfo;
	this->profile = profile;
	GNRAISE_OK;
	}

//
//  Cleanup
//

Error GenericDrive::Cleanup(void)
	{
	controlInfo = NULL;
	GNRAISE_OK;
	}

//
//  Get drive name
//

Error GenericDrive::GetDriveName(KernelString & name)
	{
	VDAutoMutex mutex(&monitorMutex);

	if (controlInfo)
		{
		name = controlInfo->driveName;
		GNRAISE_OK;
		}
	else
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	}

//
//  Get drive capabilities
//
#if MSNDISC
Error GenericDrive::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset)
	{
	type = GDRT_GENERIC;
	caps = GDC_NONE;
	slots = 1;
	positions = 1;
	changerOpenOffset = 0;
	GNRAISE_OK;
	}
#else
Error GenericDrive::GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots)
	{
	type = GDRT_GENERIC;
	caps = GDC_NONE;
	slots = 1;
	GNRAISE_OK;
	}
#endif
//
//  Get drive block size
//

Error GenericDrive::GetCurrentDriveBlockSize(DWORD & size)
	{
	VDAutoMutex mutex(&monitorMutex);
	DWORD minSize, maxSize, step;

	return GetDriveBlockSize(DST_NONE, size, minSize, maxSize, step);
	}

//
//  Get misc info about a block
//

Error GenericDrive::GetBlockInfo(DWORD sectorType, DWORD & blockSize, DWORD & headerSize, DWORD & dataSize)
	{
	switch (sectorType)
		{
		case DST_DVD_ROM:
			blockSize = DVD_BLOCK_SIZE;
			headerSize = 0;
			dataSize = DVD_BLOCK_SIZE;
			break;
		case DST_CDDA:
			blockSize = CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE;
			headerSize = 0;
			dataSize = CD_FRAME_SIZE + CDDA_SUBCHANNEL_SIZE;
			break;
		case DST_CDROM_MODE1:
			blockSize = CD_FRAME_SIZE;
			headerSize = CDROM_FORM1_HEADER;
			dataSize = CDROM_FORM1_DATA;
			break;
		case DST_CDROM_MODE2:
			blockSize = CD_FRAME_SIZE;
			headerSize = CDROM_FORM2_HEADER;
			dataSize = CDROM_FORM2_DATA;
			break;
		case DST_CDROM_MODE2_XA_FORM1:
			blockSize = CD_FRAME_SIZE;
			headerSize = CDROM_XAFORM1_HEADER;
			dataSize = CDROM_XAFORM1_DATA;
			break;
		case DST_CDROM_MODE2_XA_FORM2:
			blockSize = CD_FRAME_SIZE;
			headerSize = CDROM_XAFORM2_HEADER;
			dataSize = CDROM_XAFORM2_DATA;
			break;
		default:
			GNRAISE(GNR_OBJECT_INVALID);
		}

	GNRAISE_OK;
	}


////////////////////////////////////////////////////////////////////
//
//  CDVD Drive
//
////////////////////////////////////////////////////////////////////

//
//  Destructor
//

CDVDDrive::~CDVDDrive(void)
	{
	}


//
//  Do authentication command
//

Error CDVDDrive::DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key)
	{
	VDAutoMutex mutex(&monitorMutex);
	Error err;

//	DP(__TEXT("AP+ : %d %02x%02x%02x%02x%02x.%02x%02x%02x%02x%02x"), com, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], key[8], key[9]);

	switch (com)
		{
		case DAC_START_AUTHENTICATION:
			err = StartAuthentication();
			break;
		case DAC_READ_CHALLENGE_KEY:
			err = GetChallengeKey(key);
			break;
		case DAC_WRITE_BUS_KEY:
			err = SendBusKey(key);
			break;
		case DAC_WRITE_CHALLENGE_KEY:
			err = SendChallengeKey(key);
			break;
		case DAC_READ_BUS_KEY:
			err = GetBusKey(key);
			break;
		case DAC_READ_DISK_KEY:
			err = GetDiskKey(key);
			break;
		case DAC_READ_TITLE_KEY:
			err = GetTitleKey(sector, key);
			break;
		case DAC_COMPLETE_AUTHENTICATION:
			err = CompleteAuthentication();
			break;
		case DAC_CANCEL_AUTHENTICATION:
			err = CancelAuthentication();
			break;
		default:
			err = GNR_INVALID_PARAMETERS;
		}

//	DP(__TEXT("AP- : %d %02x%02x%02x%02x%02x.%02x%02x%02x%02x%02x"), com, key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7], key[8], key[9]);

	return err;
	}


