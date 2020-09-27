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
//  DVDPLD32: Entry Point For Win32
//
////////////////////////////////////////////////////////////////////

//#include "DDPWin.h"
#include "driver/dvdpldrv/Common/dvdpld32.h"
#include "VCDDiskPlayerFactory.h"
#include "AVFDiskPlayerFactory.h"
#include "CDDADiskPlayerFactory.h"
#include "AVFFileSystemFactory.h"
#include "CDDAFileSystemFactory.h"

////////////////////////////////////////////////////////////////////
//
//  DllMain Function
//
////////////////////////////////////////////////////////////////////

extern "C" {

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
	{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
			{
			WinPortServer * server;
			GenericProfile * profile;
			DriveTable * driveTable;
			Error err;

//			_CrtSetBreakAlloc(116);

			profile = new Profile("RAVISENT", "DVD Navigation Driver", "3.0");
			server = new WinPortServer(hDLLInst);
			if (!profile || !server)
				{
				DP("Could not create Profile or WinPortServer!");
				return FALSE;
				}

			//
			//  Create drive table
			//

			driveTable = new Win9xDriveTable();
			if (!driveTable)
				{
				DP("Not enough memory for Win9x drive table");
				return FALSE;
				}

			if (IS_ERROR(err = driveTable->Init(profile)))
				{
				delete driveTable;

				driveTable = new W2KDriveTable();
				if (!driveTable)
					{
					DP("Not enough memory for W2K drive table");
					return FALSE;
					}

				if (IS_ERROR(err = driveTable->Init(profile)))
					{
					delete driveTable;
					return FALSE;
					}
				}

			//
			//  Perform general initializations
			//

			if (IS_ERROR(DDP_Init(server, profile, driveTable,
										 new DVDVideoDiskPlayerFactory(),
										 new DVDAudioDiskPlayerFactory(),
										 new FullVCDDiskPlayerFactory(),
										 new FullCDDADiskPlayerFactory(),
										 new FullAVFDiskPlayerFactory(),
										 new DVDFileSystemFactory(),
										 new VCDFileSystemFactory(),
										 new CDDAFileSystemFactory(),
										 new AVFFileSystemFactory(),
										 new FileSystemFactory())))
				return FALSE;

			break;
			}
		case DLL_PROCESS_DETACH:

			DDP_Cleanup();

#if DUMPOBJECTS
			_CrtDumpMemoryLeaks();
//			_CrtMemDumpAllObjectsSince(&memstate );
#endif
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
		}

	return TRUE;
	}

}	// extern "C"
