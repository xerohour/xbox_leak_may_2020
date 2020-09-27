
// FILE:      library\hardware\drives\specific\dvdatapi.h
// AUTHOR:    D. Heidrich
// COPYRIGHT: (c) 1999 Viona Development.  All Rights Reserved.
// CREATED:   04.02.99
//
// PURPOSE:   General ATAPI DVD drive for CE.
//
// HISTORY:

#ifndef DVDATAPI_H
#define DVDATAPI_H

#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\common\krnlsync.h"
#include "library\common\winports.h"
#include "library\hardware\drives\generic\cdvddrv.h"
#include "library\hardware\mpeg2dec\generic\mp2dcryp.h"
#include "library\hardware\uproc\specific\st55xx\frnt5505.h"

#include <semaphor.h>



class DVDAtapiDrive : public GenericCDVDDrive, public MPEG2DVDDecryption
	{
	friend void DVDAtapi_HandlerTask (void *drive);

	private:
		BYTE lastModeClose;   // for Sanyo door open/close 

		MemoryMappedIO *port;
		MemoryMappedIO *controlPort;
		MemoryMappedIO *dataPort;
		MemoryMappedIO *epldPort;
		STi5505FrontEnd *frontEnd;
		WinPort *winPort;

		VDMutex monitorMutex;

		BOOL firstBlockWasRead;

		enum TrayStatus
			{
			TRAY_UNKNOWN, TRAY_CLOSING, TRAY_CLOSED, TRAY_OPENING, TRAY_OPEN
			} trayStatus;
		BOOL diskIsIdentified;
		BOOL traySuddenlyOpened;
		clock_t loadMediaTimeOut;

		semaphore_t irqSema;
		int diskTrayLocked;
		int currentSlotNumber;
		BOOL dataHold;
		BYTE status[6];
		BYTE sector[3];
		BYTE layer;
		BYTE playMode;
		int servoState;

		// Additional disk information, valid if LOAD_MEDIA succeeds.
		BOOL dvdMode;
		int agid;

		BOOL dualLayer;
		BOOL oppositeTrack;
		BOOL encryptedDisk;
		int regionCode;
		DWORD dataStartSector;		// usually 0x30000
		DWORD dataEndSector;
		DWORD layerZeroEndSector;
		int firstTOC, lastTOC;   // for CD mode only
		int leadOutMin, leadOutSec, leadOutFrm;   // for CD mode only

		Error CheckDriveReady (void);
		Error RequestSense (void);
		Error WaitForIRQ (void);
		Error ReadStatus (void);


		DWORD packetArray[3];
		Error DoPacket (void *buffer, int length, BOOL write = FALSE, BOOL useDMA = FALSE);


		enum Command
			{
			CMD_ILLEGAL,				// internal command
			CMD_TICK,					// internal command
			CMD_WAKEUPDRIVE,			// internal command
			CMD_GET_COPY_INFO,		// internal command
			CMD_START_AUTHENTICATION,		// internal command
			CMD_CANCEL_AUTHENTICATION,		// internal command
			CMD_GET_BUSKEY,			// internal command
			CMD_SEND_BUSKEY,			// internal command
			CMD_GET_CHALLENGEKEY,	// internal command
			CMD_SEND_CHALLENGEKEY,	// internal command
			CMD_GET_DISKKEY,			// internal command
			CMD_GET_TITLEKEY,			// internal command
			CMD_WRITE_DISKKEY,		// internal command
			CMD_WRITE_TITLEKEY,		// internal command
			CMD_UNLOCK_BUFFERS,		// internal command
			CMD_READ_TOC,				// internal command
			CMD_SET_READMODE,			// internal command
			CMD_SEEKBLOCK,				// internal command
			CMD_READBLOCKS,
			CMD_READFORCED,
			CMD_LOCK,
			CMD_UNLOCK,
			CMD_LOADMEDIA,
			CMD_UNLOADMEDIA,
			CMD_STOP,
			CMD_PAUSE
			};

		Error HandlerCommand (Command command);


		// Access to any sector on the disk.
		Error InternalReadBlocks (DWORD sector, DWORD num, void __far * buffer, DriveStreamBlockBuffer * blockBuffer,
		                          BYTE *subChannels, CDSectorType type = CST_CD_DA);


		// Track buffer.
		BYTE *trackBufferPtr;
		BYTE *dataOverflowPtr;   // overflow area for FEC DMA
		int sectorSize;
		int sectorOffset;
		int totalSectors;

		struct BufferInfo {
			DWORD sectorNumber;
			DWORD lockCount;
			BYTE *dataPtr;
			DWORD dummy;   // align struct size to 16 bytes
			};
		BufferInfo *bufferInfoMap;
		FrontEnd5505BufferStatus *bufferStatusMap;   // for buffer locking


		// Handler task related.
		void HandlerTask (void);
		void StopTransfer (void);
		void FlushTrackbuffer (void);
		BOOL handlerTaskRunning;
		int startReadCount;
		CDSectorType currentCDSectorType;
		BOOL currentIsStreamingData;

		Error HandlerLoadMedia (int slotNumber, BOOL wait);
		Error HandlerUnloadMedia (BOOL wait);
		Error HandlerReadBlocks (DWORD sector, DWORD num, BYTE *buffer);

		semaphore_t newRequest;
		semaphore_t requestDone;
		struct {
			// input
			Command command;
			DWORD sector;
			BYTE *buffer;
			int num;
			CDSectorType sectorType;
			BOOL isStreamingData;
			DWORD numberOfSectors;
			DriveStreamBlockBuffer *blockBuffer;
			// output
			Error error;
			} request;
		DWORD timeOutOneSecond;
		DWORD timeOutOneMilliSecond;

		// These variables are used by both consumer and producer. They are sampled so they
		// don't have to be protected by a mutex.
		//VDMutex trackMutex;
		int validStart;
		int readStart;
		DWORD lockedEnteringFreeArea;
		DWORD lockedLeavingFreeArea;


		volatile BOOL insideReadRequest, terminateReadRequest;


	public:
		void DisplayDebugRecord (void);   // only for internal debugging!


	public:
		DVDAtapiDrive (MemoryMappedIO *port, MemoryMappedIO *controlPort, MemoryMappedIO *dataPort, MemoryMappedIO *epldPort,
		               STi5505FrontEnd *frontEnd, WinPort *winPort = NULL);
		virtual ~DVDAtapiDrive (void);

		virtual Error Initialize (void);

		virtual Error ReadDVDBlocks (DWORD block, DWORD num, void __far * buffer, BOOL streaming);
		virtual Error ReadDVDForced (DWORD block, DWORD num, void __far * buffer, BOOL streaming);
		virtual Error ReadAndLockStreamBlocks (DWORD block, DWORD num, DriveStreamBlockBuffer * blocks, BOOL streaming);
		virtual Error UnlockStreamBlocks (DWORD num, DriveStreamBlockBuffer * blocks);
		virtual BOOL PrefersStreamBlocks (void);

		virtual Error SeekDVDBlock (DWORD block, BOOL streaming);

		virtual Error TerminateRead (void);

		virtual Error Stop (void);
		virtual Error Pause (void);

		virtual Error StartAuthentication (void);
		virtual Error SendChallengeKey (BYTE __far * key);
		virtual Error GetBusKey (BYTE __far * key);
		virtual Error GetChallengeKey (BYTE __far * key);
		virtual Error SendBusKey (BYTE __far * key);
		virtual Error GetDiskKey (BYTE __far * key);
		virtual Error CompleteAuthentication (void);
		virtual Error CancelAuthentication (void);
		virtual Error GetTitleKey (DWORD block, BYTE __far * key);

		virtual BOOL IsEncrypted (void);
		virtual BOOL IsDVDDrive (void);

		virtual Error DVDInquiry (BOOL __far & removable, BYTE __far & deviceType);

		virtual Error Lock (void);
		virtual Error Unlock (void);
		virtual Error LoadMedia (int slotNumber, BOOL wait);
		virtual Error UnloadMedia (BOOL wait);
		virtual Error GetDriveStatus (GenericTrayStatus &status);

		virtual Error WakeUpDrive (void);

		virtual Error ReadCDBlocks (DWORD block, DWORD num, void __far * buffer, CDSectorType type, void __far * subChannels, BOOL streaming);
		virtual Error ReadTOC (CDATocEntry *toc, int &tocEntryNum);

		virtual Error SeekCDBlock (DWORD block, CDSectorType type, BOOL streaming);

		// Decryption cell programming.
		virtual Error DoAuthenticationCommand (MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);

		virtual GenericIRQTrigger GetInterruptTrigger (void);

		virtual Error GetCopyManagementInfo (DWORD block, GenericCopyManagementInfo __far & copyManagementInfo);

		virtual void Interrupt (void);
		virtual void InterruptFrontEnd (void);
		virtual void InterruptPushSense (void);
	};



#endif
