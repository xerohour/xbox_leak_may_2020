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
//  File System Factories
//
////////////////////////////////////////////////////////////////////

#ifndef FILESYSTEMFACTORY_H
#define FILESYSTEMFACTORY_H

#include "driver/dvdpldrv/Common/DVDFile.h"

class FileSystemFactory
	{
	public:
		virtual Error CreateFileSystem(RootIterator * rit, GenericProfile * profile, GenericFileSystem * & gfs);
	};

class DVDFileSystemFactory : public FileSystemFactory
	{
	public:
		virtual Error CreateFileSystem(RootIterator * rit, GenericProfile * profile, GenericFileSystem * & gfs);
	};


#endif
