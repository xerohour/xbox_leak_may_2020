// FILE:			library\hardware\audio\specific\fadr6300.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		18.09.96
//
// PURPOSE:		Class for TEA6300 audio fader devices.
//
// HISTORY:

#ifndef FADR6300_H
#define FADR6300_H

#include "library\hardware\audio\generic\audmix.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\common\profiles.h"

#endif

// Inputs of TEA 6300

enum TEA6300Inputs {INPUT_IA, INPUT_IB, INPUT_IC};

// AUDMIX_D_INPUT_MUTE already defined in audmix.h!

#ifndef ONLY_EXTERNAL_VISIBLE

//
// I2C address of TEA6300
//
#define I2C_ADDRESS_TEA6300 	0x80

//
// Number of real but uncontrollable inputs of TEA6300
//
#define FADER_6300_NUM_INPUTS	3

//
// Properties for each input
//
struct TEA6300InputProperties
	{ 
	WORD	volumeLeft, volumeRight;
	BOOL	mute;
	};

class Fader6300 : public AudioMixer {
	friend class VirtualFader6300;

	private:
		DWORD				changed;

		BYTE	source;

		BOOL	mute;
		WORD	volumeLeft,			volumeRight;
		WORD 	faderFront,			faderRear;
		BOOL	faderFrontMute,	faderRearMute;
		int 	bass, treble;

		BOOL	dMute;
		WORD	dVolumeLeft, 		dVolumeRight;
		WORD	dBass, dTreble;

		int	sourceInput;
		
		int	curInput;
		TEA6300InputProperties	props[FADER_6300_NUM_INPUTS];

#if NT_KERNEL
		Profile	* profile;
#else
		HINSTANCE hinst;
#endif
	protected:
		ByteIndexedOutputPort * port;

		virtual Error SetSource(WORD source);
		virtual Error SetMute(BOOL mute);

		virtual Error SetFader(FaderType type, WORD faderValue);
		virtual Error SetFaderMute(FaderType type, BOOL mute);

		virtual Error SetVolume(AudioChannel channel, WORD volume);

		virtual Error SetBass(int bass);
		virtual Error SetTreble(int treble);

		virtual Error DirectSetMute(BOOL dMute);
		virtual Error DirectSetBass(WORD dBass);
		virtual Error DirectSetTreble(WORD dTreble);

		virtual Error DirectSetVolume(AudioChannel channel, WORD dVolume);

		virtual Error DirectSetInput(WORD input);
		virtual Error DirectSetInputVolume(AudioChannel channel, WORD dVolume);
		virtual Error DirectSetInputMute(BOOL mute);

		virtual Error ProcessChanges();
		
	public:
#if NT_KERNEL
		Fader6300(ByteIndexedOutputPort * port, Profile * profile);
#else
		Fader6300(ByteIndexedOutputPort * port, HINSTANCE hinst); 
#endif
		
		Error RetrieveINIValues();
		VirtualAudioMixer * CreateVirtualAudioMixer(void);
	};

class VirtualFader6300 : public VirtualAudioMixer {
	private:
		Fader6300			*	fader;	

		int	curInput;
		TEA6300InputProperties	props[FADER_6300_NUM_INPUTS];

		WORD	source;
		BOOL	mute;
		WORD	volumeLeft,			volumeRight;
		WORD	faderFront,			faderRear;
		BOOL	faderFrontMute,	faderRearMute;
		int 	bass, treble;
		
		WORD	dVolumeLeft, dVolumeRight;
				
		Error Preempt(VirtualUnit *previous);

	public:
		VirtualFader6300(Fader6300 *physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
