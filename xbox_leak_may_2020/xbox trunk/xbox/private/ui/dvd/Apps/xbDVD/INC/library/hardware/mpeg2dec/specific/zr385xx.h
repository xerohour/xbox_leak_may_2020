// FILE:			library\hardware\mpeg3dec\specific\zr385xx.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.95
//
// PURPOSE:		
//
// HISTORY:
#ifndef ZR385XX_H
#define ZR385XX_H

#include "..\generic\mp2eldec.h"
#include "zr38spi.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "library\lowlevel\hardwrio.h"
#include "library\general\asncstrm.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\general\dynarray.h"
#include "library\common\krnlstrm.h"
#include "library\hardware\clocks\generic\clocks.h"

class ZR385XXAC3Decoder : public MPEG2AC3Decoder, 
                          protected ZR385XXSPIComPort, 
                          protected ASyncRefillRequest,
                          protected InterruptHandler
	{
	friend class VirtualZR385XXAC3Decoder;
	protected:
		VirtualAudioDAC	*	dac; 
		ASyncOutStream		*	strm;
		
		DWORD 		initialPTS;
		DWORD			audioBitrate;
		BOOL			ptsValid;                                                   
		MPEGState	state;
		BOOL			ac3;
		BYTE			ac3StreamID;
		PTSAssoc		ptsAssoc;
		DWORD			streamPosition;
      
		DWORD			signalPosition;
		DWORD			predPosition;
				
		DWORD			stepCnt;
		
		AC3Params	ac3Params;
		PROLParams	prolParams;
		PCMParams	pcmParams;
		
		VirtualUnit	*	audioBus;
		
		AC3BassConfig		pBassConfig, bassConfig;
		AC3SpeakerConfig  pSpeakerConfig, speakerConfig;
		WORD					pCenterDelay, centerDelay;
		WORD					pSurroundDelay, surroundDelay;
		
		DynamicByteArray	mpegMicrocode, ac3Microcode;
		ProgrammableClockGenerator	*	audioClock;
		
		Error UpdateAudioParameter(void);
		
		Error SetAudioBitrate(DWORD bitrate);
		Error SetSampleRate(WORD rate);
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		Error SetAC3(BOOL ac3);

		Error SetBassConfig(AC3BassConfig config);
		Error SetSpeakerConfig(AC3SpeakerConfig config);
		Error SetCenterDelay(WORD delay);
		Error SetSurroundDelay(WORD delay);      
		
		Error SetCenterVolume(WORD volume);
		Error SetLeftSurroundVolume(WORD volume);
		Error SetRightSurroundVolume(WORD volume);
		Error SetSubwooferVolume(WORD volume);

	   Error SendCommand(MPEGCommand com, DWORD param, DWORD __far &tag);
		BOOL CommandPending(DWORD tag);

		DWORD CurrentLocation(void);
		MPEGState CurrentState(void);
		
		DWORD LastTransferLocation(DWORD scale);
			   
	   DWORD GetPTS();
	   DWORD SendData(HPTR data, DWORD size);
	   void CompleteData(void);
	
	   void PutPTS(DWORD pts);
	   
		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);
		void Interrupt(void);

	   Error SetSignalPosition(DWORD position);

		Error Configure(TAG __far * tags);
		
		InterruptServer	*	vblank;	
	public:
		ZR385XXAC3Decoder(BitInputPort		*	si,
					         BitOutputPort		*	ss,
					         BitOutputPort		*	sck,
					         BitOutputPort		*	so,
					         int						delay,
					         VirtualAudioDAC 	*	dac,
					         ASyncOutStream		*	strm,
					         InterruptServer	*	vblank,
					         VirtualUnit			*	audioBus = NULL,
					         ProgrammableClockGenerator * audioClock = NULL);
		        
		VirtualUnit * CreateVirtual(void);
		
		Error Boot(const char * name);
		Error LoadMicrocodes(KernelString ac3Name, KernelString mpegName);		
	};

class VirtualZR385XXAC3Decoder : public VirtualMPEG2AC3Decoder
	{
	private:
		ZR385XXAC3Decoder	*	device;
	protected:
      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);		
	public:
		VirtualZR385XXAC3Decoder(ZR385XXAC3Decoder * device);
	};

#endif
