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
//  Block Buffer CDVD Drive
//
////////////////////////////////////////////////////////////////////

#ifndef BLOCKBUFFERCDVDDRIVE_H
#define BLOCKBUFFERCDVDDRIVE_H

#include "library/hardware/drives/generic/GenericDrive.h"
#include "library/hardware/drives/generic/DriveBlockBuffer.h"

////////////////////////////////////////////////////////////////////
//
//  Block Buffer CDVD Drive Class
//
////////////////////////////////////////////////////////////////////

class BlockBufferCDVDDrive : public CDVDDrive
	{
	protected:
		DriveBlockBuffer	*	blockBuffer;	// Data storage
		PhysicalDiskType		lastDiskType;	// The type of the last GetDiskType inquiry. Used to adjust DriveBlockSize

		virtual DriveBlockBuffer * CreateDriveBlockBuffer(void) = 0;

		//
		//  Data Access
		//

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags) = 0;
		virtual Error ReadCDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags) = 0;

	public:
		BlockBufferCDVDDrive(void);
		virtual ~BlockBufferCDVDDrive(void);

		virtual Error Init(DriveControlInfo * info, GenericProfile * profile);
		virtual Error Cleanup(void);

		//
		//  Inquiry & config functions
		//

		virtual Error GetDriveBlockSize(DWORD sectorType, DWORD & size, DWORD & minSize, DWORD & maxSize, DWORD & step);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		//
		//  Basic Drive Functions
		//

		virtual Error TerminateRequest(RequestHandle * rh);				// Terminate the current request (read/write)
		virtual Error Flush(void);													// Make sure all commands are executed

		//
		//  Data Access Functions
		//

		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);
	};

#endif
