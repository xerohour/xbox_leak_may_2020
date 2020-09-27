// FILE:		   library\hardware\audio\generic\auddec.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Base class for audio decoders.
//
// HISTORY:

#ifndef AUDDEC_H
#define AUDDEC_H

#include "austrdev.h"

#ifndef ONLY_EXTERNAL_VISIBLE

class AudioDecoder : public AudioStreamDevice {
	protected:
	public:
	};

class VirtualAudioDecoder : public VirtualAudioStreamDevice {
	private:
		AudioDecoder	*	unit;	
	protected:
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualAudioDecoder(AudioDecoder *physical) : VirtualAudioStreamDevice(physical) {this->unit = physical;}
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE


#endif
