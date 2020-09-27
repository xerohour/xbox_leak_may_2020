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

#include "driver/dvdpldrv/Common/dvdpld32.h"
#include "library/hardware/drives/generic/DriveTable.h"
#include "DiskPlayerFactory.h"

extern "C" {

BOOL WINAPI DllMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
	{
	WinPortServer * winPortServer;
	GenericProfile * profile;
	DWORD dd;
	Error err;

	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
//			_CrtSetBreakAlloc(116);

			profile = new Profile("RAVISENT", "DVD Navigation Driver", "3.0");
			winPortServer = new WinPortServer(hDLLInst);
			if (!profile || !winPortServer)
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

			if (IS_ERROR(DDP_Init(winPortServer, profile, driveTable,
										 new DVDVideoDiskPlayerFactory(),
										 new DVDAudioDiskPlayerFactory(),
										 new VCDDiskPlayerFactory(),
										 new CDDADiskPlayerFactory(),
										 new AVFDiskPlayerFactory(),
										 new OSFileSystemFactory())))
				return FALSE;

			//
			//  Set up threads
			//

			DDPScheduleThread = CreateThread(NULL, 10000, DDPScheduleRoutine, NULL, CREATE_SUSPENDED, &dd);
			SetThreadPriority(DDPScheduleThread, THREAD_PRIORITY_HIGHEST);
			ResumeThread(DDPScheduleThread);

			break;

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
