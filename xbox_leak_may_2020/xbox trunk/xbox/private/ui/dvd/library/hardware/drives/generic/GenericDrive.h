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
//  Generic Drive Classes
//
////////////////////////////////////////////////////////////////////

#ifndef GENERICDRIVE_H
#define GENERICDRIVE_H

#include "library/common/prelude.h"
#include "library/common/krnlstr.h"
#include "library/common/krnlsync.h"
#include "library/common/Profiles.h"
#include "library/hardware/drives/generic/DriveDefs.h"
#include "library/hardware/drives/generic/Requests.h"
#include "library/hardware/drives/generic/DriveErrors.h"
#include "library/Common/KrnlInt.h"

//
//  Flag format for disk access is as follows:
//  ffffffff ffffffff cccccccc tttttttt
//  f: disk access flag
//  c: disk access command
//  t: disk sector type
//

#define GD_SECTOR_TYPE_MASK			0x000000ff
#define GD_COMMAND_MASK					0x0000ff00
#define GD_FLAGS_MASK					0xffff0000

#define GD_SECTOR_TYPE(flags)			(flags & GD_SECTOR_TYPE_MASK)
#define GD_COMMAND(flags)				(flags & GD_COMMAND_MASK)
#define GD_FLAGS(flags)					(flags & GD_FLAGS_MASK)

//
//  Disk Sector Types. Note that the macro DST_TYPE_IS_CDROM(x) depends on the order of the values.
//

#define DST_NONE							0x00000000
#define DST_DVD_ROM						0x00000001
#define DST_CDDA							0x00000002		// Including subchannel
#define DST_CDROM_MODE1					0x00000003		// order of CDROM sector types is important, see DST_TYPE_IS_CDROM macro
#define DST_CDROM_MODE2					0x00000004		// order of CDROM sector types is important, see DST_TYPE_IS_CDROM macro
#define DST_CDROM_MODE2_XA_FORM1		0x00000005		// order of CDROM sector types is important, see DST_TYPE_IS_CDROM macro
#define DST_CDROM_MODE2_XA_FORM2		0x00000006		// order of CDROM sector types is important, see DST_TYPE_IS_CDROM macro
#define DST_HD								0x00000007

#define DST_TYPE_IS_CDROM(x)			(DST_CDROM_MODE1 <= (x)  &&  (x) <= DST_CDROM_MODE2_XA_FORM2)

//
//  Disk Access Commands
//

#define DAT_NONE							0x00000000		// No action
#define DAT_LOCK_MODIFY					0x00000100		// Lock buffer to modify it (without reading data first)
#define DAT_LOCK_AND_READ				0x00000200		// Lock buffer, then read data into it
#define DAT_LOCK_READ_MODIFY			0x00000300		// Lock buffer, read data to it and allow modification
#define DAT_UNLOCK_CLEAN				0x00000400		// Unlock buffer, discard any changes
#define DAT_UNLOCK_DIRTY				0x00000500		// Unlock buffer and write contents to disk

//
//  Disk Access Flags
//

#define DAF_NONE							0x00000000		// Nothing at all
#define DAF_FORCED						0x00010000		// Force reading data (drive level)
#define DAF_STREAMING					0x00020000		// Streaming mode, prefetch might be used (drive level)
#define DAF_READ_SUBCHANNEL			0x00040000		// Read subchannel information (drive level)
#define DAF_CACHED						0x00080000		// Cache data (volume level)
#define DAF_SCANNING						0x00100000		// Scanning mode, suppress read ahead in drive

//
//  Drive Capabilities (for GetDriveCapabilities() call)
//

#define GDC_NONE							0x0000			// Nothing at all
#define GDC_REMOVABLE					0x0001			// The media can be removed
#define GDC_WRITEABLE					0x0002			// The drive supports writing on media
#define GDC_CHANGER						0x0004			// The drive has media change capabilities
#define GDC_DVD_AUTHENTICATION		0x0008			// Drive supports DVD authentication mechanisms
#define GDC_READ_RECORDABLE			0x0010			// Drive supports reading recordable media
#define GDC_SUPPORTS_SUBCHANNEL		0x0020			// Drive supports reading (CDDA) subchannel info
#if MSNDISC
#define GDC_CHANGER_ROTATES_CLOCKWISE					0x0040	// Drive can rotate changer mechanism clockwise
#define GDC_CHANGER_ROTATES_COUNTERCLOCKWISE			0x0080	// Drive can rotate changer mechanism counter-clockwise
#define GDC_SUPPORTS_PUSH_SENSE							0x0100	// Drive supports push sense detection
#endif

//
//  Flags for LoadMedia() call
//

#define LMF_NONE							0x0000			// Nothing special
#define LMF_WAIT							0x0001			// Wait till load process is complete
#define LMF_NO_SPIN_UP					0x0002			// Do not spin up drive after load

//
//  Flags for UnloadMedia() call
//

#define UMF_NONE							0x0000			// Nothing special
#define UMF_WAIT							0x0001			// Wait till unload process is complete

