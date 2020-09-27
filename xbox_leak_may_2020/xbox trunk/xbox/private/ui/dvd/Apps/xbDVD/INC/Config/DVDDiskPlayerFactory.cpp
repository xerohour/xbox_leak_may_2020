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

#include "DVDDiskPlayerFactory.h"
#include "driver/dvdpldrv/Common/DVDDiskPlayer.h"

//
//  Common DVD Disk Player Factory
//

Error DVDDiskPlayerFactoryCommon::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
																	DVDDiskType diskType, DVDFileSystem * dvdfs, BYTE region,
																	CDDiskPlayerClass * & player)
	{
	Error err;

	//
	//  Create Disk Player
	//

	player = new DVDDiskPlayerClass(server, units, this);
	if (!player)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Initialize player
	//

	if (IS_ERROR(err = ((DVDDiskPlayerClass*)player)->Init(server, profile, diskType, dvdfs, region)))
		delete player;

	GNRAISE(err);
	}

//
//  DVD Video Player Factory
//

Error DVDVideoDiskPlayerFactory::CreateDVDDisk(DVDDisk * & dvdDisk)
	{
	dvdDisk = new DVDVideoDisk(this);
	if (dvdDisk)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}

//
//  DVD Audio Player Factory
//

Error DVDAudioDiskPlayerFactory::CreateDVDDisk(DVDDisk * & dvdDisk)
	{
	dvdDisk = new DVDAudioDisk(this);
	if (dvdDisk)
		GNRAISE_OK;
	else
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	}
