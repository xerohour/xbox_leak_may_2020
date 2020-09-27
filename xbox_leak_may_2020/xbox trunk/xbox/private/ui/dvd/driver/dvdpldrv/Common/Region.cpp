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
//  Region Handling Implementation
//
////////////////////////////////////////////////////////////////////

//  Note:
//  Regions are represented by a one-byte bitmask. Cleared bits represent allowed regions,
//  set bits represent forbidden regions.

#include "Region.h"

//
//  Constructor
//

RegionHandler::RegionHandler(void)
	{
	}

//
//  Destructor
//

RegionHandler::~RegionHandler(void)
	{
	RPCInfo * rpcInfo;

	ITERATE(rpcInfo, &rpcInfos)
		delete rpcInfo;
	ITERATE_END;
	}

//
//  Initialization
//

Error RegionHandler::Init(GenericProfile * profile)
	{
	profile->Read("drives", "disablerpc2write", disableRPC2Write, TRUE);
	GNRAISE_OK;
	}

//
//  Test if region pattern contains only single region
//

BOOL RegionHandler::IsSingleRegion(BYTE region)
	{
	BYTE regionCnt = 0;

	for (int i = 0; i < 8; i++)
		{
		regionCnt += (region & 0x1);
		region >>= 1;
		}

	return (regionCnt == 7);
	}

//
//  Get Disk Region
//
//  Return values:
//  GNR_OK
//

Error RegionHandler::GetDiskRegion(RootIterator * rit, BYTE & diskRegion)
	{
	GenericFileSystem * gfs;
	GenericFile * file;
	GenericDiskItem * gdi;
	RequestHandle rh;
	Error err;

	//
	//  Read setting from file
	//

	diskRegion = 0;
	err = rit->ObtainSubFileSystem(gfs);

	if (!IS_ERROR(err))
		{
		if (!IS_ERROR(err = gfs->OpenItem("video_ts\\video_ts.ifo", FAT_HEADER, gdi)) ||
			 !IS_ERROR(err = gfs->OpenItem("video_ts\\video_ts.inf", FAT_HEADER, gdi)))
			{
			file = (GenericFile*)gdi;
			if (!IS_ERROR(file->ReadBytes(35, 1, &diskRegion, DAF_CACHED, &rh)))
				err = GNR_OK;
			file->Close();
			delete file;
			}

		rit->ReleaseSubFileSystem();
		}

	//
	//  If file could not be opened, the allow all regions
	//

	if (IS_ERROR(err))
		diskRegion = 0;

	GNRAISE_OK;
	}

//
//  Get Board Region
//
//  Return values:
//  GNR_OK
//

Error RegionHandler::GetBoardRegion(UnitSet units, BYTE & pBoardRegion, BYTE & pBoardAvailSets)
	{
	BOOL tagSupported = FALSE;
	WORD availSets;
	BYTE boardRegion = 0;
	BYTE boardAvailSets = 0;

	//
	//  Get supported regions of board
	//

	for (int i=0; i<8; i++)
		{
		if (VDR_CheckRegionCodeValid(units, ~MKBFB(i, 1)))
			boardRegion |= MKBFB(i, 1);
		}
	boardRegion = ~boardRegion;

	//
	//  Get available region sets of decoder
	//

	if (!IS_ERROR(VDR_ConfigureUnitsTags(units, QRY_BOARD_REGION_COUNT(tagSupported), TAGDONE)))
		{
		if (tagSupported && !IS_ERROR(VDR_ConfigureUnitsTags(units, GET_BOARD_REGION_COUNT(availSets), TAGDONE)))
			availSets = 5 - availSets;
		}

	pBoardRegion = boardRegion;
	pBoardAvailSets = boardAvailSets;
	GNRAISE_OK;
	}

//
//  Set board region
//
//  Return values:
//  GNR_OK, GNR_REGION_CODE_INVALID, REGION_WRITE_ERROR
//