//
//  Flags for MoveChanger call
//

#if MSNDISC
#define MCO_OPEN									0x0001		// Open changer - provide access to 1 or more media
#define MCO_CLOSE									0x0002		// Close changer - prevent access to media
#define MCO_ROTATE_CLOCKWISE					0x0004		// Rotate changer clockwise
#define MCO_ROTATE_COUNTER_CLOCKWISE		0x0008		// Rotate changer counter clockwise
#define MCO_ROTATE_TO_POSITION				0x0010		// Rotate changer to a position. Position is defined by specific implementation.
#define MCO_CLAMP									0x0020		// Clamp media
#define MCO_UNCLAMP								0x0040		// Unclamp media
#define MCO_ACKNOWLEDGE_PUSH_SENSE			0x0080		// Acknowledge push sense detection

//
//  Flags for GetChangerStatus call
//

#define GCS_UNKNOWN								0x0001		// Changer mechanism is in an unknown state
#define GCS_ERROR									0x0002		// Changer mechanism is in an error state
#define GCS_BUSY									0x0004		// Changer mechanism is not ready

#define GCS_OPEN									0x0008		// Changer mechanism is open, user has access to 1 or more media
#define GCS_CLOSED								0x0010		// Changer mechanism is closed, user has no access to media
#define GCS_OPENING								0x0020		// Changer mechanism is in the process of going to the open state
#define GCS_CLOSING								0x0040		// Changer mechanism is in the process of going to the closed state
#define GCS_ROTATING_CLOCKWISE				0x0080		// Changer mechanism is rotating clockwise
#define GCS_ROTATING_COUNTER_CLOCKWISE		0x0100		// Changer mechanism is rotating counter clockwise
#define GCS_AT_SLOT								0x0200		// Changer mechanism current position is a slot

#define GCS_CLAMP_UNKNOWN						0x0400		// Changer mechanism clamp state is unknown
#define GCS_CLAMPED								0x0800		// Changer mechanism has clamped the media
#define GCS_UNCLAMPED							0x1000		// Changer mechanism has unclamped the media
#define GCS_CLAMPING								0x2000		// Changer mechanism is clamping the media
#define GCS_UNCLAMPING							0x4000		// Changer mechanism is unclamping the media


//
// Flags for GetSlotStatus call
//
#define GSS_UNKNOWN								0x0001		// Slot status unknown
#define GSS_ERROR									0x0002		// Disc present mechanism in error state
#define GSS_BUSY									0x0004		// Disc present mechanism is busy
#define GSS_DISC_PRESENT						0x0008		// Disc present in slot
#define GSS_DISC_NOT_PRESENT					0x0010		// Disc not preset in slot


#endif

//
//  The type of drive we are talking to
//

enum GenericDriveType
	{
	GDRT_NONE,
	GDRT_GENERIC,
	GDRT_CDVD,
	GDRT_HD
	};

//
//  Volume ID (unique identifier for a volume)
//

typedef KernelInt64 VolumeID;

////////////////////////////////////////////////////////////////////
//
//  Drive Block Class
//  NOTE: None of the data contained in this class shall be modified
//  outside the DriveBlockBuffer or derived classes.
//
////////////////////////////////////////////////////////////////////

enum DriveBlockLockMode { LM_NONE, LM_READ, LM_WRITE };

class DriveBlock
	{
	protected:
		BYTE					*	realData;	// Data of the block
	public:
		DWORD						block;		// Index of the block
		BYTE					*	data;			// Data of the block with offset skipping headers
		DriveBlockLockMode	lockMode;	// Locked for read or write
		Error						err;			// Error that occurred reading this block
		DWORD						private0;	// Driver may store info here

		void Copy(DriveBlock * db);		// Copy management data to another drive block (NO BLOCK DATA!)
	};

////////////////////////////////////////////////////////////////////
//
//  Drive Control Information
//
//  This class encapsulates the information necessary for a drive
//  to initialize itself and do accesses via the OS.
//  This is usually generated by a DriveTable object.
//
////////////////////////////////////////////////////////////////////

class DriveControlInfo
	{
	public:
		KernelString driveName;

		DriveControlInfo(void) { driveName = ""; }
		DriveControlInfo(const KernelString & driveName) { this->driveName = driveName; }
		virtual ~DriveControlInfo(void) {}
	};

////////////////////////////////////////////////////////////////////
//
//  Generic Drive Class Declaration
//
//  This class declares a generic drive with basic management
//  functions. Note that this class declares block oriented drives.
//
////////////////////////////////////////////////////////////////////

class GenericDrive
	{
	protected:
		VDLocalMutex monitorMutex;
		DriveControlInfo * controlInfo;
		GenericProfile *profile;
		BOOL requestsEnabled;   // when FALSE, all drive requests should be rejected and return GNR_OPERATION_ABORTED

	public:
		GenericDrive(void);
		virtual ~GenericDrive(void);

		virtual Error Init(DriveControlInfo * info, GenericProfile * profile);
		virtual Error Cleanup(void);

		//
		//  Inquiry & config functions
		//

		virtual Error GetDriveName(KernelString & name);
#if MSNDISC
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots, DWORD& positions, DWORD& changerOpenOffset);
#else
		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
