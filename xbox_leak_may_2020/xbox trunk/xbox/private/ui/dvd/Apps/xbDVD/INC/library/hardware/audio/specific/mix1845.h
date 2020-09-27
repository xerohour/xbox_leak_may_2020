// FILE:			library\hardware\audio\specific\mix1845.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.03.1997
//
// PURPOSE:		Class for audio mixers based on the AD1845 codec.
//					Adds additional control over MIC input.
//
// HISTORY:

#ifndef MIX1845_H
#define MIX1845_H

#ifndef ONLY_EXTERNAL_VISIBLE

#include "mix4231.h"
#include "ad1845.h"

class Mixer1845 : public Mixer4231 {
	friend class VirtualMixer1845;
	private:
		AD1845StereoCodec	*	codec;

		DWORD	changed;

		WORD	micGainLeft,		micGainRight;
		BOOL	micMixMuteLeft,	micMixMuteRight;

		BOOL	dMicMixMuteLeft,	dMicMixMuteRight;

	protected:   
		Error SetInputMixGain	(AudioChannel ch, WORD mixGain);
		Error SetInputMute		(AudioChannel ch, BOOL mute);
		Error SetDirectInputMute(AudioChannel ch, BOOL mute);

		Error ProcessChanges();		
	public:
		Mixer1845(AD1845StereoCodec	*	codec); 

		VirtualUnit * CreateVirtual(void);
	};


class VirtualMixer1845: public VirtualMixer4231 {
	public:
		VirtualMixer1845(Mixer1845 * physical) : VirtualMixer4231(physical) {;}
	};

#endif // of ONLY_EXTERNAL_VISIBLE

#endif

