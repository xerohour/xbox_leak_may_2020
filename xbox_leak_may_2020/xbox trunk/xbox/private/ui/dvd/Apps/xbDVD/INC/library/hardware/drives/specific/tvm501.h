
// FILE:      library\hardware\drives\specific\tvm501.h
// AUTHOR:    D. Heidrich, H. Horak
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   10.07.98
//
// PURPOSE:   Thomson TVM501 DVD drive.
//
// HISTORY:

#ifndef TVM501_H
#define TVM501_H

#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\common\krnlsync.h"
#include "library\common\profiles.h"
#include "library\common\winports.h"
#include "library\hardware\drives\generic\cdvddrv.h"
#include "library\hardware\mpeg2dec\generic\mp2dcryp.h"
#include "library\hardware\uproc\specific\st55xx\frnt5505.h"

#include <semaphor.h>
#include <time.h>



#define TVM501_I2C_ADDRESS  0x30



class TVM501Drive : public GenericCDVDDrive, public MPEG2DVDDecryption
	{
	friend void TVM501_HandlerTask (void *drive);

	private:
		AsyncByteInOutBus *port;
		STi5505FrontEnd *frontEnd;
		GenericProfile *profile;
		MemoryMappedIO *pio1;
		MemoryMappedIO *pio2;
		MemoryMappedIO *pio3;
		WinPort *winPort;

		// Configurable parameters.
		DWORD handlerTimeout;
		int maxHandlerRetries;
		DWORD defaultRequestTimeout;
		DWORD cdRequestTimeout;
		DWORD cddaRequestTimeout;
		DWORD longLatencyDistance;

		// Drive information.
		BOOL driveIs501;
		BOOL swapCDDABytes;

		VDMutex monitorMutex;

		BOOL pushSensed, pushSenseAllowed;
		clock_t trayOpenedTime;

		semaphore_t irqSema;
		int diskTrayLocked;
		BOOL dataHold;
		BYTE status[6];
		BYTE sector[3];
		BYTE layer;
		BYTE playMode;
		int servoState;

		enum Command
			{
			CMD501_ILLEGAL,			// internal command
			CMD501_TICK,				// internal command
			CMD501_SPIN_UP,			// internal command
			CMD501_WRITE_DISKKEY,	// internal command
			CMD501_WRITE_TITLEKEY,	// internal command
			CMD501_UNLOCK_BUFFERS,	// internal command
			CMD501_READ_TOC,			// internal command
			CMD501_SET_READMODE,		// internal command
			CMD501_SEEKBLOCK,			// internal command
			CMD501_READBLOCKS,
			CMD501_READFORCED,
			CMD501_LOCK,
			CMD501_UNLOCK,
			CMD501_LOADMEDIA,
			CMD501_UNLOADMEDIA,
			CMD501_STOP,
			CMD501_PAUSE
			};

		VDMutex trayMutex;
		BOOL emergencyEject;
		enum TrayStatus
			{
			TRAY501_UNKNOWN, TRAY501_CLOSING, TRAY501_CLOSED, TRAY501_OPENING, TRAY501_OPEN
			} trayStatus;

		Error CheckForEmergencyEject (void);

		enum DriveCommand
			{
			DC501_none		= 0x00,
			DC501_stop		= 0x80,
			DC501_ndisc		= 0x01,
			DC501_plsn		= 0x02,
			DC501_sdisc		= 0x83,
			DC501_plmsf		= 0x85,
			DC501_gloc		= 0x86,
			DC501_pause		= 0x07,
			DC501_pmod		= 0x89,
			DC501_sskst		= 0x8a,
			DC501_ssken		= 0x0b,
			DC501_noskip	= 0x8c,
			DC501_bca		= 0x0d,
			DC501_gmod		= 0x0e	// TVM502
			};

		// Additional disk information, valid if LOAD_MEDIA succeeds.
		BOOL diskIsIdentified;
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

		Error MoveTray (BOOL close);
		Error ProgramDrive (DriveCommand command);
		Error Execute (Command command);
		Error WaitForState (int wantedState);
		Error WaitForIRQ (void);
		Error ReadStatus (void);

		Error UploadPatchCode (BYTE *data, int size);

		// Access to any sector on the disk.
		Error InternalReadBlocks (DWORD block, DWORD num, void __far * buffer, DriveStreamBlockBuffer * blockBuffer,
		                          BYTE *subChannels, BOOL streaming, CDSectorType type = CST_CD_DA, BOOL rawRead = FALSE);

		int ReadQChannel (BYTE *start, int nbits);
		Error FindCDSectorNumber (BYTE *source, DWORD &sectorNumber, int &offset, int &bytesLeftInRow);
		BOOL DescrambleAndCopyCD (BYTE *src1, int bytesLeftInRow, int bytesLeftInPart1,
		                          BYTE *src2, int bytesLeftInPart2,
		                          BYTE *dst);


		// Track buffer.
		BYTE *trackBufferPtr;
		int sectorSize;
		int sectorOffset;
		int totalSectors;


		// Handler task related.
		void HandlerTask (void);
		void StopTransfer (void);
		void FlushTrackbuffer (void);
		BOOL handlerTaskRunning;
		BYTE **bufferMap;
		DWORD *sectorNumberMap;
		int startReadCount;
		BOOL spinningStopped;
		CDSectorType currentCDSectorType;
		BOOL currentIsStreamingData;

		semaphore_t newRequest;
		semaphore_t requestDone;
		struct {
			Command command;	// input
			DWORD sector;		// input
			BYTE *key;			// input
			CDSectorType sectorType;	// input
			BOOL isStreamingData;		// input
			DWORD numberOfSectors;		// input
			DriveStreamBlockBuffer *blockBuffer;	// input
			Error error;		// output
			} request;
		DWORD timeOutOneSecond;

		// Buffer-locking related.
		FrontEnd5505BufferStatus *bufferStatusMap;
		DWORD *bufferLockCount;

		// The following members are protected by trackControlMutex.
		VDMutex trackControlMutex;
		int validBuffers;   // number of valid buffers in the track buffer
		int validStart;
		int lockedBuffers;   // number of locked buffers outside of the valid area
		// these variables for TVM501_TASK_TWOSTAGED only
		DWORD knownAreaStart;   // inclusive value, first sector number in known area
		DWORD knownAreaEnd;   // inclusive value, last sector number in known area

		// The following members are protected by syncPatternMutex.
		VDMutex syncPatternMutex;
		int syncLocationOffset, syncLocationBytesLeftInRow;   // for CD mode only


		volatile BOOL insideReadRequest, terminateReadRequest;


	public:
		void DisplayDebugRecord (void);   // only for internal debugging!


	public:
		TVM501Drive (AsyncByteInOutBus *port, STi5505FrontEnd *frontEnd, GenericProfile *profile,
		             MemoryMappedIO *pio1, MemoryMappedIO *pio2, MemoryMappedIO *pio3, WinPort *winPort = NULL);
		virtual ~TVM501Drive (void);

		virtual Error Initialize(void);

		virtual Error ReadDVDBlocks(DWORD block, DWORD num, void __far * buffer, BOOL streaming);
		virtual Error ReadDVDForced(DWORD block, DWORD num, void __far * buffer, BOOL streaming);
		virtual Error ReadAndLockStreamBlocks (DWORD block, DWORD num, DriveStreamBlockBuffer * blocks, BOOL streaming);
		virtual Error UnlockStreamBlocks (DWORD num, DriveStreamBlockBuffer * blocks);
		virtual BOOL PrefersStreamBlocks (void);

		virtual Error SeekDVDBlock (DWORD block, BOOL streaming);

		virtual Error TerminateRead (void);

		virtual Error Stop(void);
		virtual Error Pause(void);

		virtual Error StartAuthentication(void);
		virtual Error SendChallengeKey(BYTE __far * key);
		virtual Error GetBusKey(BYTE __far * key);
		virtual Error GetChallengeKey(BYTE __far * key);
		virtual Error SendBusKey(BYTE __far * key);
		virtual Error GetDiskKey(BYTE __far * key);
		virtual Error CompleteAuthentication(void);
		virtual Error CancelAuthentication(void);
		virtual Error GetTitleKey(DWORD block, BYTE __far * key);

		virtual BOOL IsEncrypted(void);
		virtual BOOL IsDVDDrive(void);

		virtual Error DVDInquiry(BOOL __far & removable, BYTE __far & deviceType);

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
