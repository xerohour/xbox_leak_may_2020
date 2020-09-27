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
//  CDVD Volume Classes
//
////////////////////////////////////////////////////////////////////

#ifndef CDVDVOLUME_H
#define CDVDVOLUME_H

#include "library/Files/GenericVolume.h"

////////////////////////////////////////////////////////////////////
//
//  CDVD Volume Declaration
//
////////////////////////////////////////////////////////////////////

class CDVDVolume : public GenericVolume
	{
	protected:
		CDVDDrive * cdvdDrive;
		VolumeCache cache;
		BOOL isEncrypted;
		BOOL isEncryptedValid;

	public:
		CDVDVolume(void);
		virtual ~CDVDVolume(void);

		//
		//  Init
		//

		virtual Error Init(GenericDrive * drive, GenericProfile * profile);

		//
		//  Drive handling
		//

		virtual Error GetVolumeInfo(VolumeInfo & volumeInfo);

		//
		//  Data Access Methods
		//

		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  DVD Access Methods
		//

		virtual Error DVDIsEncrypted(BOOL & enc);
		virtual Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key);
		virtual Error GetRPCData(BOOL & isRPC2, BYTE & regionCode, BYTE & availSets);
		virtual Error SetRegionCode(BYTE region);

		//
		//  CD Access Methods
		//

		virtual Error GetNumberOfSessions(WORD & sessions);
		virtual Error ReadCDTOC(WORD session, CDSessionType & sessionType, CDTocEntry * & toc, WORD & tocEntryNum, RequestHandle * rh);
	};

#endif
