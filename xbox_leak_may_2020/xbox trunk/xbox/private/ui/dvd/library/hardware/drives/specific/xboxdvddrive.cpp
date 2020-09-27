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


#include "Library/Common/Prelude.h"
#include "Library/Common/vddebug.h"
#include "XBOXDVDDrive.h"
#include "Library/Hardware/Drives/Generic/CDVDDefs.h"
#include "ntddstor.h"
#include "ntddcdrm.h"
#include "winnt.h"

#ifndef _NTDDCDVD_
#define _NTDDCDVD_

#if _MSC_VER>1000
#pragma once
#endif

#define TEST_REDUCED_DRIVE_TIMEOUT 0

//
// NtDeviceIoControlFile IoControlCode values for this device.
//
// Warning:  Remember that the low two bits of the code specify how the
//           buffers are passed to the driver!
//

#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define IOCTL_DVD_BASE                 FILE_DEVICE_DVD

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//


#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

//
// CDVD Device Control Functions
//
// Warning: Ioctls from 200 through 300 are used for the old common class
// driver ioctls and should not be used for device specific functionality
//

//
// Copyright ioctls
//

#define IOCTL_DVD_START_SESSION     CTL_CODE(IOCTL_DVD_BASE, 0x0400, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_READ_KEY          CTL_CODE(IOCTL_DVD_BASE, 0x0401, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SEND_KEY          CTL_CODE(IOCTL_DVD_BASE, 0x0402, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_END_SESSION       CTL_CODE(IOCTL_DVD_BASE, 0x0403, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SET_READ_AHEAD    CTL_CODE(IOCTL_DVD_BASE, 0x0404, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_GET_REGION        CTL_CODE(IOCTL_DVD_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS)
#define IOCTL_DVD_SEND_KEY2         CTL_CODE(IOCTL_DVD_BASE, 0x0406, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

//
// DVD Structure queries
//

#define IOCTL_DVD_READ_STRUCTURE    CTL_CODE(IOCTL_DVD_BASE, 0x0450, METHOD_BUFFERED, FILE_READ_ACCESS)

//
// The following file contains the IOCTL_STORAGE class ioctl definitions
//

#define IOCTL_STORAGE_SET_READ_AHEAD        CTL_CODE(IOCTL_STORAGE_BASE, 0x0100, METHOD_BUFFERED, FILE_READ_ACCESS)


#ifdef __cplusplus
extern "C" {
#endif

#pragma warning(disable : 4200)

typedef enum {
    DvdChallengeKey = 0x01,
    DvdBusKey1,
    DvdBusKey2,
    DvdTitleKey,
    DvdAsf,
    DvdSetRpcKey = 0x6,
    DvdGetRpcKey = 0x8,
    DvdDiskKey = 0x80,
    DvdInvalidateAGID = 0x3f
} DVD_KEY_TYPE;

typedef struct _DVD_COPY_PROTECT_KEY {
    ULONG KeyLength;
    DVD_SESSION_ID SessionId;
    DVD_KEY_TYPE KeyType;
    ULONG KeyFlags;
    union {
        HANDLE FileHandle;
        LARGE_INTEGER TitleOffset;
    } Parameters;
    UCHAR KeyData[0];
} DVD_COPY_PROTECT_KEY, *PDVD_COPY_PROTECT_KEY;

//
// Predefined (Mt. Fuji) key sizes
// Add sizeof(DVD_COPY_PROTECT_KEY) to get allocation size for
// the full key structure
//

#define DVD_CHALLENGE_KEY_LENGTH    (12 + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_BUS_KEY_LENGTH          (8 + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_TITLE_KEY_LENGTH        (8 + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_DISK_KEY_LENGTH         (2048 + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_RPC_KEY_LENGTH          (sizeof(DVD_RPC_KEY) + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_SET_RPC_KEY_LENGTH      (sizeof(DVD_SET_RPC_KEY) + sizeof(DVD_COPY_PROTECT_KEY))
#define DVD_ASF_LENGTH              (sizeof(DVD_ASF) + sizeof(DVD_COPY_PROTECT_KEY))

//
// Used with IOCTL_DVD_END_SESSION to end all DVD sessions at once
//

#define DVD_END_ALL_SESSIONS ((DVD_SESSION_ID) 0xffffffff)

//
// CGMS Copy Protection Flags
//

#define DVD_CGMS_RESERVED_MASK      0x00000078

#define DVD_CGMS_COPY_PROTECT_MASK  0x00000018
#define DVD_CGMS_COPY_PERMITTED     0x00000000
#define DVD_CGMS_COPY_ONCE          0x00000010
#define DVD_CGMS_NO_COPY            0x00000018

#define DVD_COPYRIGHT_MASK          0x00000040
#define DVD_NOT_COPYRIGHTED         0x00000000
#define DVD_COPYRIGHTED             0x00000040

#define DVD_SECTOR_PROTECT_MASK     0x00000020
#define DVD_SECTOR_NOT_PROTECTED    0x00000000
#define DVD_SECTOR_PROTECTED        0x00000020

/*++

IOCTL_STORAGE_SET_READ_AHEAD

Requests that the storage device skip to TargetAddress once it has run across
TriggerAddress during the course of it's read-ahead caching operations.

Input:

    a STORAGE_SET_READ_AHEAD structure which contains:
        * the trigger address
        * the target address

Output:

    none

--*/

typedef struct _STORAGE_SET_READ_AHEAD {
    LARGE_INTEGER TriggerAddress;
    LARGE_INTEGER TargetAddress;
} STORAGE_SET_READ_AHEAD, *PSTORAGE_SET_READ_AHEAD;

/*++

IOCTL_DVD_READ_STRUCTURE

Issues a READ_DVD_STRUCTURE command to the drive.

Input:

    a DVD_READ_STRUCTURE describing what information is requested

Output:

    a DVD Layer Descriptor as defined below

--*/

typedef enum DVD_STRUCTURE_FORMAT {
    DvdPhysicalDescriptor,
    DvdCopyrightDescriptor,
    DvdDiskKeyDescriptor,
    DvdBCADescriptor,
    DvdManufacturerDescriptor,
    DvdMaxDescriptor
} DVD_STRUCTURE_FORMAT, *PDVD_STRUCTURE_FORMAT;

typedef struct DVD_READ_STRUCTURE {
    LARGE_INTEGER BlockByteOffset;
    DVD_STRUCTURE_FORMAT Format;
    DVD_SESSION_ID SessionId;
    UCHAR LayerNumber;
} DVD_READ_STRUCTURE, *PDVD_READ_STRUCTURE;

typedef struct _DVD_DESCRIPTOR_HEADER {
    USHORT Length;
    UCHAR Reserved[2];
    UCHAR Data[0];
} DVD_DESCRIPTOR_HEADER, *PDVD_DESCRIPTOR_HEADER;

typedef struct _DVD_LAYER_DESCRIPTOR {
    USHORT Length;
    UCHAR BookVersion : 4;
    UCHAR BookType : 4;
    UCHAR MinimumRate : 4;
    UCHAR DiskSize : 4;
    UCHAR LayerType : 4;
    UCHAR TrackPath : 1;
    UCHAR NumberOfLayers : 2;
    UCHAR Reserved1 : 1;
    UCHAR TrackDensity : 4;
    UCHAR LinearDensity : 4;
    ULONG StartingDataSector;
    ULONG EndDataSector;
    ULONG EndLayerZeroSector;
    UCHAR Reserved5 : 7;
    UCHAR BCAFlag : 1;
    UCHAR Reserved6;
	 UCHAR bugbug[4];
} DVD_LAYER_DESCRIPTOR, *PDVD_LAYER_DESCRIPTOR;

typedef struct _DVD_COPYRIGHT_DESCRIPTOR {
    UCHAR CopyrightProtectionType;
    UCHAR RegionManagementInformation;
    USHORT Reserved;
} DVD_COPYRIGHT_DESCRIPTOR, *PDVD_COPYRIGHT_DESCRIPTOR;

typedef struct _DVD_DISK_KEY_DESCRIPTOR {
    UCHAR DiskKeyData[2048];
} DVD_DISK_KEY_DESCRIPTOR, *PDVD_DISK_KEY_DESCRIPTOR;

typedef struct _DVD_BCA_DESCRIPTOR {
    UCHAR BCAInformation[0];
} DVD_BCA_DESCRIPTOR, *PDVD_BCA_DESCRIPTOR;

typedef struct _DVD_MANUFACTURER_DESCRIPTOR {
    UCHAR ManufacturingInformation[2048];
} DVD_MANUFACTURER_DESCRIPTOR, *PDVD_MANUFACTURER_DESCRIPTOR;

typedef struct _DVD_RPC_KEY {
    UCHAR UserResetsAvailable:3;
    UCHAR ManufacturerResetsAvailable:3;
    UCHAR TypeCode:2;
    UCHAR RegionMask;
    UCHAR RpcScheme;
    UCHAR Reserved2[1];
} DVD_RPC_KEY, * PDVD_RPC_KEY;

typedef struct _DVD_SET_RPC_KEY {
    UCHAR PreferredDriveRegionCode;
    UCHAR Reserved[3];
} DVD_SET_RPC_KEY, * PDVD_SET_RPC_KEY;

typedef struct _DVD_ASF {
    UCHAR Reserved0[3];
    UCHAR SuccessFlag:1;
    UCHAR Reserved1:7;
} DVD_ASF, * PDVD_ASF;

typedef struct _DVD_REGION {
     UCHAR CopySystem;
     UCHAR RegionData;                      // current media region (not playable when set)
     UCHAR SystemRegion;                    // current drive region (playable when set)
     UCHAR ResetCount;                      // number of resets available
} DVD_REGION, *PDVD_REGION;

#ifdef __cplusplus
}
#endif
#pragma warning(default : 4200)


