////////////////////////////////////////////////////////////////////
//
//  Small Form Factor CDVD Drive Class
//
////////////////////////////////////////////////////////////////////

#ifndef SFFCDVDDRIVE_H
#define SFFCDVDDRIVE_H

#include "CDVDDrive.h"
#include "library\common\vddebug.h"

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
		BYTE					agid;					// For DVD authentication
		int					lockCount;			// Number of times tray was locked

		BYTE				*	flagsPtr;			// SFF comand pointer
		PVOID				*	buffPtr;				// SFF comand pointer
		DWORD				*	buffLenPtr;			// SFF comand pointer
		BYTE				*	cdbLenPtr;			// SFF comand pointer
		BYTE				*	cdbPtr;				// SFF comand pointer

		BOOL					isStreaming;

		DriveBlockBuffer	*	blockBuffer;	// Data storage

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

		//
		//  Data Access
		//

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, void * buffer, DWORD flags);
		virtual Error ReadDVDForced(DWORD block, DWORD num, void * buffer, DWORD flags);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, DiskSectorType sectorType, void * buffer);

		//
		//  Internal helper functions
		//

				  void  TranslateSenseInformation(BYTE * senseArea, Error & err, BOOL & retry);
		virtual void  SetStreamingMode(BOOL streaming) {isStreaming = streaming;}
		virtual void  SetSubChannelDefaultValues(BYTE * subChannelInfo, CDATime start, CDATime length);
		virtual Error Inquiry(BOOL & removable, BYTE & deviceType);

		//
		//  Internal functions to be implemented specifically
		//

		virtual void  EnterSection(void) = 0;
		virtual void  LeaveSection(void) = 0;
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
		virtual Error Flush(void) = 0;											// Make sure all commands are executed
		virtual Error Detach(void) = 0;											// Release drive and never access it again

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
		//  Data access functions
		//

		virtual Error SeekBlock(DWORD block, DWORD flags, DWORD & reqID);
		virtual Error LockBlocks(DWORD block, DWORD num, DriveBlock * blocks, DWORD flags, DWORD & reqID);
		virtual Error UnlockBlocks(DWORD block, DWORD num, DWORD flags, DWORD & reqID);

		virtual Error GetDriveBlockSize(DWORD & currentSize, DWORD & minSize, DWORD & maxSize, DWORD & step);
		virtual Error SetDriveBlockSize(DWORD size);

		virtual Error GetNumberOfDriveBlocks(DWORD & num);
		virtual Error SetNumberOfDriveBlocks(DWORD num);

		//
		//  DVD authentication and region coding
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