Error RegionHandler::SetBoardRegion(UnitSet units, BYTE boardRegion)
	{
	BOOL	tagSupported = FALSE;
	BYTE	regionCode = 0;
	Error	err;

	//
	//  Decoder may only have one region
	//

	if (!IsSingleRegion(boardRegion))
		GNRAISE(GNR_REGION_CODE_INVALID);

	//
	//  Get region code from bitmask
	//

	for (int i=0; i<8; i++)
		{
		if (!XTBFW(i, boardRegion))
			regionCode = (BYTE)i;
		}
	regionCode++;

	//
	//  Write  region code to board
	//

	err = VDR_ConfigureUnitsTags(units, QRY_BOARD_REGION_CODE(tagSupported), TAGDONE);
	if (!IS_ERROR(err) && tagSupported)
		GNRAISE(VDR_ConfigureUnitsTags(units, SET_BOARD_REGION_CODE(regionCode), TAGDONE));
	else
		GNRAISE(GNR_REGION_WRITE_ERROR);
	}

//
//  Check validity of region code with board region
//  Return values:
//  GNR_OK, GNR_REGIONS_DONT_MATCH
//

Error RegionHandler::CheckRegionCodeWithBoard(UnitSet units, BYTE region)
	{
	if (VDR_CheckRegionCodeValid(units, region))
		GNRAISE_OK;
	else
		GNRAISE(GNR_REGIONS_DONT_MATCH);
	}

//
//  Get Drive Region
//  Return values:
//  GNR_OK
//

Error RegionHandler::GetDriveRegion(RootIterator * rit, BOOL & isRPC2, BYTE & driveRegion, BYTE & availSets)
	{
	void * rpcInfo;
	DiskItemName itemName;

	//
	//  Initialize default values
	//

	isRPC2 = FALSE;
	driveRegion = 0;
	availSets = 0;

	//
	//  Try to find info in cache
	//

	GNREASSERT(rit->GetItemName(itemName));
	if (IS_ERROR(rpcInfos.Lookup((char*)itemName, rpcInfo)))
		{
		if (!IS_ERROR(rit->GetRPCData(isRPC2, driveRegion, availSets)))
			{
			//
			//  Try to create new RPC Info and add it to cache
			//

			if (rpcInfo = (void*)new RPCInfo())
				{
				((RPCInfo *)rpcInfo)->isRPC2 = isRPC2;
				((RPCInfo *)rpcInfo)->region = driveRegion;
				((RPCInfo *)rpcInfo)->availSets = availSets;

				if (IS_ERROR(rpcInfos.Insert(itemName, rpcInfo)))
					delete ((RPCInfo *)rpcInfo);
				}
			}
		}
	else
		{
		isRPC2 = ((RPCInfo *)rpcInfo)->isRPC2;
		driveRegion = ((RPCInfo *)rpcInfo)->region;
		availSets = ((RPCInfo *)rpcInfo)->availSets;
		}

	GNRAISE_OK;
	}

//
//  Set Drive Region
//  Return values:
//  GNR_OK, GNR_OPERATION_PROHIBITED, GNR_REGION_CODE_INVALID
//

Error RegionHandler::SetDriveRegion(RootIterator * rit, BYTE & region)
	{
	DiskItemName itemName;
	void * rpcInfo;

	//
	//  First make sure we are allowed to do that and that the region is valid
	//

	GNREASSERT(rit->GetItemName(itemName));
	DP("Setting Region Code of %d", (char*)itemName);
	if (disableRPC2Write)
		GNRAISE(GNR_OPERATION_PROHIBITED);
	else if (!IsSingleRegion(region))
		GNRAISE(GNR_REGION_CODE_INVALID);

	//
	//  Now write the region code and update the cache
	//

	DP("Writing Region");
	GNREASSERT(rit->SetRegionCode(region));
	if (!IS_ERROR(rpcInfos.Lookup((char*)itemName, rpcInfo)))
		{
		((RPCInfo*)rpcInfo)->region = region;
		((RPCInfo*)rpcInfo)->availSets--;
		}

	GNRAISE_OK;
	}

//
//  Get Region Settings (initializes cached data)
//
//  Return values:
//  GNR_OK, GNR_REGION_READ_ERROR
//

