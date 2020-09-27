// FILE:			library\hardware\audio\generic\audenc.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Base class for audio encoders.
//
// HISTORY:

#ifndef AUDENC_H
#define AUDENC_H

#include "austrdev.h"

#ifndef ONLY_EXTERNAL_VISIBLE

class AudioEncoder : public AudioStreamDevice {
	protected:
	public:
	};

class VirtualAudioEncoder : public VirtualAudioStreamDevice {
	private:
		AudioEncoder	*	unit;	
	protected:
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualAudioEncoder(AudioEncoder *physical) : VirtualAudioStreamDevice(physical) {this->unit = physical;}
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE


#endif
