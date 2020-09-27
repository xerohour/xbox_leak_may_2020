// FILE:			library\hardware\audio\specific\mix4231.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995, 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		07.02.1996
//
// PURPOSE:		Class for audio mixers based on the CS4231 codec.
//					Adds additional control over line and mono inputs.
//
// HISTORY:

#ifndef MIX4231_H
#define MIX4231_H

#include "mix1846.h"
#include "cs4231.h"

#define MIX_4231_NUM_INPUTS 5

class Mixer4231 : public Mixer1846 {
	friend class VirtualMixer4231;
	private:
		CS4231StereoCodec	*	codec;

		DWORD	changed;

		WORD	lineGainLeft,		lineGainRight;
		BOOL	lineMuteLeft,		lineMuteRight;

		BOOL	dLineMuteLeft, dLineMuteRight;
		BOOL	dMonoInMute;
		
		WORD	monoAttenuation;
		BOOL	monoInMute;
		BOOL	monoOutMute;

	protected:   
		Error SetInputMixGain	(AudioChannel ch, WORD mixGain);
		Error SetInputMute		(AudioChannel ch, BOOL mute);
		Error SetDirectInputMute(AudioChannel ch, BOOL mute);

		virtual Error SetMonoAttenuation	(WORD);
		virtual Error SetMonoInputMute	(BOOL mute);
		virtual Error SetMonoOutputMute	(BOOL mute);
                                    
		Error ProcessChanges();		
	public:
		Mixer4231(CS4231StereoCodec	*	codec); 

		VirtualUnit * CreateVirtual(void);
	};


class VirtualMixer4231: public VirtualMixer1846 {
	private:
		Mixer4231			*	mixer;	
	protected:
		WORD	monoAttenuation;
		BOOL	monoInMute;
		BOOL	monoOutMute;
		
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualMixer4231(Mixer4231 * physical);
		Error Configure(TAG __far * tags);
	};


#endif

