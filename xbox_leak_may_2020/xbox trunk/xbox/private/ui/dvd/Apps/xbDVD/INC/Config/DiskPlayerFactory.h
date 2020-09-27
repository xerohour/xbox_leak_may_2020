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

#ifndef DISKPLAYERFACTORY_H
#define DISKPLAYERFACTORY_H

class DVDDiskPlayerFactory;
class VCDDiskPlayerFactory;
class CDDADiskPlayerFactory;
class AVFDiskPlayerFactory;

class DVDFileSystem;
class VCDFileSystem;
class CDDAFileSystem;

class DVDDisk;
class DVDGenericCPBI;
#include "driver/dvdpldrv/Common/CDDiskPlayer.h"

#include "boards/generic/vdgendrv.h"

////////////////////////////////////////////////////////////////////
//
//  DVDDiskPlayer Factory Classes
//
////////////////////////////////////////////////////////////////////

//
//  Base class that only returns errors
//

class DVDDiskPlayerFactory
	{
	public:
		virtual Error CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile, DVDDiskType type,
												 DVDFileSystem * dvdfs, BYTE region, CDDiskPlayerClass * & player);
		virtual Error CreateDVDDisk(DVDDisk * & dvdDisk);
	};

////////////////////////////////////////////////////////////////////
//
//  VCDDiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base class that only returns errors
//

class VCDDiskPlayerFactory
	{
	public:
		virtual Error CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile, DVDDiskType type,
												 VCDFileSystem * vcdfs, CDDiskPlayerClass * & player);
	};

////////////////////////////////////////////////////////////////////
//
//  CDDADiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base class that only returns errors
//

class CDDADiskPlayerFactory
	{
	public:
		virtual Error CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile, DVDDiskType type,
												 CDDAFileSystem * cddafs, CDDiskPlayerClass * & player);
	};

////////////////////////////////////////////////////////////////////
//
//  AVFDiskPlayer Factory
//
////////////////////////////////////////////////////////////////////

//
//  Base class that only returns errors
//

class AVFDiskPlayerFactory
	{
	public:
		virtual Error CreateDiskPlayer(WinPortServer * server, UnitSet units, GenericProfile * profile, DVDDiskType type,
												 GenericFileSystem * gfs, CDDiskPlayerClass * & player);
		virtual Error CreateMP3Player(WinPortServer * server, GenericProfile * profile, DVDDiskType type, GenericFileSystem * gfs);
	};

//
//  Further AVF factories that only create subsets of all supported file types should be created here!!!
//

#endif
