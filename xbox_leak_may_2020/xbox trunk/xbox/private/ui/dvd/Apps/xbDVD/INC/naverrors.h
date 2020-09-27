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
//  Error Declaration for DVDPLD32
//
////////////////////////////////////////////////////////////////////


#ifndef NAVERRORS_H
#define NAVERRORS_H

#include "Library/Common/GNErrors.h"
#include "Library/hardware/drives/generic/DriveErrors.h"
#include "Library/Files/FileErrors.h"

//  Navigation in general

#define GNR_UNEXPECTED_NAVIGATION_ERROR				MKERR(ERROR,	NAVIGATION,	INTERNALSTATE,		0x00)
// Some internal error happened

#define GNR_INVALID_PLAYER									MKERR(ERROR,	NAVIGATION,	OBJECT,				0x00)
// The player handle is invalid

#define GNR_INVALID_NAV_INFO								MKERR(ERROR,	NAVIGATION,	OBJECT,				0x02)
// The navigation information is invalid (possible causes: wrong authoring, read error from drive, internal error)

#define GNR_LOST_DECODER									MKERR(ERROR,	NAVIGATION,	OBJECT,				0x03)
// The decoder has been preempted

#define GNR_DISK_TYPE_NOT_SUPPORTED						MKERR(ERROR,	NAVIGATION, OBJECT,				0x04)
// The disk type is not supported by the current version

#define GNR_OPERATION_NOT_SUPPORTED						MKERR(ERROR,	NAVIGATION, OPERATION,			0x00)
// This operation is (currently) not supported by the player

#define GNR_TITLE_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x01)
// The title specified does not exist

#define GNR_CHAPTER_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x02)
// The chapter specified does not exist

#define GNR_TIME_OUT_OF_RANGE								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x03)
// The time specified (e.g. for a search operation) is out of range

#define GNR_PROGRAM_LINK_NOT_FOUND						MKERR(ERROR,	NAVIGATION,	OPERATION,			0x04)
// The current program does not have a next/prev/upper/etc. program

#define GNR_MENU_DOES_NOT_EXIST							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x05)
// The requested menu does not exist

#define GNR_STREAM_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x06)
// The stream (audio/subpicture) does not exist

#define GNR_FLAGS_NOT_SUPPORTED							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x07)
// The flags specified for this operation are not supported

#define GNR_BUTTON_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x08)
// The specified button does not exist

#define GNR_UNKNOWN_EVENT_TYPE							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x09)
// The event type specified does not exist

#define GNR_BREAKPOINT_NOT_FOUND							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x0a)
// The breakpoint to be deleted does not exist

#define GNR_INVALID_UNIQUE_KEY							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x0b)
// The unique key could not be created/was invalid

#define GNR_ERROR_RECOVERY									MKERR(ERROR,	NAVIGATION, INTERNALSTATE,		0x0c)
// The navigation is currently in an error recovery mode for some reason and therefore it could be
// that some queries can't be satisfied (e.g. the GetExtendedPlayerState())

//
//  DVD specific errors
//

#define GNR_UOP_PROHIBITED									MKERR(ERROR,	DVD,			UOP,					0x01)
// The current UOPs do not allow this action

#define GNR_REGIONS_DONT_MATCH							MKERR(ERROR,	DVD,			REGION,				0x00)
// The system and disk regions don't match

#define GNR_REGION_CODE_INVALID							MKERR(ERROR,	DVD,			REGION,				0x01)
// The region code specified was invalid (e.g. multiregion when trying to set region)

#define GNR_NO_MORE_REGION_SETS							MKERR(ERROR,	DVD,			REGION,				0x02)
// The number of region changes is exhausted

#define GNR_REGION_WRITE_ERROR							MKERR(ERROR,	DVD,			REGION,				0x03)
// Unable to write region

#define GNR_REGION_READ_ERROR								MKERR(ERROR,	DVD,			REGION,				0x04)
// Unable to read region

#define GNR_PARENTAL_LEVEL_TOO_LOW						MKERR(ERROR,	DVD,			PARENTAL,			0x03)
// The parental level currently set is too low to play the disc

#define GNR_ILLEGAL_NAVIGATION_COMMAND					MKERR(ERROR,	DVD,			OPERATION,			0x00)
// The DVD contains an illegal navigation command

#define GNR_ILLEGAL_DOMAIN									MKERR(ERROR,	DVD,			OPERATION,			0x01)
// The domain is illegal for this operation

#define GNR_NO_RESUME_INFORMATION						MKERR(ERROR,	DVD,			OPERATION,			0x02)
// No resume information currently available

#define GNR_COULD_NOT_CHANGE_ANGLE						MKERR(ERROR,	DVD,			OPERATION,			0x03)
// Angle change was not possible

//
//  VCD specific errors
//

//
//  CDDA specific errors
//

#endif
