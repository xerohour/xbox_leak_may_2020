
// FILE:      library\hardware\drives\specific\piopd4995.h
// AUTHOR:    D. Heidrich
// COPYRIGHT: (c) 1999 Ravisent Technologies. All Rights Reserved.
// CREATED:   21.09.99
//
// PURPOSE:   Pioneer DVD drive for CE.
//
// HISTORY:

#ifndef PIOPD4995_H
#define PIOPD4995_H

#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\common\krnlsync.h"
#include "library\common\winports.h"
#include "library\hardware\drives\generic\cdvddrv.h"
#include "library\hardware\mpeg2dec\generic\mp2dcryp.h"
#include "library\hardware\uproc\specific\st55xx\frnt5505.h"
#include "library\hardware\uproc\specific\pd4995\pd4995r.h"

#include <semaphor.h>



class PioneerPD4995 : public GenericCDVDDrive, public MPEG2DVDDecryption
	{
	friend void PioneerPD4995_HandlerTask (void *drive);

	private:
		MemoryMappedIO *port;
		STi5505FrontEnd *frontEnd;
		int irqLevel;
		WinPort *winPort;

		VDMutex monitorMutex;

		BOOL firstBlockWasRead;

		enum TrayStatus
			{
			TRAY_UNKNOWN, TRAY_CLOSING, TRAY_CLOSED, TRAY_OPENING, TRAY_OPEN
			} trayStatus;
		BOOL diskIsIdentified;
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
		BOOL dualLayer;
		BOOL oppositeTrack;
		BOOL encryptedDisk;
		int regionCode;
		DWORD dataStartSector;		// usually 0x30000
		DWORD dataEndSector;
		DWORD layerZeroEndSector;
		int firstTOC, lastTOC;   // for CD mode only
		int leadOutMin, leadOutSec, leadOutFrm;   // for CD mode only

		int ReadQChannel (BYTE *start, int nbits);

		Error CheckDriveReady (void);
		Error RequestSense (void);
		Error WaitForIRQ (void);
		Error ReadStatus (void);

		Error StartDiskTransfer (int vbrBlockindex, int numBlocks);
		void StopDiskTransfer (void);

		BYTE packetArray[8];
		BYTE statusArray[8];

		Error DoPacket (BYTE *buffer, int length, BOOL waitForCompletion = FALSE);

		enum Command
			{
			CMD_ILLEGAL,				// internal command
			CMD_TICK,					// internal command
			CMD_WAKEUPDRIVE,			// internal command
			CMD_GET_COPY_INFO,		// internal command
			CMD_WRITE_DISKKEY,		// internal command
			CMD_WRITE_TITLEKEY,		// internal command
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
			CMD_PAUSE,
			CMD_SEND_CONTROL_DATA
			};

		Error HandlerCommand (Command command);


		// Access to any sector on the disk.
		Error InternalReadBlocks (DWORD sector, DWORD num, void __far * buffer, CDSectorType type = CST_CD_DA, BOOL rawRead = FALSE);


		// Track buffer.
		BYTE *trackBufferPtr;
		int sectorSize;
		int sectorOffset;
		int sectorsPerBlock;
		int totalVBRBuffers, totalVBRBlocks;
		BYTE **bufferMap;
		DWORD *sectorNumberMap;
		struct SectorHeader {
			BYTE headerbytes[8];   // contains IED and CPR_MAI for DVD or Q subcodes for CD
			int blockIndex;   // index of the block that the sector belongs to
			int dataIndex;   // index of the corresponding data inside that block
			};
		SectorHeader *sectorHeaderMap;
		BOOL *sectorHasErrors;
		VDMutex byteAccessMutex;


		// Handler task related.
		void HandlerTask (void);
		BOOL handlerTaskRunning;
		int startReadCount;
		BOOL spinningStopped;
		CDSectorType currentCDSectorType;
		BOOL currentIsStreamingData;

		semaphore_t newRequest;
		semaphore_t requestDone;
		struct {
			Command command;					// input
			DWORD sector;						// input
			int num;								// input
			BYTE *buffer;						// input
			CDSectorType sectorType;		// input
			BOOL isStreamingData;			// input
			Error error;						// output
			} request;
		DWORD timeOutOneSecond;

		// For disk to VBR transfer.
		int nextTransferBlock, nextTransferSector;
		int sectorsToTransfer, sectorsTransferred;
		enum {
			DISKTRANSFER_STOPPED, DISKTRANSFER_STREAMING, DISKTRANSFER_LASTBLOCK
			} transferState;

		// DMA from VBR to memory.
		int nextDMASector;
		int sectorsToDMA, sectorsDMADone;

		// The following members are protected by trackControlMutex.
		VDMutex trackControlMutex;
		int validBuffers;   // number of valid buffers in the track buffer
		int validStart;
		// these variables for TVM501_TASK_TWOSTAGED only
		DWORD knownAreaStart;   // inclusive value, first sector number in known area
		DWORD knownAreaEnd;   // inclusive value, last sector number in known area

		volatile BOOL insideReadRequest, terminateReadRequest;

		Error HandlerLoadMedia (int slotNumber, BOOL wait);
		Error HandlerUnloadMedia (BOOL wait);


	public:
		void DisplayDebugRecord (void);   // only for internal debugging!


	public:
		PioneerPD4995 (MemoryMappedIO *port, STi5505FrontEnd *frontEnd, int irqLevel, WinPort *winPort = NULL);
		virtual ~PioneerPD4995 (void);

		virtual Error Initialize (void);

		virtual Error ReadDVDBlocks (DWORD sector, DWORD num, void __far * buffer, BOOL streaming);
		virtual Error ReadDVDForced (DWORD sector, DWORD num, void __far * buffer, BOOL streaming);

		virtual Error SeekDVDBlock (DWORD sector, BOOL streaming);

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
		virtual Error GetTitleKey (DWORD sector, BYTE __far * key);

		virtual BOOL IsEncrypted (void);
		virtual BOOL IsDVDDrive (void);

		virtual Error DVDInquiry (BOOL __far & removable, BYTE __far & deviceType);

		virtual Error Lock (void);
		virtual Error Unlock (void);
		virtual Error LoadMedia (int slotNumber, BOOL wait);
		virtual Error UnloadMedia (BOOL wait);
		virtual Error GetDriveStatus (GenericTrayStatus &status);

		virtual Error WakeUpDrive (void);

		virtual Error ReadCDBlocks (DWORD sector, DWORD num, void __far * buffer, CDSectorType type, void __far * subChannels, BOOL streaming);
		virtual Error ReadTOC (CDATocEntry *toc, int &tocEntryNum);

		virtual Error SeekCDBlock (DWORD sector, CDSectorType type, BOOL streaming);

		// Decryption cell programming.
		virtual Error DoAuthenticationCommand (MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);

		virtual GenericIRQTrigger GetInterruptTrigger (void);

		virtual Error GetCopyManagementInfo (DWORD sector, GenericCopyManagementInfo __far & copyManagementInfo);

		virtual void Interrupt (void);
		virtual void InterruptFrontEnd (void);
		virtual void InterruptPushSense (void);
	};



#endif
