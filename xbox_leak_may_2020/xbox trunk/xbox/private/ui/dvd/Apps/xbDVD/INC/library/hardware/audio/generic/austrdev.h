// FILE:			library\hardware\audio\generic\austrdev.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Base class for audio stream devices (encoders/decoders/codecs).
//
// HISTORY:

#ifndef AUSTRDEV_H
#define AUSTRDEV_H

#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\virtunit.h"
#include "audtypes.h"
#include "library\common\hooks.h"
#include "audstrms.h"

struct AudioBufferMsg
	{
	FPTR				buffer;
	DWORD				bufferID;
	DWORD				bytesDone;
	};
	
MKHOOK(AudioBuffer, AudioBufferMsg);

typedef AudioBufferHook __far * AudioBufferHookPtr;

//
// General tags for Audio Stream Device
//
MKTAG (AUDIO_SAMPLE_RATE,			AUDIO_STREAMDEV_UNIT, 0x00000001, AudioSampleRate)		// Audio sample rate
MKTAG (AUDIO_DATA_FORMAT,			AUDIO_STREAMDEV_UNIT, 0x00000002, AudioDataFormat)		// Format of data to encode
MKTAG (AUDIO_MODE,					AUDIO_STREAMDEV_UNIT, 0x00000003, AudioMode)				// Mono/stereo selection
// Do not use this tag when a successor defines its own hook tag:
MKTAG (AUDIO_HOOK,					AUDIO_STREAMDEV_UNIT, 0x00000004, AudioBufferHookPtr)	// Provide a hook for callbacks.
MKTAG (AUDIO_SAMPLE_FREQUENCY,	AUDIO_STREAMDEV_UNIT, 0x00000005, WORD)					// Audio sample frequency in Hz

#define GNR_AUDIO_BUFFER_SUPPLIED	MKERR(ERROR, AUDIO, GENERAL, 0x01)	
#define GNR_AUDIO_UNSUPPORTED			MKERR(ERROR, AUDIO, GENERAL, 0x02)

#ifndef ONLY_EXTERNAL_VISIBLE

/////////////////////////////////////////////////////////////////////////////////
// Audio Stream Device Classes
/////////////////////////////////////////////////////////////////////////////////

class AudioStreamDevice : public PhysicalUnit {
	friend class VirtualAudioStreamDevice;
	protected:
		AudioBufferHookHandle	hook;

		virtual 	Error SetSampleRate(AudioSampleRate sampleRate) = 0;
		virtual 	Error SetDataFormat(AudioDataFormat format) = 0;
		virtual 	Error SetMode(AudioMode mode) = 0;
		virtual 	Error SetHook(AudioBufferHookPtr hook) {this->hook = hook; GNRAISE_OK;}
		
		virtual	Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID) = 0;

		virtual	Error GetPosition(DWORD __far * position) = 0;
		virtual	Error SetPosition(DWORD position) = 0;

		virtual	Error ProcessChanges() = 0;
	public:
		AudioStreamDevice() {hook = NULL;}	
	};

class VirtualAudioStreamDevice : public VirtualUnit {
	private:
		AudioStreamDevice		* 		unit;	
	protected:
		AudioSampleRate				sampleRate;
		AudioDataFormat				format;
		AudioMode						mode;
		AudioBufferHookPtr			hook;
			
		Error Preempt(VirtualUnit * previous);
	public:
		VirtualAudioStreamDevice(AudioStreamDevice *physical);
		Error Configure(TAG __far * tags);

#if NT_KERNEL
		virtual Error SupplyBuffer(FPTR info, DWORD size, DWORD bufferID);
#else
		virtual Error SupplyBuffer(HPTR buffer, DWORD size, DWORD bufferID);
#endif
		
		virtual Error GetPosition(DWORD __far * position);
		virtual Error SetPosition(DWORD position);

#if NT_KERNEL
		virtual Error PrepareBuffer(FPTR buffer, DWORD size, BOOL readOnly, FPTR __far &info);
		virtual Error UnprepareBuffer(FPTR info);
#endif
	};



#endif // ONLY_EXTERNAL_VISIBLE


#endif
