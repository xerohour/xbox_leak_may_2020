// FILE:			library\hardware\audio\specific\fadr6320.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		11.12.95
//
// PURPOSE:		Class for TEA6320 audio fader devices.
//
// HISTORY:

#ifndef FADR6320_H
#define FADR6320_H

#include "library\hardware\audio\generic\audmix.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\common\krnlstr.h"
#include "library\common\profiles.h"

#endif

// Inputs of TEA 6320

enum TEA6320Inputs {INPUT_IA, INPUT_IB, INPUT_IC, INPUT_ID, INPUT_IM};

#ifndef ONLY_EXTERNAL_VISIBLE

//
// I2C address of TEA6320
//
#define I2C_ADDRESS_TEA6320 	0x80

//
// Number of real but uncontrollable inputs of TEA6320
//
#define FADER_6320_NUM_INPUTS	5

//
// Properties for each input
//
struct InputProperties
	{ 
	WORD	faderLeft, faderRight;
	BOOL	mute;
	};


struct Fader6320Config
	{
	BOOL	faderFront;
	BOOL	faderRear;

	int	gainOffset[FADER_6320_NUM_INPUTS];
	};

class Fader6320 : public AudioMixer {
	friend class VirtualFader6320;

	private:
		DWORD				changed;
   
   	Fader6320Config	config;
   
		BYTE	source;
		int 	gain;
		BOOL	mute;
		BOOL	loudness;
		BOOL	zeroCrossing;
		WORD 	faderLeft, faderRight;
		int 	bass, treble;

		WORD	dFaderLeft, dFaderRight;
		BOOL	dMute;
		BOOL	dLoudness;
		WORD	dBass, dTreble;

		int	sourceInput;
		
		int	curInput;
		InputProperties	props[FADER_6320_NUM_INPUTS];

#if !NT_KERNEL && !VXD_VERSION
		HINSTANCE hinst;
#else
		Profile*	profile;
#endif
	protected:
		ByteIndexedOutputPort * port;

		virtual Error SetSource(WORD source);
		virtual Error SetGain(int gain);
		virtual Error SetMute(BOOL mute);
		virtual Error SetLoudness(BOOL loudness);
		virtual Error SetZeroCrossing(BOOL zeroCrossing);

		virtual Error SetFader(AudioChannel channel, WORD faderValue);

		virtual Error SetBass(int bass);
		virtual Error SetTreble(int treble);

		virtual Error DirectSetMute(BOOL dMute);
		virtual Error DirectSetLoudness(BOOL dLoudness);
		virtual Error DirectSetFaderLeft(WORD dFaderLeft);
		virtual Error DirectSetFaderRight(WORD dFaderRight);
		virtual Error DirectSetBass(WORD dBass);
		virtual Error DirectSetTreble(WORD dTreble);

		virtual Error DirectSetInput(WORD input);
		virtual Error DirectSetInputFader(AudioChannel channel, WORD dFaderValue);
		virtual Error DirectSetInputMute(BOOL mute);

		virtual Error ProcessChanges();
		
	public:
#if !NT_KERNEL && !VXD_VERSION
		Fader6320(ByteIndexedOutputPort * port, HINSTANCE hinst); 
		Error RetrieveINIValues(KernelString profileName);
#else
		Fader6320(ByteIndexedOutputPort * port, Profile * profile); 
		Error RetrieveINIValues();
#endif
		Error	Configure(Fader6320Config __far & config);
		
		VirtualAudioMixer * CreateVirtualAudioMixer(void);
	};

class VirtualFader6320 : public VirtualAudioMixer {
	private:
		Fader6320			*	fader;	

		int	curInput;
		InputProperties	props[FADER_6320_NUM_INPUTS];
	protected:
		WORD	source;
		int 	gain;
		BOOL	mute;
		BOOL	loudness;
		BOOL	zeroCrossing;
		WORD	faderLeft, faderRight;
		int 	bass, treble;
				
		Error Preempt(VirtualUnit *previous);

	public:
		VirtualFader6320(Fader6320 *physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE

#endif
