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

#include "VCDDiskPlayerFactory.h"
#include "driver/dvdpldrv/Common/VCDDiskPlayer.h"

//
//  Real Factory
//

Error FullVCDDiskPlayerFactory::CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile,
																 DVDDiskType diskType, VCDFileSystem * vcdfs, CDDiskPlayerClass * & player)
	{
	Error err;

	//
	//  Create Disk Player
	//

	player = new VCDDiskPlayerClass(server, units);
	if (!player)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	//
	//  Initialize player
	//

	if (IS_ERROR(err = ((VCDDiskPlayerClass*)player)->Init(server, profile, diskType, vcdfs)))
		delete player;

	GNRAISE(err);
	}

