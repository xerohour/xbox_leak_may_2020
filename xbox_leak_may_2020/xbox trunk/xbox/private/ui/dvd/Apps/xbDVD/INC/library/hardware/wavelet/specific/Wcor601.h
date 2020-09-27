
// FILE:      library\hardware\wavelet\specific\wcor601.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   07.05.96
//
// PURPOSE: The virtualized Wavelet core for the ADV601.
//
// HISTORY:

#ifndef WAVELET601_H
#define WAVELET601_H

#include "library\hardware\wavelet\generic\wcore.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\lowlevel\dmachanl.h"
#include "library\lowlevel\intrctrl.h"

#include "library\hardware\pcibrdge\specific\saa7146\saa7146.h"
#include "library\hardware\pcibrdge\specific\saa7146\rps7146.h"
#include "library\hardware\videodma\specific\s7146pip.h"
#include "library\hardware\pcibrdge\specific\saa7146\scatdma.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class VirtualWaveletUnit601;
class WaveletEndIntHandler;
class VBlankIntHandler;
class PortAccess601;



class WaveletManager601 : public WaveletManager
	{
	friend class VirtualWaveletUnit601;

	public:
		WaveletManager601 (IndexedInOutPort *io, Profile *profile,
		                   InterruptServer *waveletIRQ, InterruptServer *vblankIRQ,
		                   SAA7146 *saa7146,
		                   BitOutputPort *resetEncoderPort,
		                   BitOutputPort *reset601Port,
		                   BitOutputPort *slaveMode601Port,  // bit to set 601 to slave mode
		                   BitOutputPort *frontEndEnable,    // bit to enable video bus front end in Wavelet core
		                   BitOutputPort *stillOutEnable,    // bit to enable still mode output
		                   BitOutputPort *slowClockEnable,   // bit to enable 1/8th clock for 601
		                   BitOutputPort *sync601Enable,     // bit to enable 601 sync in Wavelet core
		                   BitOutputPort *data601Enable,     // bit to enable 601 data on video bus
		                   BitOutputPort *decouple601,       // bit to decouple 601 from video bus
		                   BitOutputPort *frameLastcode,     // bit to get LCODE each frame instead of field
		                   BitOutputPort *stopReadEnable,    // bit to stop read transfer for field/frame separation
		                   VirtualUnit *blankUnit);
		virtual ~WaveletManager601 (void);

		virtual VirtualUnit *CreateVirtual (void);

		virtual void Start (void);
		virtual void Stop  (void);
		virtual void Reset (void);

		void WaveletEndInterrupt (void);
		void VBlankInterrupt (void);

	protected:
		VirtualWaveletUnit601 *GetCurrent (void) {return (VirtualWaveletUnit601 *)(WaveletManager::GetCurrent());}

		Error InitOperation (void);
		Error StartIRQs (void);
		Error StopIRQs  (void);

		PortAccess601 *io;
		Profile *profile;
		InterruptServer *waveletIRQ;
		InterruptServer *vblankIRQ;
		SAA7146 *saa7146;
		BitOutputPort *resetEncoderPort;
		BitOutputPort *reset601Port;
		BitOutputPort *slaveMode601Port;
		BitOutputPort *frontEndEnable;
		BitOutputPort *stillOutEnable;
		BitOutputPort *slowClockEnable;
		BitOutputPort *sync601Enable;
		BitOutputPort *data601Enable;
		BitOutputPort *decouple601;
		BitOutputPort *frameLastcode;
		BitOutputPort *stopReadEnable;
		VirtualUnit *blankUnit;

		VirtualWaveletUnit601 *operatingUnit;

		DataInOutPort *codePort;
		DMAChannel *readChannel, *writeChannel;
		WaveletEndIntHandler *waveletEndIntHandler;
		VBlankIntHandler *vblankIntHandler;

		ContinuousDMABuffer *dmaBuffer;
		DWORD dmaSize;
		FPTR dmaDataPtr;
		class WaveletTransferRequest *dmaRequest;

		int encodeThreshold, decodeThreshold;

		BOOL endHandlerEnabled, vBlankHandlerEnabled;
	};