#endif
		virtual Error GetDriveBlockSize(DWORD sectorType, DWORD & currentSize, DWORD & minSize, DWORD & maxSize, DWORD & step) = 0;
		virtual Error GetCurrentDriveBlockSize(DWORD & size);
		virtual Error SetDriveBlockSize(DWORD size) = 0;

		virtual Error GetNumberOfDriveBlocks(DWORD & num) = 0;
		virtual Error SetNumberOfDriveBlocks(DWORD num) = 0;

		virtual Error GetBlockInfo(DWORD sectorType, DWORD & blockSize, DWORD & headerSize, DWORD & dataSize);

		//
		//  Basic Drive Functions
		//

		virtual Error SpinUp(void) = 0;													// Spin up drive
		virtual Error SpinDown(void) = 0;												// Spin down drive
		virtual Error TerminateRequest(RequestHandle * rh) = 0;					// Terminate the current request (read/write)
		virtual Error Flush(void) = 0;													// Make sure all commands are executed

		//
		//  Media handling
		//


#if MSNDISC
		virtual Error LoadMedia(int slotNumber = 0, DWORD flags = LMF_WAIT) = 0;	// load media in slot slotNumber
		virtual Error UnloadMedia(DWORD flags = UMF_WAIT) = 0;							// remove current media
		virtual Error MoveChanger( DWORD operation, DWORD flags, DWORD param )=0;	// Move Changer
		virtual Error GetChangerStatus( DWORD& status, DWORD& position)=0;									// Get Changer Status
		virtual Error GetSlotStatus( DWORD slot, DWORD& status )=0;
		virtual Error LockClamp( void )=0;														// Lock clamp
		virtual Error UnlockClamp( void )=0;													// Unlock clamp

#else
		virtual Error LoadMedia(int slotNumber = 0, DWORD flags = LMF_WAIT) = 0;	// Close Tray, load media in slot slotNumber
		virtual Error UnloadMedia(DWORD flags = UMF_WAIT) = 0;							// Open Tray, remove current media

		virtual Error GetTrayStatus(GenericTrayStatus & status) = 0;			// Get Tray Status, was: GetDriveStatus
		virtual Error LockTray(void) = 0;												// Lock Tray
		virtual Error UnlockTray(void) = 0;												// Unlock Tray
#endif

		virtual Error GetDiskType(PhysicalDiskType & type) = 0;					// DVD, CD Rom, Audio CD, ...
		virtual Error GetVolumeID(VolumeID & volumeID) = 0;
		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi) = 0;

		//
		//  Data Access Functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, RequestHandle * rh) = 0;
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh) = 0;
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, RequestHandle * rh) = 0;

		virtual void DisableRequests (void) {requestsEnabled = FALSE;}
		virtual void EnableRequests (void) {requestsEnabled = TRUE;}
	};

////////////////////////////////////////////////////////////////////
//
//  CDVD Drive
//
////////////////////////////////////////////////////////////////////

#ifndef CDVDDRIVE_H
#define CDVDDRIVE_H

#include "GenericDrive.h"
#include "CDVDDefs.h"

class CDVDDrive : public GenericDrive
	{
	protected:

		//
		//  DVD Authentication
		//
		virtual Error StartAuthentication (void) = 0;
		virtual Error CompleteAuthentication (void) = 0;
		virtual Error CancelAuthentication (void) = 0;
		virtual Error GetChallengeKey (BYTE * key) = 0;
		virtual Error SendChallengeKey (BYTE * key) = 0;
		virtual Error GetBusKey (BYTE * key) = 0;
		virtual Error SendBusKey (BYTE * key) = 0;
		virtual Error GetDiskKey (BYTE * key) = 0;
		virtual Error GetTitleKey (DWORD block, BYTE * key) = 0;

	public:
		CDVDDrive (void) {}
		virtual ~CDVDDrive (void);

		//
		//  DVD Authentication
		//

		virtual Error DVDIsEncrypted (BOOL & enc) = 0;
		virtual Error DoAuthenticationCommand (DVDAuthenticationCommand com, DWORD sector, BYTE * key);
		virtual Error GetRPCData (BOOL & isRPC2, BYTE & regionCode, BYTE & availSets) = 0;
		virtual Error SetRegionCode (BYTE region) = 0;

		//
		//  CD Access Methods
		//

		virtual Error GetNumberOfSessions (WORD & sessions) = 0;

		virtual Error ReadCDTOC (WORD session, CDSessionType & sessionType, CDTocEntry * & toc, WORD & tocEntryNum, RequestHandle * rh) = 0;
			// Drive creates TOC entries, to be deleted by caller
	};

////////////////////////////////////////////////////////////////////
//
//  Interrupt CE Drive
//
////////////////////////////////////////////////////////////////////


#endif // of CDVDDRIVE_H

#endif // of GENERICDRIVE_H