#endif  // _NTDDCDVD_

// session info, used to get information about the last session on a multisession disk
// used at GetNumberOfSessions(..).
// this session can be referred to from outside the drive object as session 2
// structure see ATAPI-Documentation, Read Toc Command, Format Field 01h
typedef struct _SessionInfo
	{
	WORD dataLength;
	BYTE firstSession;
	BYTE lastSession;
	BYTE reserved;
	BYTE addr:4;
	BYTE contr:4;
	BYTE firstTrackLastSession;
	BYTE reserved2;
	BYTE addresse[4];
	} SessionInfo;

#define FILE_DEVICE_CONTROLLER          0x00000004
#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER
#define IOCTL_SCSI_PASS_THROUGH_DIRECT  CTL_CODE(IOCTL_SCSI_BASE, 0x0405, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define SCSI_IOCTL_DATA_OUT          0
#define SCSI_IOCTL_DATA_IN           1
#define SCSI_IOCTL_DATA_UNSPECIFIED  2

typedef struct _SCSI_PASS_THROUGH_DIRECT {
    USHORT Length;
    UCHAR ScsiStatus;
    UCHAR PathId;
    UCHAR TargetId;
    UCHAR Lun;
    UCHAR CdbLength;
    UCHAR SenseInfoLength;
    UCHAR DataIn;
    ULONG DataTransferLength;
    ULONG TimeOutValue;
    PVOID DataBuffer;
    ULONG SenseInfoOffset;
    UCHAR Cdb[16];
}SCSI_PASS_THROUGH_DIRECT, *PSCSI_PASS_THROUGH_DIRECT;

struct SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER : SCSI_PASS_THROUGH_DIRECT {
    ULONG             Filler;      // realign buffer to double word boundary
    UCHAR             ucSenseBuf[32];
    };

#define SFF_MODE_SENSE					0x5a
#define SFF_MODE_SELECT					0x55





////////////////////////////////////////////////////////////////////
//
//  W2K CDVD Drive Class
//
////////////////////////////////////////////////////////////////////


//
//  Constructor
//

XBOXDVDDrive::XBOXDVDDrive()
	{
	this->lockCount = 0;
	this->hDevice = NULL;
	this->blockDiff = 0;
	this->singleBlockCacheBlock = 0xffffffff;
	this->streamingMode = SMD_UNKNOWN;
	this->spindleSpeed = 2;
	this->successfulReadSectors = 0;
	}

//
//  Destructor
//

XBOXDVDDrive::~XBOXDVDDrive()
	{
	if (hDevice)
		{
		CloseDevice();
		}

	hDevice = NULL;
	}


//
//  Initialize drive
//

Error XBOXDVDDrive::Init(DriveControlInfo * pInfo, GenericProfile * pProfile)
	{
	Error err = GNR_OK;


	if (!IS_ERROR(err))
		{
		err = BlockBufferCDVDDrive::Init(pInfo, pProfile);
		}

	if (!IS_ERROR(err))
		{
		this->internalDriveName = pInfo->driveName+ (KernelString) ":";
		}

	// if device open, close it
	if (!IS_ERROR(err))
		{
		CloseDevice();
		}

	// set internal device name
	if (!IS_ERROR(err))
		{
		err = this->GetDevice(true);
		}

	return err;
	}


//
// CleanUp
//

Error XBOXDVDDrive::Cleanup()
	{
	Error err = GNR_OK;

/*	if (!IS_ERROR(err) && blockBuffer)
		{
		delete blockBuffer;
		blockBuffer = NULL;
		}

	if (!IS_ERROR(err) && tocBuffer)
		{
		delete[] tocBuffer;
		tocBufferSize = 0;
		} */

	if (!IS_ERROR(err))
		{
		err = CloseDevice();
		}

	return err;
	}

//
// Close Handle to Device
//
Error XBOXDVDDrive::CloseDevice()
	{
	Error err = GNR_OK;

	if (!IS_ERROR(err) && this->hDevice)
		{
		CloseHandle(this->hDevice);
		this->hDevice = NULL;
		}

	return err;
	}


