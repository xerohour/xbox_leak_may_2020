// FILE:			library\hardware\audio\generic\audtimer.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Base class for timers integrated in audio codecs.
//
// HISTORY:

#ifndef AUDTIMER_H
#define AUDTIMER_H

#include "..\common\gnerrors.h"
#include "..\common\tags.h"
#include "..\common\tagunits.h"
#include "..\common\virtunit.h"

MKTAG (AUDMIX_INPUT_NUMBER, AUDIO_TIMER_UNIT, 0x0001001, WORD)

#ifndef ONLY_EXTERNAL_VISIBLE

class AudioTimer : public PhysicalUnit {
	public:
		AudioTimer() {;}
	};

class VirtualAudioTimer : public VirtualUnit {
	private:
		AudioTimer		*	timer;	
	protected:
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualAudioTimer(AudioTimer *physical) {this->timer = physical};
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