class VirtualWaveletUnit601 : public VirtualWaveletUnit
	{
	friend class WaveletManager601;

	public:
		VirtualWaveletUnit601 (WaveletManager601 *physical, VirtualSAA7146PIP *pip);
		virtual ~VirtualWaveletUnit601 (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info);
		virtual Error UnprepareBuffer (FPTR info);

		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);
		virtual BOOL IsValidExpansionScaling (WORD srcWidth, WORD srcHeight, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);

		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32);
		virtual BOOL IsValidCompression (WORD width, WORD height, BOOL bits32);

		// These are allowed only after locking and activating the unit.
		virtual Error InitCapture (JPEGCaptureHook __far *hook);
		virtual Error FinishCapture (void);
		virtual Error AddVideoBuffer (FPTR info, FPTR userData, DWORD size);
		virtual Error Start (void);
		virtual Error Stop  (void);

		virtual Error AllocateVideoBuffer (DWORD size, FPTR __far &info, FPTR __far &data);
		virtual Error FreeVideoBuffer (FPTR info);
		virtual Error InitPlayback (JPEGPlaybackHook __far *hook);
		virtual Error FinishPlayback (void);
		virtual Error FlushPlayback (DWORD bufferSize);
		virtual Error PlayBuffer (FPTR info, DWORD size);

	protected:
		virtual Error Preempt (VirtualUnit *previous);

//		Error CompressField (FPTR src, WORD numberOfBlocks, FPTR dst, DWORD maxSize, DWORD __far &size);

		void WaveletEndInterrupt (void);
		void VBlankInterrupt (void);

		void CalculateBinWidths (DWORD currentSize);

		virtual Error InternalAddVideoBuffer (FPTR info, FPTR userData, DWORD size);

		WaveletManager601 *manager;
		VirtualSAA7146PIP *pip;

		FPTR dummyCaptureData;
		FPTR dummyCaptureInfo;
		DWORD dummyCapSize;

		FPTR playBackupData;
		FPTR playBackupInfo;
		DWORD playBackupSize;

		JPEGCaptureHookHandle capHook;
		JPEGPlaybackHookHandle playHook;
		JPEGMessageHookHandle preemptHook;
		volatile int nextBuffer;   // already processed buffer
		int lastBuffer;   // entry for providing new buffer
		volatile int nBuffers;
		DWORD frameNum;
		DWORD fieldNum, calculatedFieldNum;
		WORD lastFieldCnt;
		JPEGCaptureMsg buffers[4];
		volatile int stoppingCapture;
		BOOL running;
		BOOL playAddAllowed;

		RPSProgram *rpsPlaySlots;
		int lastScatter;

		RPSProgram *rpsPlayTransfer;
		ScatteredBuffer7146::PlayParametersRPS rpsPlayPhysical;
		Error CreatePlayTransfer (void);

		volatile BOOL irqSecondField;   // only for playback
		volatile BOOL transferRunning;

		RPSProgram *rpsCapTransfer;
		Error CreateCaptureTransfer (void);

		ContinuousDMABuffer *capCycleBuffer;
		FPTR capBufferPtr;
		DWORD capPhysical;
		RPSProgram *rpsCapture;
		int nCapPages;
		int startPage;
		DWORD __huge *startPagePtr;
		DWORD __huge *capSrc;
		DWORD __huge *capDst;
		DWORD bytesCaptured;
		DWORD lastFrameSize;
		DWORD rbwOffsetRPS, rbwBlockSize;   // in DWORD units
		BOOL noDestination;

		DWORD nextFrameTime;
		DWORD milliSecPerFrame;
		BOOL singleFrame;

		VideoStandard vStandard;
		VideoFormat vFormat;
		VideoField fieldOrder;
		BOOL thumbnail;
		BOOL filtersOn;

		DWORD totalCodeVolume;
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