//
// Get Handle to Device
//
Error XBOXDVDDrive::GetDevice(BOOL forced)
	{
	// if forced then close handle first to reopen...
	if (forced)
		CloseDevice();

	// get possible access rights
	if (!this->hDevice)
		{
		//
		// get access rights
		//
		HRESULT res = S_OK;

		encryptionInfoValid = false;

		DWORD desiredAccess = 0;

		desiredAccess = GENERIC_READ/* | GENERIC_WRITE*/;

		// Open file, FILE_FLAG_NO_BUFFERING must be set !
		//	GENERIC_WRITE must be set to provide SENDKEY2, used by SetRegion()...
		// if GENERIC_WRITE cannot be set cause there are insufficent rights,
		// SetRegion() will lead into an error
		hDevice = ::CreateFile("cdrom0:", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);

		if (hDevice == INVALID_HANDLE_VALUE)
			{
			DP(__TEXT("Could not get device, error %x"), GetLastError());
			this->hDevice = NULL;
			GNRAISE(GNR_NO_DVD_DRIVE);
			}
		}

	GNRAISE_OK;
	}


//
//	test if DVD is entrypted
//

Error XBOXDVDDrive::DVDIsEncrypted(BOOL & isEncrypted)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	if (encryptionInfoValid)
		{
		isEncrypted = encryptionInfo;
		GNRAISE_OK;
		}

	Error err = GNR_OK;
	DWORD structSize = 0;

	DVD_READ_STRUCTURE readStruct = {0};
	BYTE * pCprStruct = NULL;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}


	// get memory for copy protection descriptor
	if (!IS_ERROR(err))
		{
		structSize = sizeof(DVD_COPYRIGHT_DESCRIPTOR) + sizeof(DVD_DESCRIPTOR_HEADER);
		pCprStruct = new BYTE [structSize];
		if (!pCprStruct)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	// read copy protection descriptor
	if (!IS_ERROR(err))
		{
		readStruct.Format = DvdCopyrightDescriptor;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(readStruct),
											pCprStruct, structSize, &returned, NULL);
		if (fResult == 0x00)
			{
			DP(__TEXT("Error - IsEncrypted: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}


	// get information out of read struct
	if (!IS_ERROR(err))
		{
		DVD_COPYRIGHT_DESCRIPTOR * pCprDescr = (DVD_COPYRIGHT_DESCRIPTOR *) &(pCprStruct[sizeof(DVD_DESCRIPTOR_HEADER)]);

		encryptionInfo = isEncrypted = (pCprDescr->CopyrightProtectionType != 0);
		encryptionInfoValid = true;
		}

	// garbate collection
	if (pCprStruct)
		delete[] pCprStruct;

	// return result
	return err;
	}


//
//	Get copy management information
//

Error XBOXDVDDrive::GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BOOL fResult;
	DWORD returned = 0;

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// try DVD Read Structure
	if (!IS_ERROR(err))
		{
		DVD_READ_STRUCTURE readStruct = {0};
		DVD_COPYRIGHT_DESCRIPTOR cprDescr = {0};

		readStruct.Format = DvdCopyrightDescriptor;

		fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(readStruct),
											&cprDescr, sizeof(cprDescr), &returned, NULL);
		if (fResult == 0x00)
			{
			err = GNR_NO_DVD_DRIVE;
			DP(__TEXT("Error - IsDVDDrive : %x"), GetLastError());
			}
		else
			{
			switch (cprDescr.CopyrightProtectionType)
				{
				case 0x00 :
					gcmi = GCMI_NO_RESTRICTION;
					break;
				case 0x02 :
					gcmi = GCMI_ONE_GENERATION;
					break;
				case 0x03 :
					gcmi = GCMI_COPY_FORBIDDEN;
					break;
				default:
					gcmi = GCMI_COPY_FORBIDDEN;
					break;
				}
			}
		}

	return GNR_OK;
	}


//
//  Lock tray
//

Error XBOXDVDDrive::LockTray(void)
	{
	GNRAISE_OK;

	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{

		lockCount++;
		if (lockCount == 1)
			{
			BOOL lock = true;
			DWORD returned = 0;

			BOOL fResult = DeviceIoControl( hDevice, IOCTL_STORAGE_MEDIA_REMOVAL, &lock, sizeof(lock),
											NULL, 0, &returned, NULL);

			if (fResult == 0x00)
				{
				DP(__TEXT("Error - Lock: %x"), GetLastError());
				err = GNR_WRITE_ERROR;
				}
			}
		}

	return err;
	}

//
//  Unlock tray
//

Error XBOXDVDDrive::UnlockTray(void)
	{
	GNRAISE_OK;

	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	// get device
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// lock drive...
	if (!IS_ERROR(err))
		{
		lockCount--;
		if (lockCount == 0)
			{

			BOOL lock = false;
			DWORD returned = 0;

			BOOL fResult = DeviceIoControl( hDevice, IOCTL_STORAGE_MEDIA_REMOVAL, &lock, sizeof(lock),
											NULL, 0, &returned, NULL);

			if (fResult == 0x00)
				{
				DP(__TEXT("Error - Unlock: %x"), GetLastError());
				err = GNR_WRITE_ERROR;
				}
			}
		}

	return err;
	}


//
//  Get tray status
//

Error XBOXDVDDrive::GetTrayStatus(GenericTrayStatus & status)
	{
	status = GTS_TRAY_UNKNOWN;

	GNRAISE_OK;
	}


//
//  Get volume ID
//

Error XBOXDVDDrive::GetVolumeID(VolumeID & volumeID)
	{
	volumeID = 0;
	GNRAISE_OK;
	}

//
//  Load media
//

Error XBOXDVDDrive::LoadMedia(int slotNumber, DWORD flags)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	// get device handle to drive
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// do ioctl to load media into drive
	if (!IS_ERROR(err))
		{
		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0,
											NULL, 0, &returned, NULL);
		if (fResult == 0x00)
			{
			DP(__TEXT("Error - LoadMedia: %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}

		}

	return err;
	}


//
//  Unload media
//

Error XBOXDVDDrive::UnloadMedia(DWORD flags)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err =	GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}


	if (!IS_ERROR(err))
		{
		if (lockCount != 0)
			DP(__TEXT("Media Removed but should be locked"));

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0,
										NULL, 0, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - UnloadMedia: %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}
		}


	return err;
	}

//
//  Start authentification
//

Error XBOXDVDDrive::StartAuthentication()
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		err = this->GetDevice();

	if (!IS_ERROR(err))
		{
		DWORD returned = 0;

		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_START_SESSION, NULL, 0,
													&this->sessionId, sizeof(this->sessionId), &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - StartAuthentication: %x"), GetLastError());
			this->sessionId = 0;
			err = GNR_WRITE_ERROR;
			}
		}


	return err;
	}

//
//  Complete authentication
//

Error XBOXDVDDrive::CompleteAuthentication()
	{
	// for unmarked encrypted discs
	encryptionInfo = encryptionInfoValid = true;
	GNRAISE_OK;
	}

//
//  Cancel authentication
//

