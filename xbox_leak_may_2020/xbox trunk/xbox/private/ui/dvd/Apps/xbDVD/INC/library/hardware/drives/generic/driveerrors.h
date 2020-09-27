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
//  Error Declaration for Drive Classes
//
////////////////////////////////////////////////////////////////////


#ifndef DRIVEERRORS_H
#define DRIVEERRORS_H

#include "library\common\gnerrors.h"

#define GNR_DRIVE_FAILURE							MKERR(ERROR,	DISK,			HARDWARE,			0x00)
// General drive failure

#define GNR_DRIVE_FATAL_ERROR						MKERR(ERROR,	DISK,			HARDWARE,			0x01)
// Drive firmware is not responding any more

#define GNR_NO_VALID_DISK							MKERR(ERROR,	DISK,			OBJECT,				0x00)
// No valid/known disk was found

#define GNR_NO_DRIVE									MKERR(ERROR,	DISK,			OBJECT,				0x02)
// Drive does not exist/could not be found

#define GNR_DRIVE_DETACHED							MKERR(ERROR,	DISK,			OBJECT,				0x03)
// The drive has been detached -> is not available for playback

#define GNR_NO_DVD_DRIVE							MKERR(ERROR,	DISK,			OBJECT,				0x04)
// The drive is not a DVD drive (used on PCs)

#define GNR_BLOCK_ALREADY_LOCKED					MKERR(ERROR,	DISK,			OBJECT,				0x05)
// A block was already locked in a different mode (read/write)

#define GNR_BLOCK_NOT_LOCKED						MKERR(ERROR,	DISK,			OBJECT,				0x06)
// The block not be unlocked is not locked

#define GNR_DISK_IS_NOT_PRESENT					MKERR(ERROR,	DISK,			OBJECT,				0x07)
// The disk is not present in the drive

#define GNR_INVALID_DRIVE_LETTER					MKERR(ERROR,	DISK,			BOUNDS,				0x01)
// The drive letter specified was not correct/out of range

#define GNR_DISK_READ_ONLY							MKERR(ERROR,	DISK,			FILEIO,				0x00)
// The disk is read-only

#define GNR_DRIVE_LOCK_FAILED						MKERR(ERROR,	DISK,			OPERATION,			0x00)
// Unable to lock/unlock the drive

#define GNR_DRIVE_ALREADY_LOCKED					MKERR(WARNING,	DISK,			OPERATION,			0x01)
// The drive was already locked

#define GNR_DRIVE_LOAD_FAILED						MKERR(ERROR,	DISK,			OPERATION,			0x02)
// Could not load/unload the drive

#define GNR_DRIVE_NOT_LOADABLE					MKERR(ERROR,	DISK,			OPERATION,			0x03)
// Drive does not support loading/unloading the disk

#define GNR_READ_ERROR								MKERR(ERROR,	DISK,			OPERATION,			0x04)
// Some read error occurred

#define GNR_WRITE_ERROR								MKERR(ERROR,	DISK,			OPERATION,			0x05)
// Some write error occurred

#define GNR_HIGH_TEMPERATURE						MKERR(ERROR,	DISK,			OPERATION,			0x06)
// Drive reported that is is unable to work properly due to high temperature

#define GNR_COPY_PROTECTION_VIOLATION			MKERR(ERROR,	DISK,			COPYPROTECTION,	0x00)

#define GNR_COPY_PROTECTION_FAILED				MKERR(ERROR,	DISK,			COPYPROTECTION,	0x01)

#define GNR_READ_ERROR_SECTOR_ENCRYPTED		MKERR(ERROR,	DISK,			COPYPROTECTION,	0x02)
// A sector was requested which happened to be encrypted, but the authentication process was not completed

#endif
