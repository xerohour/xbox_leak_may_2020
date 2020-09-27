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

#ifndef DVDDISKPLAYERFACTORY_H
#define DVDDISKPLAYERFACTORY_H

#include "DiskPlayerFactory.h"

//
//  This one collects common things of Video/Audio DVD. It should not be instantiated, only its derived classes.
//

class DVDDiskPlayerFactoryCommon : public DVDDiskPlayerFactory
	{
	public:
		virtual Error CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile, DVDDiskType type,
												 DVDFileSystem * dvdfs, BYTE region, CDDiskPlayerClass * & player);
	};

//
//  DVD Video Disk Player Factory
//

class DVDVideoDiskPlayerFactory : public DVDDiskPlayerFactoryCommon
	{
	public:
		virtual Error CreateDVDDisk(DVDDisk * & dvdDisk);
	};

//
//  DVD Audio Disk Player Factory
//

class DVDAudioDiskPlayerFactory : public DVDDiskPlayerFactoryCommon
	{
	public:
		virtual Error CreateDVDDisk(DVDDisk * & dvdDisk);
	};


#endif
