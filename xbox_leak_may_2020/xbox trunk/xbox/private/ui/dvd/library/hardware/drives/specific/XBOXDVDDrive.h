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

#ifndef XBOXDVDDRIVE_H
#define XBOXDVDDRIVE_H

#include "library\common\prelude.h"
#define DEVICE_TYPE DWORD
#include "library\common\krnlsync.h"
#include "library\hardware\drives\generic\BlockBufferCDVDDrive.h"


typedef ULONG DVD_SESSION_ID, *PDVD_SESSION_ID;



////////////////////////////////////////////////////////////////////
//
//  W2K Drive Block Buffer Class
//
////////////////////////////////////////////////////////////////////

class W2KDriveBlockBuffer : public ScatterDriveBlockBuffer
	{
	protected:
		void AllocateData(DWORD size)				{ data = (BYTE*)::VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE); }
		void FreeData(void)							{ ::VirtualFree(data, 0, MEM_RELEASE);	}

	public:
		W2KDriveBlockBuffer(void) : ScatterDriveBlockBuffer() {}
		// W2KDriveBlockBuffer(DWORD blockNum, DWORD blockSize) : ScatterDriveBlockBuffer(blockNum, blockSize) {}
		virtual ~W2KDriveBlockBuffer(void)	{ if (data) ::VirtualFree(data, 0, MEM_RELEASE); }
	};

////////////////////////////////////////////////////////////////////
//
//  NT5 CDVD Drive Class
//
////////////////////////////////////////////////////////////////////

class XBOXDVDDrive : public BlockBufferCDVDDrive
	{
	private:
		KernelString internalDriveName;
		DWORD blockDiff;
		BYTE	singleBlockCacheBuffer[2048];
		DWORD	singleBlockCacheBlock;
		enum	StreamingMode {SMD_UNKNOWN, SMD_STREAMING, SMD_NAVIGATING} streamingMode ;
	protected:
		DVD_SESSION_ID sessionId;
		TCHAR				deviceName[16];
		int				lockCount;
		HANDLE			hDevice;

		DWORD				spindleSpeed;
		DWORD				successfulReadSectors;

		bool				encryptionInfoValid;
		bool				encryptionInfo;

		//
		//  DVD authentication
		//

		virtual Error StartAuthentication(void);
		virtual Error SendChallengeKey(BYTE * key);
		virtual Error GetBusKey(BYTE * key);
		virtual Error GetChallengeKey(BYTE * key);
		virtual Error SendBusKey(BYTE * key);
		virtual Error GetDiskKey(BYTE * key);
		virtual Error CompleteAuthentication(void);
		virtual Error CancelAuthentication(void);
		virtual Error GetTitleKey(DWORD block, BYTE * key);

		Error				GetDevice(BOOL forced=false);
		Error				CloseDevice();

		virtual Error GetDVDType(PhysicalDiskType & type);
		virtual Error GetCDType(PhysicalDiskType & type);

		//
		//  Data Access
		//

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags);
		Error			  ReadCDASubchannel(void __far * subchannel);


		virtual Error SetStreamingMode(bool streaming);

		//
		//	Memory Allocation
		//
		virtual DriveBlockBuffer * CreateDriveBlockBuffer(void);

	public:
		XBOXDVDDrive();
		~XBOXDVDDrive();

		virtual Error Init(DriveControlInfo * info, GenericProfile * profile);
		virtual Error Cleanup(void);


//
		//  Inquiry & config functions
		//
		/*
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
		*/

		//
		//  Basic Drive Functions
		//

		virtual Error SpinUp(void);												// Spin up drive
		virtual Error SpinDown(void);												// Spin down drive
		virtual Error TerminateRequest(RequestHandle * rh);				// Terminate the current request (read/write)
		virtual Error Flush(void);													// Make sure all commands are executed

		//
		//  Media handling
		//

		virtual Error LoadMedia (int slotNumber = 0, DWORD flags = LMF_WAIT);
		virtual Error UnloadMedia (DWORD flags = UMF_WAIT);

		virtual Error GetTrayStatus(GenericTrayStatus & status);			// Get Tray Status, was: GetDriveStatus
		virtual Error LockTray(void);												// Lock Tray
		virtual Error UnlockTray(void);											// Unlock Tray

		virtual Error GetDiskType(PhysicalDiskType & type);				// DVD, CD Rom, Audio CD, ...
		virtual Error GetVolumeID(VolumeID & volumeID);
		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi);

		//
		//  Data access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh);

		//
		//  DVD authentication and region coding
		//

		virtual Error DVDIsEncrypted(BOOL & enc);
		virtual Error GetRPCData(BOOL & isRPC2, BYTE & regionCode, BYTE & availSets);
		virtual Error SetRegionCode(BYTE region);

		//
		//  CD Access Methods
		//

		virtual Error GetNumberOfSessions(WORD & num);
		virtual Error ReadCDTOC(WORD session, CDSessionType & sessionType, CDTocEntry * & toc, WORD & tocEntryNum, RequestHandle * rh);
	};

#endif
