////////////////////////////////////////////////////////////////////
//
//  PC CDVD Drive Implementation
//
////////////////////////////////////////////////////////////////////

#ifndef PCDVDRIVE_H
#define PCDVDRIVE_H

#include "CDVDDrive.h"
#include "library\support\aspi32\wnaspi32.h"
#include "library\common\vddebug.h"
#include "y:\winntddk\src\storage\inc\ntddscsi.h"

//////////////////////////////////////////////////////////////////////
//
//  Small Form Factor Drive Commands
//
//////////////////////////////////////////////////////////////////////

#define SFF_REQUEST_SENSE				0x03
#define SFF_MODE_SENSE					0x5a
#define SFF_MODE_SELECT					0x55
#define SFF_READ_CD_MSF					0xb9
#define SFF_READ_CD						0xbe
#define SFF_READ_DISC_INFO				0x51
#define SFF_READ_DVD_STRUCTURE		0xad
#define SFF_READ_TOC						0x43
#define SFF_DVD_READ						0xa8
#define SFF_START_STOP_UNIT			0x1b
#define SFF_MEDIA_REMOVAL				0x1e
#define SFF_SEND_KEY						0xa3
#define SFF_REPORT_KEY					0xa4
#define SFF_INQUIRY						0x12

//////////////////////////////////////////////////////////////////////
//
//  Small Form Factor CDVD Drive
//
//////////////////////////////////////////////////////////////////////

class SFFCDVDDrive : public CDVDDrive
	{
	protected:
		BYTE					agid;
		int					lockCount;
		BYTE				*	flagsPtr;
		PVOID				*	buffPtr;
		DWORD				*	buffLenPtr;
		BYTE				*	cdbLenPtr;
		BYTE				*	cdbPtr;
		BOOL					isStreaming;
		CRITICAL_SECTION	lock;

		PCDriveBlockBuffer blockBuffer;

		BOOL IsDVDDrive(void);		// Check if this is really a DVD drive

		//
		//  DVD Authentication
		//

		virtual Error StartAuthentication(void);
		virtual Error CompleteAuthentication(void);
		virtual Error CancelAuthentication(void);
		virtual Error GetChallengeKey(BYTE * key);
		virtual Error SendChallengeKey(BYTE * key);
		virtual Error GetBusKey(BYTE * key);
		virtual Error SendBusKey(BYTE * key);
		virtual Error GetDiskKey(BYTE * key);
		virtual Error GetTitleKey(DWORD block, BYTE * key);

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, void * buffer);
		virtual Error ReadDVDForced(DWORD block, DWORD num, void * buffer);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, DiskSectorType sectorType, void * buffer);

		//
		//  Internal helper functions
		//

		void TranslateSenseInformation(BYTE * senseArea, Error & err, BOOL & retry);
		virtual void SetStreamingMode(BOOL streaming) {isStreaming = streaming;}
		virtual void SetSubChannelDefaultValues(BYTE * subChannelInfo, CDATime start, CDATime length);


		//
		//  Internal functions to be implemented specifically
		//

		virtual void EnterSection(void) = 0;
		virtual void LeaveSection(void) = 0;
		virtual Error DoCommand(void) = 0;

	public:
		SFFCDVDDrive(void);
		SFFCDVDDrive(const KernelString & driveName, DWORD conFlags);
		virtual ~SFFCDVDDrive(void);

		virtual Error Init(const KernelString & driveName, DWORD conFlags);
		virtual Error Cleanup(void);

		//
		//  Inquiry & config functions
		//

		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
		virtual Error Configure(DWORD conFlags);

		//
		//  Basic Drive Functions
		//

		virtual Error SpinUp(void);												// Spin up drive
		virtual Error SpinDown(void);												// Spin down drive
		virtual Error TerminateRequest(DWORD reqID);							// Terminate the current request (read/write)
		virtual Error Flush(void);													// Make sure all commands are executed
		virtual Error Detach(void);												// Release drive and never access it again

		//
		//  Media handling
		//

		virtual Error LoadMedia(int slotNumber = 0, BOOL wait = TRUE);	// Close Tray, load media in slot slotNumber
		virtual Error UnloadMedia(BOOL wait = TRUE);							// Open Tray, remove current media

		virtual Error GetTrayStatus(GenericTrayStatus & status);			// Get Tray Status, was: GetDriveStatus
		virtual Error LockTray(void);												// Lock Tray
		virtual Error UnlockTray(void);											// Unlock Tray

		virtual GenericDiskType GetDiskType(void);							// DVD, CD Rom, Audio CD, ...
		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & copyManagementInfo);

		//
		//  Data Access Functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, DWORD & reqID);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, DWORD & reqID);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DWORD flags, DWORD & reqID);

		virtual Error GetDriveBlockSize(DWORD & currentSize, DWORD & minSize, DWORD & maxSize, DWORD & step);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		//
		//  DVD Authentication
		//

		virtual BOOL  DVDIsEncrypted(void);
		virtual Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key)
		virtual Error GetRPCData(BYTE & regionCode, BYTE & availSets, BOOL & isRPC2);
		virtual Error SetRegionCode(BYTE region);

		//
		//  CD Access Methods
		//

		virtual WORD  GetNumberOfSessions(void);
		virtual Error ReadCDTOC(WORD session, CDTocEntry * toc, int & tocEntryNum, DWORD & reqID);

	};