Error XBOXDVDDrive::CancelAuthentication()
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{
		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_END_SESSION, &this->sessionId, sizeof(this->sessionId),
										NULL, 0, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - CancelAutentication: %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}
		}

	return err;
	}


//
//  Get bus key
//

Error XBOXDVDDrive::GetBusKey(BYTE * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;
	PDVD_COPY_PROTECT_KEY pKey = NULL;

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_BUS_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_BUS_KEY_LENGTH);

		pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_BUS_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType  = DvdBusKey1;

		DWORD returned = 0;

		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_KEY, pKeyData, DVD_BUS_KEY_LENGTH,
											pKeyData, DVD_BUS_KEY_LENGTH, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - GetBusKey: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	if (!IS_ERROR(err))
		{
		for (int i=0; i<6; i++)
			key[i] = pKey->KeyData[i];
		}

	if (pKeyData)
		delete pKeyData;

	return err;
	}


//
//  Get challenge key
//

Error XBOXDVDDrive::GetChallengeKey(BYTE * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;
	PDVD_COPY_PROTECT_KEY pKey = NULL;

	Error err = GNR_OK;



	// get device parameter
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// get memory for key
	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_CHALLENGE_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	// read challenge key...
	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_CHALLENGE_KEY_LENGTH);

		pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_CHALLENGE_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType  = DvdChallengeKey;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_KEY, pKeyData, DVD_CHALLENGE_KEY_LENGTH,
											pKeyData, DVD_CHALLENGE_KEY_LENGTH, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - GetChallengeKey: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	// copy key content to result variable
	if (!IS_ERROR(err))
		{
		for (int i=0; i<10; i++)
			key[i] = pKey->KeyData[i];
		}

	// garbage collection
	if (pKeyData)
		delete pKeyData;


	return err;
	}

//
//  Get disk key
//

Error XBOXDVDDrive::GetDiskKey(BYTE * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	DVD_READ_STRUCTURE	readStruct = {0};
	DVD_DISK_KEY_DESCRIPTOR		keyData = {0};

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{
		readStruct.Format = DvdDiskKeyDescriptor;
		readStruct.SessionId = sessionId;

		for (int i=0; i<2048; i++)
			keyData.DiskKeyData[i] = 0xbd;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(readStruct),
											&keyData, sizeof(keyData), &returned, NULL);


		if (fResult == 0x00)
			{
			DP(__TEXT("Error - GetDiskKey: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	// copy key content to return variable
	if (!IS_ERROR(err))
		{
		for (int i=0; i<2048; i++)
			key[i] = keyData.DiskKeyData[i+4];
		}


	return err;

	}

//
//  Get title key
//

Error XBOXDVDDrive::GetTitleKey(DWORD block, BYTE * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;
	BYTE* pKeyData = NULL;
	PDVD_COPY_PROTECT_KEY pKey = NULL;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// get memory for key data
	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_TITLE_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}


	// read title key from drive
	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_TITLE_KEY_LENGTH);

		pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_TITLE_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType  = DvdTitleKey;
		pKey->Parameters.TitleOffset.QuadPart = (__int64)block * 2048;

		DWORD returned  = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_KEY, pKeyData, DVD_TITLE_KEY_LENGTH,
											pKeyData, DVD_TITLE_KEY_LENGTH, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - GetTitleKey: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}


	// copy key content
	if (!IS_ERROR(err))
		{
		key[0] = 0xF0;
		for (int i=0; i<5; i++)
			key[i+1] = pKey->KeyData[i];
		key[6] = 0;
		key[7] = 0;
		}

	// garbage collection
	if (pKeyData)
		delete pKeyData;

	return err;
	}



//
// Key Handling
// Send Keys
//


//
// Send Challenge Key
//
Error XBOXDVDDrive::SendChallengeKey(BYTE __far * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_CHALLENGE_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_CHALLENGE_KEY_LENGTH);

		PDVD_COPY_PROTECT_KEY pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		// pKey->Parameters.FileHandle = (ULONG) hDevice;
		pKey->KeyLength = DVD_CHALLENGE_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType = DvdChallengeKey;

		// copy key to write struct
		for (int i=0; i<10; i++)
			pKey->KeyData[i] = key[i];
		pKey->KeyData[10] = pKey->KeyData[11] = 0;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_SEND_KEY, pKeyData, DVD_CHALLENGE_KEY_LENGTH,
											NULL, 0 , &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - SendChalllengeKey: %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}
		}

	if (pKeyData)
		delete[] pKeyData;

	return err;
	}


//
// Send bus key
//

Error XBOXDVDDrive::SendBusKey(BYTE __far * key)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		err = this->GetDevice();

	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_BUS_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	if (!IS_ERROR(err))
		{

		ZeroMemory(pKeyData, DVD_BUS_KEY_LENGTH);

		PDVD_COPY_PROTECT_KEY pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_BUS_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType  = DvdBusKey2;

		// copy key content
		for (int i=0; i<6; i++)
			pKey->KeyData[i] = key[i];
		pKey->KeyData[6] = pKey->KeyData[7] = 0;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_SEND_KEY, pKeyData, DVD_BUS_KEY_LENGTH,
											pKeyData, DVD_BUS_KEY_LENGTH, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - SendBusKey : %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}
		}

	if (pKeyData)
		delete[] pKeyData;

	return err;
	}






//
// RPC2 Support
//

Error XBOXDVDDrive::GetRPCData(BOOL __far &isRPC2, BYTE __far &regionCode, BYTE __far &availResets)
	{
	availResets = 10;
	isRPC2 = false;
	regionCode = 0;

	GNRAISE_OK;

	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;
	PDVD_COPY_PROTECT_KEY pKey = NULL;

	Error err = GNR_OK;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_RPC_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_RPC_KEY_LENGTH);

		pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_RPC_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType  = DvdGetRpcKey;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_KEY, pKeyData, DVD_RPC_KEY_LENGTH,
											pKeyData, DVD_RPC_KEY_LENGTH, &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - GetRpcData: %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	if (!IS_ERROR(err))
		{
		availResets = pKey->KeyData[0] & 0x07;
		regionCode = pKey->KeyData[1];
  		isRPC2 = (pKey->KeyData[2] != 0x00);
		}

	if (pKeyData)
		delete[] pKeyData;

	return err;
	}


// Set Region will only work if drive is open with right access.
// If the calling thread had not the access rights to open the drive with write
// access, SetRegion will lead into an error code (GNR_WRITE_ERROR)

