//
// FILE:      library\hardware\mpeg2enc\specific\kfir.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:   19.11.1999
//
// PURPOSE:   VisionTech KFIR Encoder Chip class  --- header file 
//
// HISTORY:
//

#ifndef KFIR_H
#define KFIR_H

#include "library\lowlevel\intrctrl.h"
#include "library\hardware\mpeg2enc\generic\mp2elenc.h"


// Must be in consecutive order !!!
// When new controller type is added, check if it max size is lower then MAX_BUFFER and set MAX_BUFFER if neccessary.
// mst: taken from VisionTech's driver (file kfir.h). Has to be examined (what means KF_CONTR_TYPE???)
typedef enum KF_CONTR_TYPE_Tag {
  KF_GC               = 0,
  KF_DSP_RISC_MC      = 1,
  KF_DSP_RISC_CRAM    = 2,
  KF_DSP_UNIT_MC      = 3,
  KF_BSM_MC           = 4,
  KF_MUX_MC           = 5,
} KF_CONTR_TYPE;

// mst: taken from VisionTech's driver (file kfir.h).
typedef enum KF_CONTR_TYPE_DEV_ID_Tag {
  KF_DEV_ID_GC             = 7,
  KF_DEV_ID_DSP            = 4,
  KF_DEV_ID_BSM            = 5,
  KF_DEV_ID_MUX            = 8,
} KF_CONTR_DEV_ID;


//
// KFIR encoder chip class
// 
// inherits from MPEG2AVEncoder, InterruptHandler, and VDSpinLock
// To synchronize the access to the device's registers you can use the VDSpinLock class. 
// This is mostly the case when the Interrupt Service Routine is doing a lot of access to the 
// device's registers. 
//	For example if you want to read or write to a register just call EnterLock() before and 
// LeaveLock() after the access:
// Function OutDWord()
//		{
//		EnterLock()
//		...
//		register access 
//		...
//		LeaveLock()
//		}
// 1.12.99: leave out the VDSpinLock class. Because the PCI bridge chip (if separate) should need and use this. 
//
class KFIRMPEGEncoder : public MPEG2VideoEncoder, protected InterruptHandler //, protected VDSpinLock
	{
	friend class VirtualKFIRMPEGEncoder;

	public:
		KFIRMPEGEncoder(DWordIndexedInOutPort		* ioPort,
								DWordIndexedInOutPort	* muxFifoPort,
								InterruptServer			* irqServer);
		KFIRMPEGEncoder(DWordIndexedInOutPort		* ioPort,
								InterruptServer			* irqServer);
		
		~KFIRMPEGEncoder(void);

		InterruptServer			*	vidServer;

		Error Initialize(void);
		void Interrupt(void);
		VirtualUnit * CreateVirtual(void);		

	protected:
		MPEGEncoderStreamType	outputType;
		VideoSource					videoSource;
		VideoStandard				videoStandard;
		WORD							aspectRatio;
		DWORD							frameRate;
		DWORD							videoBitrate;
		DWORD							audioBitrate;
		WORD							frameWidth;
		WORD							frameHeight;
		WORD							vbvBufferSize;

		//
		// Basic IO operations
		//
		DWordIndexedInOutPort	*	ioPort;
		//
		// IO operations for the MUX Audio FIFO 
		//
		DWordIndexedInOutPort	*  muxFifoPort;

		Error EnableVidServer(void);
		Error DisableVidServer(void);

		Error OutDWord(int idx, DWORD val);
		Error InDWord(int idx, DWORD __far &val);
		Error KFIRMPEGEncoder::OutAVMuxPortDWord(int idx, DWORD val);
		Error KFIRMPEGEncoder::InAVMuxPortDWord(int idx, DWORD val);

		Error BeginCapture(void);		// initialize chip for encoding
		Error EndCapture(void);			// reset chip after encoding
		Error StartCapture(void);		// start capturing
		Error StopCapture(void);		// stop capturing
		Error PauseCapture(void);		// pause capturing
		Error RestartCapture(void);
		
		MPEGState CurrentState(void);
		Error DoCommand(MPEGCommand com, DWORD param);
		virtual Error AddBuffer(HPTR data, DWORD size);

		Error SetOutputStreamType(MPEGEncoderStreamType outputStreamType);
		Error SetVideoSource(VideoSource source);
		Error SetVideoStandard(VideoStandard standard);		
		Error SetAspectRatio(WORD aspectRatio);
		Error SetFrameRate(DWORD frameRate);
		Error SetVideoBitrate(DWORD videoBitrate);
		Error SetAudioBitrate(DWORD audioBitrate);
		Error SetVideoWidth(WORD width);
		Error SetVideoHeight(WORD height);

		DWORD irqmask;

		Error ReadStatus(DWORD __far& status);
		Error ReadRecord(DWORD __far& record);
		
		Error InitInterruptMask(DWORD mask);
		Error SetInterruptMask(DWORD set);
		Error ResetInterruptMask(DWORD reset);
		Error ReadInterruptMask(DWORD __far&status);
		Error WriteInterruptMask(DWORD mask);
		Error ReadInterruptStatus(DWORD __far&status);
		Error ClearInterruptStatus(DWORD mask);

	private:
		InterruptServer			*	irqServer;
		MPEGState					currentState;
		BOOL							parameterHasChanged;

		Error DownloadMicrocode(void);
		Error CheckDRAM(void);
		Error SetKfirParameters(void);
		Error SetAVMuxParameters(void);
		Error StartKfir(void);
   };							

//
// virtual KFIR encoder chip class
//
class VirtualKFIRMPEGEncoder : public VirtualMPEG2VideoEncoder
	{
	public:
		VirtualKFIRMPEGEncoder(KFIRMPEGEncoder * encoder);

		Error Configure(TAG __far * tags);

	protected:
		Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptChange(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);

	private:
		KFIRMPEGEncoder * encoder;

	};


#endif // KFIR_H

