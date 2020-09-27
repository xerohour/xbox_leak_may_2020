
// FILE:      library\hardware\uproc\specific\st55xx\frnt5505.h
// AUTHOR:    D. Heidrich, H. Horak
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   13.08.98
//
// PURPOSE:   STi5505 front-end.
//
// HISTORY:

#ifndef FRNT5505_H
#define FRNT5505_H

#include "library\lowlevel\memmpdio.h"
#include "library\common\krnlsync.h"

#include <semaphor.h>



enum FrontEnd5505DataSource
	{
	// Source port of incoming data.
	FE5505DS_FEC, FE5505DS_PARALLEL,
	FE5505DS_FEC_TMM   // special mode with bug fixes
	};



enum FrontEnd5505BufferStatus
	{
	FE5505BSTAT_FREE,			// no restrictions
	FE5505BSTAT_LOCKED,		// locked buffers don't receive new data
	FE5505BSTAT_UNLOCKED		// buffers was just unlocked and cannot contain new data
	};



class STi5505FrontEnd : public VDMutex
	{
	private:
		MemoryMappedIO *mem;
		int irqLevel;
		FrontEnd5505DataSource dataSource;
		BOOL driveIsTMM, cdModeIsTMM;

		BYTE chipVersion;

		DWORD config;
		enum {
			DECRYPT_RESET, DECRYPT_RUNNING, DECRYPT_STOPPED
			} decryptionState;

		DWORD startSector, endSector;
		int sectorsToTransfer;
		int maxTimeout;

		int startBufferNumber;
		int currentBuffer;

		int sectorsLeft;
		DWORD firstTarget;

		BOOL streamingIn;   // TRUE if data is coming in from the startSector
		BOOL readingFirst;

		int stoppedCount;

		DWORD dmaAdhStart, dmaAdlStart;
		DWORD dmaAdh, dmaAdl;
		DWORD safeAdh, safeAdl;

		// Track buffer mapping table.
		DWORD *bufferMap;
		int bufferMapEntries;
		int sectorSize;
		int totalSectors;
		BOOL dvdMode;
		int defaultDVDSectorSize;
		BYTE *overflowArea;

		FrontEnd5505BufferStatus *bufferStatus;

		void InternalStopTransfer (void);

	public:
		STi5505FrontEnd (MemoryMappedIO *mem, int irqLevel, int defaultDVDSectorSize, FrontEnd5505DataSource source);
		virtual ~STi5505FrontEnd (void);

		virtual Error Initialize (void);

		virtual Error InitTrackBufferMapping (semaphore_t *wakeUp, BYTE *trackBuffer,
		                                      int sectorOffset, int totalSectors, int sectorSize,
		                                      FrontEnd5505BufferStatus *bufferStatusMap,
		                                      BYTE *overflowArray = NULL, int overflowSectors = 0,
		                                      BOOL linearMapping = FALSE);

		// Note: At least five sectors should be requested.
		Error StartTransfer (int bufferNum, DWORD startSector, int nSectors);

		void StopTransfer (void);

		void RedirectAndWait (BYTE *buffer);

		// Note: Call this only if at least two buffers are not successful yet.
		Error AddTransferBuffers (int nBuffers);

		BOOL IsValidSector (BYTE *sector, DWORD &sectorNumber);

		Error ResetDecryption (void);

		void SuspendDecryption (void);
		void ResumeDecryption (void);

		Error StartAuthentication (DWORD sector);
		Error ReadChallengeKey (BYTE *key);
		Error WriteBusKey (BYTE *key);
		Error WriteChallengeKey (BYTE *key);
		Error ReadBusKey (BYTE *key);
		Error WriteDiskKey (BYTE *key);
		Error WriteTitleKey (BYTE *key);

		void InterruptDMA (void);


		// only for prototype!!!
		int sectorsTransferred;   // counts successfully read sectors
		semaphore_t *wakeUp;   // to signal the receiver
		BOOL stopped;
		BOOL dmaActive;
		int arOverflows, validOverflows;
		int eofCount;
		void PrintDebugStat (void);
	};



#endif