Error XBOXDVDDrive::SetRegionCode(BYTE regionCode)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	BYTE* pKeyData = NULL;

	Error err = GNR_OK;

	// get drive
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// get memory for key
	if (!IS_ERROR(err))
		{
		pKeyData = new BYTE [DVD_SET_RPC_KEY_LENGTH];
		if (!pKeyData)
			err = GNR_NOT_ENOUGH_MEMORY;
		}

	// Write key
	if (!IS_ERROR(err))
		{
		ZeroMemory(pKeyData, DVD_SET_RPC_KEY_LENGTH);

		PDVD_COPY_PROTECT_KEY pKey = (PDVD_COPY_PROTECT_KEY) pKeyData;
		pKey->KeyLength = DVD_SET_RPC_KEY_LENGTH;
		pKey->SessionId = this->sessionId;
		pKey->KeyType = DvdSetRpcKey;

		DVD_SET_RPC_KEY rpcKeyData = {0};
		rpcKeyData.PreferredDriveRegionCode = regionCode;
		rpcKeyData.Reserved[0] = 0;
		rpcKeyData.Reserved[1] = 0;
		rpcKeyData.Reserved[2] = 0;

		// copy key content
		for (int i=0; i<sizeof(DVD_SET_RPC_KEY); i++)
			pKey->KeyData[i] = ((BYTE*) (&rpcKeyData))[i];

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_SEND_KEY2, pKeyData, DVD_SET_RPC_KEY_LENGTH,
											pKeyData, DVD_SET_RPC_KEY_LENGTH , &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - Set RPC Key failed due to : %x"), GetLastError());
			err = GNR_WRITE_ERROR;
			}
		}

	// do some garbage collection
	if (pKeyData)
		delete[] pKeyData;

	GNRAISE_OK;
	}



//
// Audio CD handling...
//


// Read Audio CD Subchannel...

Error XBOXDVDDrive::ReadCDASubchannel(void __far * subchannel)
	{
	Error err = GNR_OK;

	if (subchannel == NULL)
		GNRAISE_OK;

	VDAutoMutex autoMutex(&monitorMutex);

	if (IS_ERROR(err = this->GetDevice()))
		GNRAISE(err);

	BOOL fResult = 0;
	DWORD returned = 0;

	CDROM_SUB_Q_DATA_FORMAT subQReqStruct = {0};
	subQReqStruct.Format = IOCTL_CDROM_CURRENT_POSITION;
	subQReqStruct.Track = 0;

	SUB_Q_CURRENT_POSITION subQCurrPos = {0};

	fResult = DeviceIoControl( hDevice, IOCTL_CDROM_READ_Q_CHANNEL, &subQReqStruct, sizeof(subQReqStruct),
										&subQCurrPos, sizeof(subQCurrPos), &returned, NULL);

	if (fResult == 0x00)
		{
		DP(__TEXT("Error - ReadCDABlock : %x"), GetLastError());
		GNRAISE(GetLastError());
		}
	else
		{
		// write subchannel.
		// since we only get Q-Channel, white out the other channels (12 Bytes P-Channel, 12 Bytes Q-Channel,...)
		BYTE* pSub = (BYTE*)subchannel;
		for (int i=0; i<12; i++)
			{
			*pSub = 0;
			pSub++;
			}


		*pSub = 0; pSub++;
		*pSub = 0; pSub++;
		*pSub = (subQCurrPos.Control << 4) | (subQCurrPos.ADR); pSub++;
		*pSub = (subQCurrPos.TrackNumber); pSub++;
		*pSub = (subQCurrPos.IndexNumber); pSub++;
		i += 5;
		BYTE * pQChan = (BYTE*)(&subQCurrPos);
		for (;i<24;i++)
			{
			*pSub = *pQChan;
			pSub++;
			pQChan++;
			}

		for (;i<CDDA_SUBCHANNEL_SIZE;i++)
			{
			*pSub = 0;
			pSub++;
			}

		}
	GNRAISE_OK;

	}


// Read blocks from Audio CD


Error XBOXDVDDrive::ReadCDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;
	RAW_READ_INFO rrInfo;

	// get device
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// blockDiff = 150;
	if (!IS_ERROR(err))
		{
		switch (GD_SECTOR_TYPE(flags))
			{
			case DST_CDDA :
			case DST_CDROM_MODE2_XA_FORM2 :
			case DST_CDROM_MODE2 :
				{
				ZeroMemory(&rrInfo, sizeof(rrInfo));
				rrInfo.DiskOffset.QuadPart = (block - blockDiff)*2048;
				rrInfo.SectorCount = num;


				switch (GD_SECTOR_TYPE(flags))
					{
					case DST_CDDA : rrInfo.TrackMode = CDDA; break;
					case DST_CDROM_MODE2 : rrInfo.TrackMode = YellowMode2; break;
					case DST_CDROM_MODE2_XA_FORM2 : rrInfo.TrackMode = XAForm2; break;
					default : err = GNR_INVALID_PARAMETERS;
					}

				// read blocks
				if (!IS_ERROR(err))
					{
					// data size. See remarks at rrInfo.Diskoffset...
					DWORD dataSize = num * CD_FRAME_SIZE;

					DWORD returned = 0;
					BOOL fResult = DeviceIoControl(	hDevice, IOCTL_CDROM_RAW_READ, &rrInfo, sizeof(rrInfo),
																buffer, dataSize , &returned, NULL);

					if (fResult == 0x00)
						{
						DP(__TEXT("Error - ReadCDABlock : %x"), GetLastError());
						err = GNR_READ_ERROR;
						}
					}
				break;
				}
			case DST_CDROM_MODE1:
			case DST_CDROM_MODE2_XA_FORM1:
				{
				DWORD dstBlock = block - blockDiff;
				long highPart = dstBlock >> 21;

				DWORD dwPointer = ::SetFilePointer (this->hDevice, dstBlock << 11, &highPart, FILE_BEGIN);

				if (dwPointer != 0xffffffff || (dwPointer == 0xffffffff && GetLastError() == NO_ERROR))
					{
					DWORD bytesRead = 0;
					BOOL fResult = ::ReadFile(hDevice, buffer, num << 11, &bytesRead, NULL);

					if (fResult == 0x00)
						DP(__TEXT("Error in ReadBlocks, cause %d"), GetLastError());
					}

				// format data in right format
				if (GD_SECTOR_TYPE(flags) == DST_CDROM_MODE2_XA_FORM1)
					{
					for (int i = num-1; i >= 0; i--)
						{
						for (int j = 2047; j >= 0; j--)
							buffer[i*CD_FRAME_SIZE+0x18 + j] = buffer[i*2048+j];
						}
					}
				break;
				}
			}
		}


	// read subchannel
	if (!IS_ERROR(err) && (flags & DAF_READ_SUBCHANNEL))
		{
		BYTE * buf = (BYTE*) buffer;
		buf += CD_FRAME_SIZE;
		err = ReadCDASubchannel(buf);
		}

	return err;
	}



//
// Read Audio CD Directory
// session number must be 1 or 2 (only for mutisession disk)
// if there are more than 2 real session, 2 sessions are faked the following way: sessions 1..n-1 is the first session,
// the last session is given as the secound session.
// See also GetNumberOfSessions(..)
// Toc array starts at index 0 (formally 1)
//

#define CDA_AUDIO_BLOCK_SIZE	2352
#define CDA_TOC_BUFFER_SIZE  CDA_AUDIO_BLOCK_SIZE+1000


