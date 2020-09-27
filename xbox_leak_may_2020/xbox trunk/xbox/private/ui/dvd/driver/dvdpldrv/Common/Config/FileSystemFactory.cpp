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
//  Operating System Specific File System Factories
//
////////////////////////////////////////////////////////////////////

#include "FileSystemFactory.h"

////////////////////////////////////////////////////////////////////
//
//  Base Class For File System Factories
//
////////////////////////////////////////////////////////////////////

//
//  Default implementation
//

Error FileSystemFactory::CreateFileSystem(RootIterator * rit, GenericProfile * profile, GenericFileSystem * & gfs)
	{

	GNRAISE(GNR_DISK_TYPE_NOT_SUPPORTED);
	}

//
//  DVD File System Factory
//

Error DVDFileSystemFactory::CreateFileSystem(RootIterator * rit, GenericProfile * profile, GenericFileSystem * & gfs)
	{
	Error err;

	gfs = new DVDFileSystem(NULL); //Remember to set EventDispatcher for this file system (here we have to pass NULL)
	if (!gfs)
		GNRAISE(GNR_NOT_ENOUGH_MEMORY);

	if (IS_ERROR(err = gfs->Init(rit, NULL, profile)))
		delete gfs;

	GNRAISE(err);
	}

