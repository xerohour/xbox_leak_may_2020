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
//  General Drive Definitions
//
////////////////////////////////////////////////////////////////////

#ifndef DRIVEDEFS_H
#define DRIVEDEFS_H

#include "Library/Common/Prelude.h"
#include "Library/Common/GNErrors.h"
//
//  From drvtypes.h
//

enum GenericTrayStatus
	{
	GTS_TRAY_UNKNOWN,
	GTS_TRAY_OPEN,
	GTS_TRAY_CLOSED,
	GTS_TRAY_MOVING
	};

#if BOARD_USE_NEW_NAVIGATION

enum GenericIRQTrigger
	{
	GITR_NO_TRIGGER,
	GITR_HIGH_LEVEL,
	GITR_LOW_LEVEL,
	GITR_RISING_EDGE,
	GITR_FALLING_EDGE,
	GITR_ANY_TRIGGER
	};
#endif

enum GenericCopyManagementInfo
	{
	GCMI_NO_RESTRICTION,
	GCMI_ONE_GENERATION,
	GCMI_COPY_FORBIDDEN
	};

//
//  The disk type
//

enum PhysicalDiskType
	{
	PHDT_NONE,
	PHDT_CDDA,
	PHDT_CDROM,
	PHDT_CDI,
	PHDT_CDROM_XA,
	PHDT_DVD_ROM,
	PHDT_DVD_RAM,
	PHDT_DVD_R,
	PHDT_DVD_RW,
	PHDT_HD,
	PHDT_ILLEGAL
	};

#endif