Error XBOXDVDDrive::ReadCDTOC(WORD session, CDSessionType & sessionType, CDTocEntry * & toc, WORD & entryNum, RequestHandle * rh)
	{
	VDAutoMutex autoMutex(&monitorMutex);

	CDROM_TOC cdToc = {0};
	SessionInfo sessionInfo = {0};

	Error err = GNR_OK;

	// get number of sessions in drive
	if (!IS_ERROR(err))
		{
		DWORD returned = 0;
		BOOL fResult = ::DeviceIoControl(this->hDevice, IOCTL_CDROM_GET_LAST_SESSION, NULL, NULL, &sessionInfo, sizeof(sessionInfo), &returned, NULL);
		if (fResult == 0x00)
			{
			DP(__TEXT("Error in ReadCDToc, GetLastSession returned %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	// compare to test parameters
	if (!IS_ERROR(err))
		{
		if (session > 2)
			err = GNR_INVALID_PARAMETERS;
		if ((sessionInfo.firstSession == sessionInfo.lastSession) && (session == 2))
			err = GNR_INVALID_PARAMETERS;
		}

	// get drive device
	if (!IS_ERROR(err))
		err = this->GetDevice();


	// read complete toc
	if (!IS_ERROR(err))
		{

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_CDROM_READ_TOC, &cdToc, sizeof(cdToc),
											&cdToc, sizeof(cdToc) , &returned, NULL);

		if (fResult == 0x00)
			{
			DP(__TEXT("Error - ReadCDAToc : %x"), GetLastError());
			err = GNR_READ_ERROR;
			}
		}

	// convert toc
	if (!IS_ERROR(err))
		{
		// calculate number of toc entries
		DWORD tocLength = cdToc.LastTrack - cdToc.FirstTrack + 1;
		DWORD firstTrackOfSession = 0;
		DWORD lastTrackOfSession = 0;

		// set first toc entry for session.
		if (sessionInfo.firstSession == sessionInfo.lastSession)
			{
			// single session...
			firstTrackOfSession = 0;
			lastTrackOfSession = tocLength-1;
			}
		else
			{
			// Multisession
			if (session == 2)
				{
				// Multisession, secound session toc
				firstTrackOfSession = sessionInfo.firstTrackLastSession-1;
				lastTrackOfSession = tocLength-1;
				}
			else
				{
				// Multisession, first session toc
				firstTrackOfSession = 0;
				lastTrackOfSession = sessionInfo.firstTrackLastSession-2;
				}
			}

		entryNum = (WORD) (lastTrackOfSession - firstTrackOfSession +1);

		if (entryNum != 0)
			{
			toc = new CDTocEntry[entryNum];
			if  (!toc)
				err = GNR_NOT_ENOUGH_MEMORY;

			if (!IS_ERROR(err))
				{
				for (DWORD i = firstTrackOfSession; i<lastTrackOfSession+1; i++)
					{
					CDTocEntry dummyEntry;

					DWORD dstPos = i - firstTrackOfSession;

					// set track begginging and length...

					toc[dstPos].SetStartBlock(cdToc.TrackData[i].Address[1], cdToc.TrackData[i].Address[2], cdToc.TrackData[i].Address[3]);
					if (dstPos == 0)
						blockDiff = toc[dstPos].GetStartBlock();
					dummyEntry.SetStartBlock(cdToc.TrackData[i+1].Address[1], cdToc.TrackData[i+1].Address[2], cdToc.TrackData[i+1].Address[3]);
					toc[dstPos].SetNumberOfBlocks(dummyEntry.GetStartBlock() -  toc[i - firstTrackOfSession].GetStartBlock());

					// get track type
					CDTrackType trackType = CTT_NONE;
					if ((cdToc.TrackData[i].Control & 0x0c) != 0x04)
						trackType = CTT_AUDIO;
					else
						trackType = CTT_DATA;
					toc[dstPos].SetTrackType(trackType);

					DP("track added");
					}
				}
			}
		else
			{
			//  tocEntryNum == 0
			err = GNR_OBJECT_INVALID;
			}
		}

	// find out which type of sector
	PhysicalDiskType diskType = PHDT_NONE;
	if (!IS_ERROR(err))
		{
		err = this->GetCDType(diskType);
		}
	if (!IS_ERROR(err))
		{
		switch (diskType)
			{
			case PHDT_CDROM :
			case PHDT_CDDA : sessionType = CST_CDDA; break;

			case PHDT_CDI : sessionType = CST_CDI; break;
			case PHDT_CDROM_XA : sessionType = CST_CDROM_XA; break;

			case PHDT_NONE : sessionType = CST_NONE; break;
			}
		}


	// return result
	return err;
	}



//
// Read / Write commands
//


//
// Read Block(s) from DVD
//

#include "library\lowlevel\timer.h"

#define IOCTL_CDROM_SET_SPINDLE_SPEED CTL_CODE(IOCTL_CDROM_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS)


Error XBOXDVDDrive::ReadDVDBlocks(DWORD block, DWORD num, BYTE * buffer, DWORD flags)
	{
	// no lock while called from LockBlocks, although double blocking will make no problem...
	// VDAutoMutex autoMutex(&mutex);

	Error err = GNR_OK;
	DWORD res;
	bool retry;
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	BOOL fSuccess;
	DWORD cbReturned;
	BYTE	senseBuffer[32];
	int	retryCount;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	if (block == singleBlockCacheBlock)
		{
		memcpy(buffer, singleBlockCacheBuffer, 2048);
		block++;
		num--;
		buffer += 2048;
		}

	//
	// Retry navigation sectors
	//
	if (streamingMode == SMD_NAVIGATING)
		retryCount = 3;
	else
		retryCount = 1;

	if (num && !IS_ERROR(err))
		{
		ZeroMemory(&PassThrough, sizeof(PassThrough));
		PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

		PassThrough.DataBuffer = buffer;
		PassThrough.DataTransferLength = num << 11;

		PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
		PassThrough.Cdb[ 0] = 0x28;
		PassThrough.Cdb[ 1] = 0x00;
		PassThrough.Cdb[ 2] = (BYTE)(block >> 24) & 0xff;
		PassThrough.Cdb[ 3] = (BYTE)(block >> 16) & 0xff;
		PassThrough.Cdb[ 4] = (BYTE)(block >>  8) & 0xff;
		PassThrough.Cdb[ 5] = (BYTE)(block      ) & 0xff;
		PassThrough.Cdb[ 6] = 0x00;
		PassThrough.Cdb[ 7] = (BYTE)(num   >>  8) & 0xff;
		PassThrough.Cdb[ 8] = (BYTE)(num        ) & 0xff;
		PassThrough.Cdb[ 9] = 0x00;
		PassThrough.Cdb[10] = 0x00;
		PassThrough.Cdb[11] = 0x00;

		do {
			senseBuffer[0] = 0;
			retry = false;

			fSuccess = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), senseBuffer, 32, &cbReturned, NULL);
			if (!fSuccess)
				{
				DWORD res = GetLastError();

				if (res == 23 || res == 121)
					err = GNR_FILE_READ_ERROR;
				else
					err = GNR_END_OF_FILE;

				DP("Read error at %08lx of %d sectors %d", block, num, res);
				successfulReadSectors = 0;
				}
			else if (senseBuffer[0])
				{
				err = GNR_FILE_READ_ERROR;

				DP("Read error at %08lx of %d sectors %02x %02x %02x %02x - %02x %02x %02x %02x - %02x %02x %02x %02x %02x %02x",  block, num,
					senseBuffer[0], senseBuffer[1], senseBuffer[2], senseBuffer[3],
					senseBuffer[4], senseBuffer[5], senseBuffer[6], senseBuffer[7],
					senseBuffer[8], senseBuffer[9], senseBuffer[10], senseBuffer[11],
					senseBuffer[12], senseBuffer[13]);

				successfulReadSectors = 0;
				}
			else
				{
				successfulReadSectors += num;
				if (spindleSpeed < 2 && successfulReadSectors >= 10000)
					{
					spindleSpeed++;
					fSuccess = DeviceIoControl(hDevice, IOCTL_CDROM_SET_SPINDLE_SPEED, &spindleSpeed, sizeof(spindleSpeed), NULL, 0, &cbReturned, NULL);
					DP("Speed change to %d returned %d", spindleSpeed, fSuccess);
					successfulReadSectors = 0;
					}

				if (num == 1)
					{
					singleBlockCacheBlock = block;
					memcpy(singleBlockCacheBuffer, buffer, 2048);
					}

				err = GNR_OK;
				}

			//
			// Retry based on counter
			//
			if (err != GNR_OK && !retry && retryCount)
				{
				DP("Retrying due to retry counter %d", retryCount);

				if (spindleSpeed > 1)
					{
					spindleSpeed--;

					fSuccess = DeviceIoControl(hDevice, IOCTL_CDROM_SET_SPINDLE_SPEED, &spindleSpeed, sizeof(spindleSpeed), NULL, 0, &cbReturned, NULL);

					DP("Speed change to %d returned %d", spindleSpeed, fSuccess);
					}

				retry = true;
				retryCount--;
				}

			} while (retry);
		}

    if(err != GNR_OK)
        DP("Read error failed after all with err=%d\n", err);

	GNRAISE(err);
	}


// Spin dows drive - not implemented but returns ok
Error XBOXDVDDrive::SpinDown()
	{
	// drive in PC should spin down automatically
	GNRAISE_OK;
	}

// Spin up drive - not implelemted but returns ok, cause this is no error
Error XBOXDVDDrive::SpinUp()
	{
	// drive in PC should be spinned up...
	GNRAISE_OK;
	}

// Seek Blocks - no functionallity at the moment...
Error XBOXDVDDrive::SeekBlock(DWORD block, DWORD flags, RequestHandle *rh)
	{
	GNRAISE_OK;
	}


Error XBOXDVDDrive::SetStreamingMode(bool streaming)
	{
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	UCHAR ReadErrorRecoveryPage[20];
	BOOL success;
	Error err;
	BOOL fSuccess;
	DWORD cbReturned;

	err = this->GetDevice();

	// try DVD Read Structure
	if (!IS_ERROR(err))
		{
		if (streaming && streamingMode != SMD_STREAMING)
			{
			ZeroMemory(&PassThrough, sizeof(PassThrough));
			PassThrough.Length = sizeof(PassThrough);
			PassThrough.DataBuffer = &ReadErrorRecoveryPage;
			PassThrough.DataTransferLength = sizeof(ReadErrorRecoveryPage);

			PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
			PassThrough.Cdb[ 0] = SFF_MODE_SENSE;
			PassThrough.Cdb[ 1] = 0x08;
			PassThrough.Cdb[ 2] = 0x01;
			PassThrough.Cdb[ 3] = 0x00;
			PassThrough.Cdb[ 4] = 0x00;
			PassThrough.Cdb[ 5] = 0x00;
			PassThrough.Cdb[ 6] = 0x00;
			PassThrough.Cdb[ 7] = 0x00;
			PassThrough.Cdb[ 8] = 0x14;
			PassThrough.Cdb[ 9] = 0x00;
			PassThrough.Cdb[10] = 0x00;
			PassThrough.Cdb[11] = 0x00;
			fSuccess = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(PassThrough), NULL, 0, &cbReturned, NULL);


			if (fSuccess)
				{
				ReadErrorRecoveryPage[8 + 2] = 0x00;
				ReadErrorRecoveryPage[8 + 3] = 0x01;

				PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
				PassThrough.Cdb[ 0] = SFF_MODE_SELECT;
				PassThrough.Cdb[ 1] = 0x10;
				PassThrough.Cdb[ 2] = 0x01;
				PassThrough.Cdb[ 3] = 0x00;
				PassThrough.Cdb[ 4] = 0x00;
				PassThrough.Cdb[ 5] = 0x00;
				PassThrough.Cdb[ 6] = 0x00;
				PassThrough.Cdb[ 7] = 0x00;
				PassThrough.Cdb[ 8] = 0x14;
				PassThrough.Cdb[ 9] = 0x00;
				PassThrough.Cdb[10] = 0x00;
				PassThrough.Cdb[11] = 0x00;
 				fSuccess = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(PassThrough), NULL, 0, &cbReturned, NULL);

				if (fSuccess)
					{
					streamingMode = SMD_STREAMING;
					}
				}

			GNRAISE_OK;
			}
		else if (!streaming && streamingMode != SMD_NAVIGATING)
			{
			ZeroMemory(&PassThrough, sizeof(PassThrough));
			PassThrough.Length = sizeof(PassThrough);
			PassThrough.DataBuffer = &ReadErrorRecoveryPage;
			PassThrough.DataTransferLength = sizeof(ReadErrorRecoveryPage);

			PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
			PassThrough.Cdb[ 0] = SFF_MODE_SENSE;
			PassThrough.Cdb[ 1] = 0x08;
			PassThrough.Cdb[ 2] = 0x01;
			PassThrough.Cdb[ 3] = 0x00;
			PassThrough.Cdb[ 4] = 0x00;
			PassThrough.Cdb[ 5] = 0x00;
			PassThrough.Cdb[ 6] = 0x00;
			PassThrough.Cdb[ 7] = 0x00;
			PassThrough.Cdb[ 8] = 0x14;
			PassThrough.Cdb[ 9] = 0x00;
			PassThrough.Cdb[10] = 0x00;
			PassThrough.Cdb[11] = 0x00;
			fSuccess = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(PassThrough), NULL, 0, &cbReturned, NULL);

			if (fSuccess)
				{
				ReadErrorRecoveryPage[8 + 2] = 0x00;
				ReadErrorRecoveryPage[8 + 3] = 0x04;

				PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
				PassThrough.Cdb[ 0] = SFF_MODE_SELECT;
				PassThrough.Cdb[ 1] = 0x10;
				PassThrough.Cdb[ 2] = 0x01;
				PassThrough.Cdb[ 3] = 0x00;
				PassThrough.Cdb[ 4] = 0x00;
				PassThrough.Cdb[ 5] = 0x00;
				PassThrough.Cdb[ 6] = 0x00;
				PassThrough.Cdb[ 7] = 0x00;
				PassThrough.Cdb[ 8] = 0x14;
				PassThrough.Cdb[ 9] = 0x00;
				PassThrough.Cdb[10] = 0x00;
				PassThrough.Cdb[11] = 0x00;
 				fSuccess = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT, &PassThrough, sizeof(PassThrough), NULL, 0, &cbReturned, NULL);

				if (fSuccess)
					{
					streamingMode = SMD_NAVIGATING;
					}
				}

			GNRAISE_OK;
			}
		else
			GNRAISE_OK;
		}
	else
		GNRAISE(err);
	}

