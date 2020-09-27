////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
//
//  DDP Layer Configuration File For Cyberdrive
//
////////////////////////////////////////////////////////////////////

//#include "DDPST20.h"
#include "driver/dvdpldrv/Common/dvdpld32.h"
#include "VCDDiskPlayerFactory.h"
#include "AVFDiskPlayerFactory.h"
#include "CDDADiskPlayerFactory.h"
#include "AVFFileSystemFactory.h"
#include "CDDAFileSystemFactory.h"

////////////////////////////////////////////////////////////////////
//
//  Initialization function
//
////////////////////////////////////////////////////////////////////

Error DDP_Initialize(GenericProfile * profile)
	{
	WinPortServer * server;
	DriveTable * driveTable;

	//
	//  Create WinPortServer
	//

	server = new WinPortServer();
	if (!server)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Create ST20LITE drive table
	//

	driveTable = new ST20DriveTable();
	if (!driveTable)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);
	GNREASSERT(driveTable->Init(profile));

	//
	//  Perform general initialization
	//

	GNRAISE(DDP_Init(server, profile, driveTable,
						  new DVDVideoDiskPlayerFactory(),
						  new DVDAudioDiskPlayerFactory(),
						  new FullVCDDiskPlayerFactory(),
						  new FullCDDADiskPlayerFactory(),
						  new FullAVFDiskPlayerFactory(),
						  new DVDFileSystemFactory(),
						  new VCDFileSystemFactory(),
						  new CDDAFileSystemFactory(),
						  new AVFFileSystemFactory(),
						  new FileSystemFactory()));
	}
