
// FILE:      library\hardware\jpeg\generic\jcor22.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   15.01.96
//
// PURPOSE: The virtualized JPEG core implemented as combination of 022/016 chips.
//
// HISTORY:

#ifndef JPEGCORE22_H
#define JPEGCORE22_H

#include "library\hardware\jpeg\generic\jcore.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\lowlevel\intrctrl.h"
#include "library\hardware\videodma\specific\i22pip.h"
#include "library\common\krnlsync.h"
#include "library\common\krnllog.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class VirtualJPEGUnit022;
class JPEGCodeIntHandler;
class Z016Class;



class JPEGManager022 : public JPEGManager
	{
	friend class VirtualJPEGUnit022;

	public:
		JPEGManager022 (MemoryMappedIO *io,
		                InterruptServer *jpegCodeIntServer,
		                Profile *profile,
		                Z016Class *z016,
		                BitOutputPort *pxoe016Bit,    // bit to disable 016 PXOUT bus
		                BitOutputPort *resetJPEGBit,  // bit to reset 050 and 016
		                BitOutputPort *clken050Bit,   // bit to enable 050 clock
#if VIONA_VERSION
		                ByteOutputPort *syncSelection,    // 2 bit selection
		                BitOutputPort *decoderSyncEnable, // bit to enable video decoder sync in JPEG core
		                BitOutputPort *frontEndEnable,    // bit to enable video bus front end in JPEG core
		                BitOutputPort *videoBusDecode,    // bit to set encode/ecode state
		                VirtualUnit *blankUnit,
		                BOOL fixPlaybackLongBursts,
		                KernelLogger *log = NULL,
		                BOOL ccirMode = FALSE,
		                AllowedJPEGVStandard allowedStandard = AVSTD_NTSC_PAL_BOTH);
#else
		                BitOutputPort *syncMUX,
		                BOOL fixPlaybackLongBursts);      // bit to switch sync MUX
#endif
		virtual ~JPEGManager022 (void);

		virtual VirtualUnit *CreateVirtual (void);

		virtual void Start (void);
		virtual void Stop  (void);
		virtual void Reset (void);

		void JPEGCodeInterrupt (void);

	protected:
		VirtualJPEGUnit022 *GetCurrent (void) {return (VirtualJPEGUnit022 *)(JPEGManager::GetCurrent());}

		Error InitOperation (DWORD statCom);
		Error StartIRQs (void);
		Error StopIRQs  (void);

		Error WriteStillPixels24 (FPTR src, WORD width, WORD height, VideoField fieldOrder);
#if VIONA_VERSION
		Error WriteStillPixels32 (FPTR src, WORD width, WORD height, VideoField fieldOrder);
		Error ReadStillPixels24 (FPTR dst, WORD width, WORD height, DWORD stride, VideoField fieldOrder);
#endif

		MemoryMappedIO *io;
		InterruptServer *jpegCodeIntServer;
		JPEGCodeIntHandler *jpegCodeIntHandler;
		Profile *profile;
		Z016Class *z016;
		BitOutputPort *pxoe016Bit;
		BitOutputPort *resetJPEGBit;
		BitOutputPort *clken050Bit;
#if VIONA_VERSION
		ByteOutputPort *syncSelection;
		BitOutputPort *decoderSyncEnable;
		BitOutputPort *frontEndEnable;
		BitOutputPort *videoBusDecode;
		VirtualUnit *blankUnit;
		KernelLogger *log;
		BOOL ccirMode;
		AllowedJPEGVStandard allowedStandard;
#else
		BitOutputPort *syncMUX;
#endif

		BOOL fixPlaybackLongBursts;

		// code thresholds for various operation modes
		WORD playbackCodeThreshold, captureCodeThreshold;
		WORD expandCodeThreshold, compressCodeThreshold;

		BOOL handlerEnabled;
		BOOL jpegCoreRunning;
	};



class VirtualJPEGUnit022 : public VirtualJPEGUnit
	{
	friend class JPEGManager022;

	public:
		VirtualJPEGUnit022 (JPEGManager022 *physical, VirtualJCoder *coder, VirtualI22PIP *pip);
		virtual ~VirtualJPEGUnit022 (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info);
		virtual Error UnprepareBuffer (FPTR info);

#if VIONA_VERSION
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);
		virtual BOOL IsValidExpansionScaling (WORD srcWidth, WORD srcHeight, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32);
		virtual BOOL IsValidCompression (WORD width, WORD height, BOOL bits32);
#else
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst);
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size);
#endif

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

		void JPEGCodeInterrupt (void);

		void CalcVideoParameters (void);

		virtual Error InternalAddVideoBuffer (FPTR info, FPTR userData, DWORD size);

		JPEGManager022 *manager;
		VirtualI22PIP *pip;

		ContinuousDMABuffer statComTable;   // DMA table
		DWORD statComPhysical;
		DWORD volatile __far *statCom;

		FPTR dummyCaptureData;
		FPTR dummyCaptureInfo;
		DWORD dummyCapSize;

		FPTR playBackupData;
		FPTR playBackupInfo;
		DWORD playBackupSize;

		IRQMutex	irqMutex;
		JPEGCaptureHookHandle capHook;
		JPEGPlaybackHookHandle playHook;
		JPEGMessageHookHandle preemptHook;
		volatile int nextBuffer;   // already processed by the I22
		int lastBuffer;   // entry for providing new buffer
		int nBuffers;
		DWORD frameNum;
		WORD lastFieldCnt;
		JPEGCaptureMsg buffers[8];
		volatile int stoppingCapture;
		BOOL running;
		BOOL playAddAllowed;
		// Valid only if pipeline deeper than STATCOM table.
		int nStatBuffers;
		volatile int statNextBuffer;
		int statLastBuffer;

		DWORD nextFrameTime;
		DWORD milliSecPerFrame;

		VideoStandard vStandard;
		VideoFormat vFormat;
		VideoField fieldOrder;
		BOOL thumbnail;
		BOOL filtersOn;

		DWORD totalCodeVolume;

		BOOL stillExpansion;
		BOOL singleFrame;

		// Video parameters.
		BOOL syncMaster;
		WORD opWidth, opHeight;
		WORD vSyncSize, frameTotal, hSyncStart, lineTotal, nax, pax, nay, pay;   // of I22
		int hOffset, vOffset;
		BOOL horDecimation;
		DWORD jpgMode;
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