Error XBOXDVDDrive::LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle *rh)
	{
#if TEST_REDUCED_DRIVE_TIMEOUT
#if _DEBUG
	int t1 = timeGetTime();
#endif

	SetStreamingMode((flags & DAF_STREAMING) != 0);

#if _DEBUG
	int t2 = timeGetTime();
	if (t2-t1 > 50)
		{
		DP("SetStreamingMode took long time %d", t2-t1);
		}
#endif
#endif //TEST_REDUCED_DRIVE_TIMEOUT

	return BlockBufferCDVDDrive::LockBlocks(block, num, blocks, flags, rh);
	}


DriveBlockBuffer * XBOXDVDDrive::CreateDriveBlockBuffer(void)
	{
	return new W2KDriveBlockBuffer;
	}


// Flush - not implemented at the moment
Error XBOXDVDDrive::Flush()
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}

// Flush - not implemented at the moment
Error XBOXDVDDrive::TerminateRequest(RequestHandle * rh)
	{
	GNRAISE(GNR_UNIMPLEMENTED);
	}


// Get Disk Type
// Calls first GetDVDType(..), if fails calls GetCDType(..).
// returns the GenericDiskType, see definition there of possible return values

Error XBOXDVDDrive::GetDiskType(PhysicalDiskType & type)
	{
	VDAutoMutex mutex(&monitorMutex);
	Error err = GNR_OK;

	type = PHDT_NONE;

	if (!IS_ERROR(err))
		{
		err = GetDVDType(type);
		}

	if (IS_ERROR(err) || type == PHDT_NONE)
		err = GetCDType(type);

	if (IS_ERROR(err))
		type = PHDT_NONE;

	return err;

	}


