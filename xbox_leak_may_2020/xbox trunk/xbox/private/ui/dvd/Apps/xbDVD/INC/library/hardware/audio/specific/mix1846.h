// FILE:			library\hardware\audio\specific\mix1846.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Class for AD1846 based audio mixer.
//
// HISTORY:

#ifndef MIX1846_H
#define MIX1846_H

#include "..\generic\audmix.h"
#include "ad1846.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#define MIX_1846_NUM_INPUTS 4

struct InputProperties
	{ 
	WORD	mixGainLeft,		mixGainRight;
	BOOL	muteLeft,			muteRight;
	};

class Mixer1846 : public AudioMixer {
	friend class VirtualMixer1846;
	private:
		AD1846StereoCodec	*	codec;

		DWORD	changed;

		WORD	sourceLeft, 		sourceRight;
		WORD	gainLeft, 			gainRight;
		BOOL	gainEnableLeft,	gainEnableRight;
		WORD	attLeft, 			attRight;
		BOOL	muteLeft, 			muteRight;

		WORD	dGainLeft,			dGainRight;
		WORD	dAttLeft,			dAttRight;
		BOOL	dMuteLeft,			dMuteRight;

		WORD	loopBackAtt;
		BOOL	loopBack;

		BOOL	directMute;

		InputProperties			props[MIX_1846_NUM_INPUTS];
		InputProperties			dProps[MIX_1846_NUM_INPUTS];
	protected:
		int	currentDirectInput;
		
		virtual Error SetCurrentDirectInput	(int currentDirectInput);

		virtual Error SetSource			(AudioChannel ch, WORD src);
		virtual Error SetGain			(AudioChannel ch, WORD gain);
		virtual Error SetGainEnable	(AudioChannel ch, WORD gainEnable);
		virtual Error SetAttenuation	(AudioChannel ch, WORD att);
		virtual Error SetMute			(AudioChannel ch, BOOL mute);

		virtual Error SetDirectGain	(AudioChannel ch, WORD gain);
		virtual Error SetDirectAtt		(AudioChannel ch, WORD att);
		virtual Error SetDirectMute	(AudioChannel ch, BOOL mute);
		virtual Error SetDirectInputMute(AudioChannel ch, BOOL mute);
                                    
		virtual Error SetInputMixGain	(AudioChannel ch, WORD mixGain);
		virtual Error SetInputMute		(AudioChannel ch, BOOL mute);
                                    
		virtual Error SetLoopback		(BOOL loopBack);
		virtual Error SetLoopbackAttenuation(WORD att);

		Error ProcessChanges();		
	public:
		Mixer1846(AD1846StereoCodec	*	codec); 

		VirtualUnit * CreateVirtual(void);
	};


class VirtualMixer1846 : public VirtualAudioMixer {
	private:
		Mixer1846			*	mixer;	

		InputProperties	props[MIX_1846_NUM_INPUTS];
	protected:
		WORD	sourceLeft, 		sourceRight;
		WORD	gainLeft, 			gainRight;
		BOOL	gainEnableLeft,	gainEnableRight;
		WORD	attLeft, 			attRight;
		BOOL	muteLeft, 			muteRight;

		WORD	loopBackAtt;
		BOOL	loopBack;

		WORD	dGainLeft,			dGainRight;
		WORD	dAttLeft,			dAttRight;
		BOOL	dMuteLeft,			dMuteRight;
		
		BOOL	dInputMuteLeft,	dInputMuteRight;

		int	curDirectInput;
		
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualMixer1846(Mixer1846 *physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
