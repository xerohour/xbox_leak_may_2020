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
//  Disk Player Factory Classes
//
////////////////////////////////////////////////////////////////////

#include "DiskPlayerFactory.h"

////////////////////////////////////////////////////////////////////
//
//  DVDDiskPlayer Factory Classes
//
////////////////////////////////////////////////////////////////////

//
//  Base Class
//

Error DVDDiskPlayerFactory::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
															DVDDiskType diskType, DVDFileSystem * dvdfs, BYTE region,
															CDDiskPlayerClass * & player)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

Error DVDDiskPlayerFactory::CreateDVDDisk(DVDDisk * & dvdDisk)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

////////////////////////////////////////////////////////////////////
//
//  VCDDiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base Class
//

Error VCDDiskPlayerFactory::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
															DVDDiskType diskType, VCDFileSystem * vcdfs, CDDiskPlayerClass * & player)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

////////////////////////////////////////////////////////////////////
//
//  CDDADiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base Class
//

Error CDDADiskPlayerFactory::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
															 DVDDiskType diskType, CDDAFileSystem * cddafs, CDDiskPlayerClass * & player)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

////////////////////////////////////////////////////////////////////
//
//  AVFDiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base Class
//

Error AVFDiskPlayerFactory::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
															DVDDiskType diskType, GenericFileSystem * gfs, CDDiskPlayerClass * & player)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

Error AVFDiskPlayerFactory::CreateMP3Player(WinPortServer * server, GenericProfile * profile, DVDDiskType diskType,
														  GenericFileSystem * gfs)
	{
	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