// Get type of DVD disk in drive.
// If the drive is no DVD drive, this call will return with an error and type = PDT_NONE

Error XBOXDVDDrive::GetDVDType(PhysicalDiskType & type)
	{
	VDAutoMutex autoMutex(&monitorMutex);
	Error err = GNR_OK;

	DVD_READ_STRUCTURE readStruct = {0};
	DVD_LAYER_DESCRIPTOR layerDescr = {0};
	BYTE buffer[2048];
	for(int i=0; i<2048; i++) buffer[i] = 0xbd;

	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// try DVD Read Structure
	if (!IS_ERROR(err))
		{
		readStruct.Format = DvdPhysicalDescriptor;

		DWORD returned = 0;
		BOOL fResult = DeviceIoControl( hDevice, IOCTL_DVD_READ_STRUCTURE, &readStruct, sizeof(readStruct),
											&layerDescr, sizeof(layerDescr), &returned, NULL);
		if (fResult == 0x00)
			{
			err = GNR_READ_ERROR;
			type = PHDT_NONE;
			DP(__TEXT("Error - GetDVDType : %x"), GetLastError());
			}
		}

	// find out from result which type
	if (!IS_ERROR(err))
		{
		BYTE layerType = (layerDescr.BookType) | (layerDescr.BookVersion << 4);
		switch (layerType)
			{
			case 0x0000:
				type = PHDT_DVD_ROM;
				break;
			case 0x0001:
				type = PHDT_DVD_RAM;
				break;
			case 0x0010:
				type = PHDT_DVD_R;
				break;
			case 0x1001:
				type = PHDT_DVD_RW;
				break;
			}
		}

	return GNR_OK;
	}



//! Get type of CD
// Disks supported are
// - CD Audio (CDDA)
// - CDROM Mode 1
// - CDROM Mode 2 XA Form 1 & 2
// if there is a CDROM Mode 2 NOT XA, this detection fails (we never seen such a CD).
// if this leads into problems, you could find out with evaluating the subheader byte after the
// evaluaing the sector checksum (See "System Description CDROM XA, II.4, May 1991)
//

Error XBOXDVDDrive::GetCDType(PhysicalDiskType & type)
	{
	type = PHDT_CDROM_XA;

	GNRAISE_OK;

	VDAutoMutex autoMutex(&monitorMutex);

	Error err = GNR_OK;

	CDROM_DISK_DATA diskType = {0};
	CDROM_TOC cdToc = {0};

	// get device
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// find out if cd contains data and / or audio tracks
	if (!IS_ERROR(err))
		{
		DWORD returned = 0;
		BOOL hResult = ::DeviceIoControl(this->hDevice, IOCTL_CDROM_DISK_TYPE, NULL, NULL, &diskType, sizeof(diskType), &returned, NULL);
		if (hResult == 0x00)
			{
			err = GNR_READ_ERROR;
			type = PHDT_NONE;
			}
		}

	// find out which type of disk.
	// if only audio tracks, it is simple.
	// else, try to read a data sector in the different forms.
	if (!IS_ERROR(err))
		{
		if (diskType.DiskData == CDROM_DISK_AUDIO_TRACK)
			{
			type = PHDT_CDDA;
			}
		else
			{
			type = PHDT_CDROM_XA;
			}
		}

	return err;
	}


//! get number of sessions.
// do not return real number of sessions, but maximum 2 Sessions if Multisession,
// cause you only need the last session.
// see also ReadCDToc
//
Error XBOXDVDDrive::GetNumberOfSessions(WORD & num)
	{
	VDAutoMutex autoMutex(&monitorMutex);
	Error err = GNR_OK;

	SessionInfo sessionInfo = {0};
	WORD numSessions = 0;

	// get device for DVD drive
	if (!IS_ERROR(err))
		{
		err = this->GetDevice();
		}

	// get last session information
	if (!IS_ERROR(err))
		{
		// the Microsoft Documentation on this IOCTL (documentation included in W2000DDK) is wrong.
		// it claims this call has no input and output parameters, but you will get the struct defined below (see ATAPI Docu)
		DWORD returned = 0;
		BOOL hResult = ::DeviceIoControl(this->hDevice, IOCTL_CDROM_GET_LAST_SESSION, NULL, NULL, &sessionInfo, sizeof(sessionInfo), &returned, NULL);
		if (hResult == 0x00)
			{
			err = GNR_READ_ERROR;
			}
		}

	// calculate number of sessions

	if (!IS_ERROR(err))
		{
		if (sessionInfo.firstSession != sessionInfo.lastSession)
			numSessions = 2;
		else
			numSessions = 1;
		}

	// return result.
	if (IS_ERROR(err))
		num = 0;
	else
		num = numSessions;

	return err;
	}

