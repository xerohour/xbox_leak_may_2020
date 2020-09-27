// FILE:			library\hardware\mpeg3dec\specific\ac3exdec.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.95
//
// PURPOSE:		
//
// HISTORY:
#ifndef AC3EXDEC_H
#define AC3EXDEC_H

#include "..\generic\mp2eldec.h"
#include "..\..\..\common\profiles.h"

class ExternalAC3Decoder : public MPEG2AC3Decoder
	{
	friend class VirtualExternalAC3Decoder;
	protected:
		PTSAssoc		ptsAssoc;
		
		DWORD 		audioBitrate;
		MPEGState	state;
		BOOL			ac3;		
		
		WORD			leftVolume, rightVolume;
		BOOL			mute;    
		
		DWORD			transferLocation;
		
		DWORD			decoderDelay;
		
		Error SetAudioBitrate(DWORD bitrate);
		Error SetSampleRate(WORD rate);
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		Error SetAC3(BOOL ac3);
		Error SetBassConfig(AC3BassConfig config);
		Error SetSpeakerConfig(AC3SpeakerConfig config);
		Error SetDualModeConfig(AC3DualModeConfig config) {GNRAISE_OK;}
		Error SetKaraokeConfig(AC3KaraokeConfig config) {GNRAISE_OK;}
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

	   Error SetSignalPosition(DWORD position);
	   
	   friend void __far pascal RefillCallback(DWORD idata, DWORD free);
	
	public:
		ExternalAC3Decoder(Profile * profile);
		~ExternalAC3Decoder(void);
		
		VirtualUnit * CreateVirtual(void);		
	};
	
class VirtualExternalAC3Decoder : public VirtualMPEG2AC3Decoder
	{
	private:
		ExternalAC3Decoder	*	device;
	protected:
      Error PreemptStopPrevious(VirtualUnit * previous);
      Error PreemptStartNew(VirtualUnit * previous);		
	public:
		VirtualExternalAC3Decoder(ExternalAC3Decoder * device);	
	};
	
#endif
