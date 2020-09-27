
// FILE:      library\hardware\jpeg\generic\jcoram55.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   02.02.96
//
// PURPOSE: The virtualized JPEG core implemented as combination of S5933/055/050 chips.
//
// HISTORY:

#ifndef JPEGCORE_AMCC55_H
#define JPEGCORE_AMCC55_H

#include "library\hardware\jpeg\generic\jcore.h"
#include "library\lowlevel\hardwrio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\lowlevel\dmachanl.h"
#include "library\lowlevel\intrctrl.h"
#include "library\hardware\jpeg\specific\z055.h"
#include "library\common\krnlsync.h"
#include "library\common\krnllog.h"

#ifndef ONLY_EXTERNAL_VISIBLE



#define N_DLL_BUFFERS  4   // number of buffers possible in the DLL pipeline, must be power of 2



class VirtualJPEGUnit055;
class JPEGEndIntHandler;



class JPEGManager055 : public JPEGManager
	{
	friend class VirtualJPEGUnit055;

	public:
		JPEGManager055 (Z055 *z055, VirtualUnit *blankUnit, InterruptServer *jpegIRQ,
		                DataInOutPort *codePort, DataInOutPort *stripPort,
		                DMAChannel *readChannel, DMAChannel *writeChannel,
							 KernelLogger *log = NULL);
		virtual ~JPEGManager055 (void);

		virtual VirtualUnit *CreateVirtual (void);

		virtual void Start (void);
		virtual void Stop  (void);
		virtual void Reset (void);

		void JPEGEndInterrupt (void);

	protected:
		VirtualJPEGUnit055 *GetCurrent (void) {return (VirtualJPEGUnit055 *)(JPEGManager::GetCurrent());}

		Error InitOperation (void);
		Error StartIRQs (void);
		Error StopIRQs  (void);

		Error ExpandIntoOverlayFrame (FPTR frame, DWORD size);
		Error ExpandStart (DWORD size);
		void ExpandStop (void);

		Error ReadCompressedField (FPTR field, BYTE pageNum, DWORD size);
		Error CompressStart (BYTE pageNum, DWORD size);
		Error CompressStop (DWORD size);

		KernelLogger *log;
		Z055 *z055;
		VirtualUnit *blankUnit;
		InterruptServer *jpegIRQ;
		DataInOutPort *codePort, *stripPort;
		DMAChannel *readChannel, *writeChannel;
		InterruptServer *jpegEndIntServer;
		JPEGEndIntHandler *jpegEndIntHandler;

		ContinuousDMABuffer *dmaBuffer;
		DWORD dmaSize;
		FPTR dmaDataPtr;
		class JPEGTransferRequest *dmaRequest;

		BOOL pageAvailable[H55_N_BUFFERS];
		int currentPage;

		BOOL handlerEnabled;
	};



class VirtualJPEGUnit055 : public VirtualJPEGUnit
	{
	friend class JPEGManager055;

	public:
		VirtualJPEGUnit055 (JPEGManager055 *physical, VirtualJCoder *coder);
		virtual ~VirtualJPEGUnit055 (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info);
		virtual Error UnprepareBuffer (FPTR info);

#if VIONA_VERSION
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);
		virtual BOOL IsValidExpansionScaling (WORD srcWidth, WORD srcHeight, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat);
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32);
#else
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst);
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size);
#endif

		// These are allowed only after locking and activating the unit.
		virtual Error InitCapture (JPEGCaptureHook __far *hook);
		virtual Error FinishCapture (void);
		virtual Error AddVideoBuffer (FPTR info, FPTR userData, DWORD size);
		virtual Error Start (void);

		virtual Error AllocateVideoBuffer (DWORD size, FPTR __far &info, FPTR __far &data);
		virtual Error FreeVideoBuffer (FPTR info);
		virtual Error InitPlayback (JPEGPlaybackHook __far *hook);
		virtual Error FinishPlayback (void);
		virtual Error PlayBuffer (FPTR info, DWORD size);

		void DMATransferDone (void);

	protected:
		virtual Error Preempt (VirtualUnit *previous);

		Error CompressField (FPTR src, WORD numberOfBlocks, FPTR dst, DWORD maxSize, DWORD __far &size);

		void JPEGEndInterrupt (void);

		JPEGManager055 *manager;

		IRQMutex	irqMutex;
		JPEGCaptureHookHandle capHook;
		JPEGPlaybackHookHandle playHook;
		JPEGMessageHookHandle preemptHook;
		volatile int nextBuffer;   // already processed buffer
		int lastBuffer;   // entry for providing new buffer
		volatile int nBuffers;

		enum CaptureState { CAP_NONE, CAP_SINGLE_FIRST_BAD, CAP_SINGLE_FIRST, CAP_FIRST, CAP_FIRST_BAD, CAP_SECOND };
		CaptureState capState;
		JPEGCaptureMsg capMsg;
		DWORD fieldNum;
		WORD lastFieldCnt;
		JPEGCaptureMsg buffers[N_DLL_BUFFERS];
		void PassUpBuffer (void);

		volatile BOOL irqSecondField;   // only for playback
		volatile BOOL transferRunning;

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