//////////////////////////////////////////////////////////////////////
//
//  PCDVDrive Class
//
//////////////////////////////////////////////////////////////////////

class PCDVDrive : public CDVDDrive
	{
	protected:
		BYTE					agid;
		int					lockCount;
		BYTE				*	flagsPtr;
		PVOID				*	buffPtr;
		DWORD				*	buffLenPtr;
		BYTE				*	cdbLenPtr;
		BYTE				*	cdbPtr;
		BOOL					isStreaming;
		CRITICAL_SECTION	lock;

		PCDriveBlockBuffer blockBuffer;

		BOOL IsDVDDrive(void);		// Check if this is really a DVD drive

		//
		//  DVD Authentication
		//

		virtual Error StartAuthentication(void);
		virtual Error CompleteAuthentication(void);
		virtual Error CancelAuthentication(void);
		virtual Error GetChallengeKey(BYTE * key);
		virtual Error SendChallengeKey(BYTE * key);
		virtual Error GetBusKey(BYTE * key);
		virtual Error SendBusKey(BYTE * key);
		virtual Error GetDiskKey(BYTE * key);
		virtual Error GetTitleKey(DWORD block, BYTE * key);

		virtual Error DoCommand(void) = 0;
		void TranslateSenseInformation(BYTE * senseArea, Error & err, BOOL & retry);

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, void * buffer);
		virtual Error ReadDVDForced(DWORD block, DWORD num, void * buffer);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, DiskSectorType sectorType, void * buffer);

		virtual void SetStreamingMode(BOOL streaming) {isStreaming = streaming;}

		virtual void SetSubChannelDefaultValues(BYTE * subChannelInfo, CDATime start, CDATime length);

		void EnterSection(void);
		void LeaveSection(void);

	public:
		PCDVDrive(void);
		PCDVDrive(const KernelString & driveName, DWORD conFlags);
		virtual ~PCDVDrive(void);

		virtual Error Init(const KernelString & driveName, DWORD conFlags);
		virtual Error Cleanup(void);

		//
		//  Inquiry & config functions
		//

		virtual Error GetDriveCaps(GenericDriveType & type, DWORD & caps, DWORD & slots);
		virtual Error Configure(DWORD conFlags);

		//
		//  Basic Drive Functions
		//

		virtual Error SpinUp(void);												// Spin up drive
		virtual Error SpinDown(void);												// Spin down drive
		virtual Error TerminateRequest(DWORD reqID);							// Terminate the current request (read/write)
		virtual Error Flush(void);													// Make sure all commands are executed
		virtual Error Detach(void);												// Release drive and never access it again

		//
		//  Media handling
		//

		virtual Error LoadMedia(int slotNumber = 0, BOOL wait = TRUE);	// Close Tray, load media in slot slotNumber
		virtual Error UnloadMedia(BOOL wait = TRUE);							// Open Tray, remove current media

		virtual Error GetTrayStatus(GenericTrayStatus & status);			// Get Tray Status, was: GetDriveStatus
		virtual Error LockTray(void);												// Lock Tray
		virtual Error UnlockTray(void);											// Unlock Tray

		virtual GenericDiskType GetDiskType(void);							// DVD, CD Rom, Audio CD, ...
		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & copyManagementInfo);

		//
		//  Data Access Functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, DWORD & reqID);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, DWORD & reqID);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DWORD flags, DWORD & reqID);

		virtual Error GetDriveBlockSize(DWORD & currentSize, DWORD & minSize, DWORD & maxSize, DWORD & step);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		//
		//  DVD Authentication
		//

		virtual BOOL  DVDIsEncrypted(void);
		virtual Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key)
		virtual Error GetRPCData(BYTE & regionCode, BYTE & availSets, BOOL & isRPC2);
		virtual Error SetRegionCode(BYTE region);

		//
		//  CD Access Methods
		//

		virtual WORD  GetNumberOfSessions(void);
		virtual Error ReadCDTOC(WORD session, CDTocEntry * toc, int & tocEntryNum, DWORD & reqID);
	};

