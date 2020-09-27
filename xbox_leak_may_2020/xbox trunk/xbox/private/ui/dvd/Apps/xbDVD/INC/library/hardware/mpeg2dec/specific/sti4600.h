// FILE:			library\hardware\mpeg3dec\specific\sti4600.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.95
//
// PURPOSE:		
//
// HISTORY:
#ifndef STI4600_H
#define STI4600_H

#include "..\generic\mp2eldec.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "library\lowlevel\hardwrio.h"
#include "library\general\asncstrm.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\common\krnlstr.h"
#include "library\hardware\clocks\generic\clocks.h"

class STi4600AC3Decoder : public MPEG2AC3Decoder, 
                          protected ASyncRefillRequest,
                          protected InterruptHandler
	{
	friend class VirtualSTi4600AC3Decoder;
	protected:
		ByteIndexedInOutPort			*	port;
		VirtualAudioDAC				*	dac; 
		ASyncOutStream					*	strm;		
		ProgrammableClockGenerator * audioClock;
		
		DWORD 		initialPTS;
		DWORD			audioBitrate;
		DWORD			sampleRate;
		BOOL			ptsValid;                                                   
		MPEGState	state;
		BOOL			ac3, lpcm;
		BYTE			ac3StreamID;
		PTSAssoc		ptsAssoc;
		DWORD			streamPosition;
		WORD			muteDelay;
		WORD			playbackSpeed;

		DWORD			signalPosition;
		DWORD			predPosition;
				
		DWORD			stepCnt;
		
		BOOL			internalPCMClock;
		BOOL			starving, syncPlayDelay;		
		BOOL			needsPES;
		
		AC3SpeakerConfig speakerConfig;

		VirtualUnit	*	audioBus;
		
		BOOL	mute;
		WORD	lv, rv, lsv, rsv, swv, cv;
		Error SetVolume(void);

		Error SetAudioBitrate(DWORD bitrate);
		Error SetSampleRate(WORD rate);
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		Error SetAC3(BOOL ac3);
		Error SetLPCM(BOOL lpcm);

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
#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return needsPES ? MP2PES_DVD : MP2PES_ELEMENTARY;}
		Error SendPESData(HPTR data, DWORD size, DWORD __far & done, DWORD __far & used);
#endif
	   void CompleteData(void);
	
	   void PutPTS(DWORD pts);
	   
		//
		// Forward the refill request
		//
		void RefillRequest(DWORD free);
		void Interrupt(void);

	   Error SetSignalPosition(DWORD position);

		Error SyncPlay(void);

		InterruptServer	*	vblank;	
	public:
		STi4600AC3Decoder(ByteIndexedInOutPort	*	port,
					         VirtualAudioDAC	 	*	dac,
					         ASyncOutStream			*	strm,
					         InterruptServer		*	vblank,
					         VirtualUnit				*	audioBus = NULL,
					         BOOL							internalPCMClock = NULL,
					         ProgrammableClockGenerator * audioClock = NULL);
		        
		VirtualUnit * CreateVirtual(void);
		
		Error LoadPatchFile(KernelString name);
		Error Initialize(void);
	};

class VirtualSTi4600AC3Decoder : public VirtualMPEG2AC3Decoder
	{
	private:
		STi4600AC3Decoder	*	device;
	protected:
      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);		
	public:
		VirtualSTi4600AC3Decoder(STi4600AC3Decoder * device);

#if VIONA_VERSION
		MPEG2PESType NeedsPES(void) {return (lpcm || dvdStreamEncrypted || (!ac3 && audioBitrate > 384000)) ? MP2PES_DVD : MP2PES_ELEMENTARY;}
#endif
	};

#endif
