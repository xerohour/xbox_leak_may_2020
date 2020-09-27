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
//  Region Handling
//
////////////////////////////////////////////////////////////////////

#ifndef REGION_H
#define REGION_H

#include "Boards/Generic/vdgendrv.h"

#include "DVDTime.h"
#include "Library/General/Dictnary.h"
#include "Library/Files/RootFile.h"
#include "DVDFile.h"

////////////////////////////////////////////////////////////////////
//
//  Region Handler Class
//
//  Does region management for board and drives
//
////////////////////////////////////////////////////////////////////

class RegionHandler
	{
	protected:
		RootFileSystem * rootfs;

		//
		//  Structure containing RPC information about a drive
		//

		struct RPCInfo
			{
			BOOL	isRPC2;
			BYTE	region;			// Region of drive
			BYTE	availSets;		// Number of region changes left for drive
			};

		StringDictionary rpcInfos;

		BOOL	disableRPC2Write;		// Disable setting the region code on RPC2 drives

		//
		//  Internal functions
		//

		Error	GetBoardRegion(UnitSet units, BYTE & boardRegion, BYTE & availSets);
		Error	SetBoardRegion(UnitSet units, BYTE boardRegion);
		Error CheckRegionCodeWithBoard(UnitSet units, BYTE region);
		Error	GetDriveRegion(RootIterator * rit, BOOL & isRPC2, BYTE & driveRegion, BYTE & availSets);
		Error SetDriveRegion(RootIterator * rit, BYTE & driveRegion);

		BOOL	IsSingleRegion(BYTE region);
		BOOL	IsValidRegion(BYTE region)		{ return (region & 0xff) != 0; }

	public:
		RegionHandler(void);
		virtual ~RegionHandler(void);

		Error Init(GenericProfile * profile);
		Error	GetRegionSettings(UnitSet units, RootIterator * rit, BYTE & systemRegion, BYTE & diskRegion,
										RegionSource & regionSource, BYTE & availSets);
		Error	SetSystemRegion(UnitSet units, RootIterator * rit, BYTE newSystemRegion);
		Error	CheckRegionCodeValid(UnitSet units, RootIterator * rit);
		Error	GetDiskRegion(RootIterator * rit, BYTE & diskRegion);
	};

#endif