//////////////////////////////////////////////////////////////////////
//
//  ASPI CDVD Drive Class
//
//////////////////////////////////////////////////////////////////////


#define VWIN32_DIOC_DOS_IOCTL 1 

typedef struct _DEVIOCTL_REGISTERS
	{
	DWORD reg_EBX; 
	DWORD reg_EDX; 
	DWORD reg_ECX; 
	DWORD reg_EAX; 
	DWORD reg_EDI; 
	DWORD reg_ESI; 
	DWORD reg_Flags; 
	} DEVIOCTL_REGISTERS, *PDEVIOCTL_REGISTERS; 

class ASPICDVDDrive : public PCDVDrive
	{
	protected:
		SRB_ExecSCSICmd	cmd;

		HANDLE				aspiCompletionEvent;

		DWORD					driveNumber;

		void *				cdaBuffer;			// Place to store mixed audio/subchannel data

		CDATime				cdaBufferLength;	// Size of the buffer

		BYTE	*				tocBuffer;			// Place for CD TOC information
		DWORD					tocBufferSize;		// Size of the TOC buffer
		WORD					numSessions;		// Number of sessions on CD

		//
		//  Internal functions
		//

		BOOL DoIOCTL(PDEVIOCTL_REGISTERS preg);

		virtual void EnterSection(void);
		virtual void LeaveSection(void);
		virtual Error DoCommand(void);

		void SetStreamingMode(BOOL streaming);

	public:
		ASOICDVDDrive(void);
		ASPICDVDDrive(WORD adapter, WORD id, DWORD driveNumber);
		virtual ~ASPICDVDDrive(void);

		virtual Error Init(const KernelString & driveName, DWORD conFlags);
		virtual Error Cleanup(void);

		virtual Error Inquiry(BOOL & removable, BYTE & deviceType)
			{removable = TRUE; deviceType = 0x5; return TRUE;}

		virtual Error LockTray(void);
		virtual Error UnlockTray(void);
		virtual Error UnloadMedia(BOOL wait = TRUE);

		virtual Error SeekBlock(DWORD block, DWORD flags, DWORD & reqID);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, DWORD & reqID);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, DWORD & reqID);

		virtual WORD  GetNumberOfSessions(void);
		virtual Error ReadCDTOC(WORD session, CDTocEntry * toc, int & tocEntryNum, DWORD & reqID);
	};

//////////////////////////////////////////////////////////////////////
//
//   NT4 CDVD Drive Class
//
//////////////////////////////////////////////////////////////////////

//
//  Some defines
//

typedef struct _SCSI_PASS_THROUGH_WITH_BUFFERS
	{
	SCSI_PASS_THROUGH spt;
	ULONG             Filler;      // realign buffers to double word boundary
	UCHAR             ucSenseBuf[32];
	UCHAR             ucDataBuf[2352];
	} SCSI_PASS_THROUGH_WITH_BUFFERS, *PSCSI_PASS_THROUGH_WITH_BUFFERS;

typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER
	{
	SCSI_PASS_THROUGH_DIRECT sptd;
	ULONG             Filler;      // realign buffer to double word boundary
	UCHAR             ucSenseBuf[32];
	} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;

//
//  NT4 CDVD Drive
//

class NT4CDVDDrive : public PCDVDrive
	{
	protected:
		HANDLE	hDevice;

		BYTE		dummyFlag;
		LPVOID	bufferPtr;
	
		SCSI_PASS_THROUGH_WITH_BUFFERS	sptdwb;

		TCHAR		deviceName[16];

		BOOL rpc2Check;

		//
		//  Internal functions
		//

		virtual Error OpenDevice(bool closeFirst = FALSE);

		virtual void EnterSection(void);
		virtual void LeaveSection(void);
		virtual Error DoCommand(void);

		void SetStreamingMode(BOOL streaming);

	public:
		NT4CDVDDrive(KernelString devName);
		virtual ~NT4CDVDDrive(void);
		
		virtual Error ReadBlocks(DWORD block, DWORD num, void * buffer, BOOL streaming);
		virtual Error ReadForced(DWORD block, DWORD num, void * buffer, BOOL streaming);
		virtual Error ReadCDABlocks(CDATime start, CDATime length, void * buffer, void * subChannels);
		virtual Error ReadCDAToc(CDATocEntry * toc, int & tocEntryNum);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, void * buffer, CDSectorType type, BOOL streaming = FALSE);

		virtual Error GetRPCData(BYTE & regionCode, BYTE & availResets, BOOL & isRPC2);
	};

#endif