Error RegionHandler::GetRegionSettings(UnitSet units, RootIterator * rit, BYTE & pSystemRegion, BYTE & pDiskRegion,
													RegionSource & pRegionSource, BYTE & pSystemAvailSets)
	{
	BYTE pDriveRegion;
	BYTE pDriveAvailSets;
	BOOL pDriveIsRPC2;
	BYTE pBoardRegion;
	BYTE pBoardAvailSets;

	if (!rit)
		GNRAISE(GNR_REGION_READ_ERROR);

	//
	//  Read region settings
	//

	GNREASSERT(GetDiskRegion(rit, pDiskRegion));
	GNREASSERT(GetDriveRegion(rit, pDriveIsRPC2, pDriveRegion, pDriveAvailSets));
	GNREASSERT(GetBoardRegion(units, pBoardRegion, pBoardAvailSets));

	//
	//  Determine region source
	//

	if (pDriveIsRPC2)
		{
		pRegionSource = RGSRC_DRIVE;
		pSystemRegion = pDriveRegion;
		pSystemAvailSets = pDriveAvailSets;
		}
	else
		{
		pRegionSource = RGSRC_BOARD;
		pSystemRegion = pBoardRegion;
		pSystemAvailSets = pBoardAvailSets;
		}

	GNRAISE_OK;
	}

//
//  Set System Region
//
//  Return Values:
//  GNR_OK, GNR_REGION_WRITE_ERROR, GNR_REGIONS_DONT_MATCH
//

Error RegionHandler::SetSystemRegion(UnitSet units, RootIterator * rit, BYTE newSystemRegion)
	{
	RegionSource regionSource;
	BYTE	availSets;
	BYTE	systemRegion = 0;
	BYTE	diskRegion = 0;

	if (!rit)
		GNRAISE(GNR_REGION_WRITE_ERROR);

	//
	//  If new region matches current one then we are done
	//  Do not set region if disk is not single region
	//  or new region doesn't match disk region
	//

	GetRegionSettings(units, rit, systemRegion, diskRegion, regionSource, availSets);
	if (systemRegion == newSystemRegion)
		GNRAISE(GNR_OK);
	if (((~diskRegion) & (~newSystemRegion) & 0xff) == (BYTE)0)
		GNRAISE(GNR_REGIONS_DONT_MATCH);

	//
	//  Set region on drive or board
	//

	if (regionSource == RGSRC_DRIVE)
		GNRAISE(SetDriveRegion(rit, newSystemRegion));
	else
		GNRAISE(SetBoardRegion(units, newSystemRegion));
	}

//
//  Check validity of region code
//
//  Return values:
//  GNR_OK, GNR_REGION_READ_ERROR, GNR_REGIONS_DONT_MATCH
//

Error RegionHandler::CheckRegionCodeValid(UnitSet units, RootIterator * rit)
	{
	BYTE	diskRegion;
	BYTE	driveRegion;
	BYTE	driveAvailSets;
	BOOL	isRPC2;
	Error	err;

	if (!rit)
		GNRAISE(GNR_REGION_READ_ERROR);

	//
	//  Get disk region and allow an all region disc to play always
	//

	GNREASSERT(GetDiskRegion(rit, diskRegion));
	if (!IsValidRegion(diskRegion))
		GNRAISE_OK;

	//
	// Check region code
	//

	GNREASSERT(GetDriveRegion(rit, isRPC2, driveRegion, driveAvailSets));
	if (isRPC2)
		{
		//
		// RPC2 drive
		//

		err = GNR_REGIONS_DONT_MATCH;
		if (driveRegion == 0xff)
			{
			//
			// Drive has no region, means drive is not initialized....
			// And we should not try to write a region code that means not a single region
			//

			if (IsSingleRegion(diskRegion))
				{
				if (!IS_ERROR(SetDriveRegion(rit, diskRegion)))
					err = GNR_OK;
				}
			}
		else
			{
			if (((~driveRegion) & (~diskRegion) & 0xff) != (BYTE) 0 )
				err = GNR_OK;
			}

		if (err != GNR_REGIONS_DONT_MATCH)
			err = GNR_OK;
		}
	else
		{
		//
		// Only board has region code
		//

		err = CheckRegionCodeWithBoard(units, diskRegion);
		}

	GNRAISE(err);
	}
