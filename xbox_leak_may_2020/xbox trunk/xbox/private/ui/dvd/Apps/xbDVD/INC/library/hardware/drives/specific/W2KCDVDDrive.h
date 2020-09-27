////////////////////////////////////////////////////////////////////
//
//  NT5 CDVD Drive Class
//
////////////////////////////////////////////////////////////////////

#ifndef W2KCDVDRVIVE_H
#define W2KCDVDRVIVE_H

#include "cdvddrive.h"
#include <devioctl.h>
#include <ntddstor.h>
#pragma warning(disable : 4200)
#include "y:\w2000ddk\src\storage\inc\ntddcdvd.h"
#pragma warning(default : 4200)
#include "library\common\krnlsync.h"


class W2KCDVDDrive : public CDVDDrive
	{
	protected:
		DVD_SESSION_ID sessionId;
		TCHAR				deviceName[16];
		int				lockCount;
		HANDLE			hDevice;		
		VDLocalMutex	mutex;

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
		Error				ReadCDASubchannel(void __far * subchannel);

	public:
		W2KCDVDDrive(KernelString devName);
		~W2KCDVDDrive();

		virtual Error ReadBlocks(DWORD block, DWORD num, void * buffer, BOOL streaming);
		virtual Error ReadForced(DWORD block, DWORD num, void * buffer, BOOL streaming);
		virtual Error ReadCDABlocks(CDATime start, CDATime length, void * buffer, void * subChannel);
		virtual Error ReadCDAToc(CDATocEntry * toc, int & tocEntryNum);
		virtual Error ReadCDBlocks(DWORD block, DWORD num, void * buffer, CDSectorType type, BOOL streaming = FALSE);

		virtual BOOL  IsEncrypted(void);
		virtual BOOL  IsDVDDrive(void);
		virtual Error DoAuthenticationCommand(DVDAuthenticationCommand com, DWORD sector, BYTE * key);

		virtual Error Inquiry(BOOL & removable, BYTE & deviceType);
		virtual Error WakeUpDrive(void) {GNRAISE_OK;}

		virtual Error Lock(void);
		virtual Error Unlock(void);
		virtual Error LoadMedia(int slotNumber = 0, BOOL wait = FALSE);
		virtual Error UnloadMedia(BOOL wait = FALSE);
		virtual Error GetDriveStatus(GenericTrayStatus & status);
		
		virtual Error GetRPCData(BYTE & regionCode, BYTE & availResets, BOOL & isRPC2);
		virtual Error SetRegion(BYTE regionCode);

		virtual Error GetCopyManagementInfo(DWORD block, GenericCopyManagementInfo & gcmi);
	};

#endif
