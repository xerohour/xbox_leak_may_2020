// FILE:			library\hardware\audio\specific\coac3mix.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		20.11.1996
//
// PURPOSE:		Concerto AC3 Mixer
//
// HISTORY:

#ifndef COAC3MIX_H
#define COAC3MIX_H

#include "..\generic\ac3mix.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "library\common\profiles.h"
#endif

// The different audio inputs of Concerto
enum ConcertoAudioInputs {INPUT_FM, INPUT_LINE, INPUT_CDROM, INPUT_TV, INPUT_MONO, INPUT_DVD};

// For Direct Tag Change Message's "input" member.
#define MASK_INPUT_FM			(DWORD) (1 << INPUT_FM)
#define MASK_INPUT_LINE			(DWORD) (1 << INPUT_LINE)
#define MASK_INPUT_CDROM		(DWORD) (1 << INPUT_CDROM)
#define MASK_INPUT_TV			(DWORD) (1 << INPUT_TV)
#define MASK_INPUT_MONO			(DWORD) (1 << INPUT_MONO)
#define MASK_INPUT_DVD			(DWORD) (1 << INPUT_DVD)

#ifndef ONLY_EXTERNAL_VISIBLE

struct AC3InputProperties
	{ 
	WORD	volumeLeft, volumeRight;
	BOOL	mute;
	WORD	volumeCenter;
	WORD	volumeSurroundLeft;
	WORD	volumeSurroundRight;
	WORD	volumeSubwoofer;

	WORD	delayCenter;
	WORD	delaySurround;

	AC3SpeakerConfig	speakerConfig;
	AC3BassConfig		bassConfig;
	};

enum SourceMuxSetting {SWITCH_MIXER, SWITCH_AC3};

// Total number of inputs
#define COAC3MIX_INPUTS 	6

class DTCCallback : public Node
	{
	public:
		VirtualUnit					*unit;
		DirectTagChangeHookPtr	hookPtr;
		                                
		DTCCallback(VirtualUnit * unit, DirectTagChangeHookPtr hookPtr) {this->unit = unit; this->hookPtr = hookPtr;}
	};

typedef  DTCCallback * DTCCallbackPtr;

class DTCCallbackList : public List
	{
	public:
		~DTCCallbackList();
		DTCCallbackPtr FindByUnit(VirtualUnit * unit);
	};


class ConcertoAC3Mixer : public AudioMixer {
	friend class VirtualConcertoAC3Mixer;
	private:
		DWORD				changed;
      DWORD				dChanged;
      DWORD				dChangedInputs;
      
		int	source;

		BOOL	mute;

		WORD	volumeLeft,volumeRight;
		WORD	volumeCenter;
		WORD	volumeSurroundLeft, volumeSurroundRight;
		WORD	volumeSubwoofer;

		int	sourceInput;
		int	dCurInput;
		WORD	mixerSource;
		SourceMuxSetting		sourceMux;

		VirtualUnit				*audioMux;
		VirtualUnit				*mixer;
		PhysicalUnit			*audioDAC;
		PhysicalUnit			*decoder;

		Profile					*profile;
		
		DTCCallbackList		*callbackList;

		BOOL	updateCalled;

	protected:
		BOOL	dMute;
		WORD	dVolumeLeft, dVolumeRight;
		WORD	dVolumeCenter;
		WORD	dVolumeSurroundLeft, dVolumeSurroundRight;
		WORD	dVolumeSubwoofer;

		AC3InputProperties	dProps[COAC3MIX_INPUTS];

/*
		AC3SpeakerConfig		dSpeakerConfig;
		AC3BassConfig			dBassConfig;

		WORD	dSpeakerDelayCenter;
		WORD	dSpeakerDelaySurround;
*/
	protected:
		virtual Error SetSource(WORD source);
		virtual Error SetMute(BOOL mute);
		virtual Error SetVolume(AudioChannel channel, WORD volume);

		virtual Error DirectSetMute(BOOL dMute);
		virtual Error DirectSetVolume(AudioChannel channel, WORD dVolume);

		virtual Error DirectSetInput(WORD input);
		virtual Error DirectSetInputVolume(AudioChannel channel, WORD dVolume);
		virtual Error DirectSetInputMute(BOOL mute);

		virtual Error DirectSetInputSpeakerConfig(AC3SpeakerConfig config);
		virtual Error DirectSetInputBassConfig(AC3BassConfig config);
		virtual Error DirectSetInputSpeakerDelayCenter(WORD delay);
		virtual Error DirectSetInputSpeakerDelaySurround(WORD delay);

		virtual Error SetDirectTagChangeHook(VirtualUnit * unit, DirectTagChangeHookPtr hookPtr);

		Error ProcessChanges();

		virtual Error DirectUpdate();

	public:
		ConcertoAC3Mixer(Profile * profile,
							  VirtualUnit * mixer,
							  PhysicalUnit * audioDAC); 
		~ConcertoAC3Mixer();

		void SetAudioMux(VirtualUnit * audioMux);
		void SetDecoder(PhysicalUnit * decoder);
		
		Error RetrieveConfig();
		Error StoreConfig();
      
      VirtualAudioMixer * CreateVirtualAudioMixer(void);
	};

class VirtualConcertoAC3Mixer : public VirtualAudioMixer {
	private:
		ConcertoAC3Mixer			*	mixer;

		AC3InputProperties	dProps[COAC3MIX_INPUTS];

		BOOL	mute;
		WORD	source;
   
		WORD	volumeLeft,volumeRight;
		WORD	volumeCenter;
		WORD	volumeSurroundLeft, volumeSurroundRight;
		WORD	volumeSubwoofer;
	
		BOOL	dMute;

		WORD	dVolumeLeft, dVolumeRight;
		WORD	dVolumeCenter;
		WORD	dVolumeSurroundLeft, dVolumeSurroundRight;
		WORD	dVolumeSubwoofer;
		
		int	dCurInput;

/*
		AC3SpeakerConfig		dSpeakerConfig;
		AC3BassConfig			dBassConfig;

		WORD	dSpeakerDelayCenter;
		WORD	dSpeakerDelaySurround;
*/
		DirectTagChangeHookPtr	hookPtr;
		BOOL	updateCalled;
	protected:
		Error Preempt(VirtualUnit *previous);
	public:
		VirtualConcertoAC3Mixer(ConcertoAC3Mixer *mixer);
		~VirtualConcertoAC3Mixer();
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE



#endif

