// FILE:			library\hardware\audio\specific\fadr7318.h
// AUTHOR:		P.C. Straasheijm
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.01.97
//
// PURPOSE:		Class for TDA7318 audio fader devices. Implementation.
//
// HISTORY:


#ifndef TDA7318_H
#define TDA7318_H

#include "library\hardware\audio\generic\audmix.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\common\krnlstr.h"
#include "library\common\profiles.h"

#endif

// Inputs of TDA 733

enum TDA7318Inputs {INPUT_IN2, INPUT_IN1, INPUT_IN3, INPUT_IN4};

#ifndef ONLY_EXTERNAL_VISIBLE

//
// I2C address of TDA 7318
//
#define I2C_ADDRESS_TDA7318 	0x88

//
// Number of inputs of TDA7318
//
#define TDA7318_NUM_INPUTS	4

//
// Properties for each input
//
struct InputProperties
	{ 
	WORD	faderFrontLeft, faderFrontRight;
	WORD	faderRearLeft, faderRearRight;
	BOOL	mute;
	};


struct TDA7318Config
	{
	BOOL	symBassCut;
	BOOL	extendedBassRange;
	int	gainOffset[TDA7318_NUM_INPUTS];
	};


class TDA7318 : public AudioMixer {
	friend class VirtualTDA7318;

	private:
		DWORD				changed;

   	TDA7318Config	config;

		BYTE	source;

		int 	bass, treble;

		WORD	gain;
		WORD 	faderFrontLeft,	faderFrontRight;
		WORD 	faderRearLeft,		faderRearRight;
		BOOL	faderFrontMute,	faderRearMute;
		
		WORD	dBass, dTreble;

		WORD	dGain;
		WORD 	dFaderFrontLeft,	dFaderFrontRight;
		WORD 	dFaderRearLeft,	dFaderRearRight;
		BOOL	dFaderFrontMute,	dFaderRearMute;
		
		InputProperties			dProps[TDA7318_NUM_INPUTS];
                        	
		int	sourceInput;
		int	curInput;

		Profile	* profile;
	protected:
		ByteIndexedOutputPort * port;

		virtual Error SetSource(WORD source);

		virtual Error SetGain(WORD gain);
//		virtual Error SetSymBassCut(BOOL symBassCut);
//		virtual Error SetExtendedBassRange(BOOL extendedBassRange);
		
		virtual Error SetFader(FaderType type, AudioChannel channel, WORD faderValue);
		virtual Error SetFaderMute(FaderType type, BOOL mute);


		virtual Error DirectSetMute(BOOL dMute);

		virtual Error DirectSetGain(WORD dGain);
		virtual Error DirectSetBass(WORD dBass);
		virtual Error DirectSetTreble(WORD dTreble);

		virtual Error DirectSetFader(FaderType type, AudioChannel channel, WORD dFaderValue);

		virtual Error DirectSetInput(WORD input);
		virtual Error DirectSetInputFader(FaderType type, AudioChannel channel, WORD dFaderValue);
		virtual Error DirectSetInputMute(BOOL mute);

		virtual Error ProcessChanges();
		
	public:
		TDA7318(ByteIndexedOutputPort * port, Profile * profile); 
		
		Error RetrieveINIValues();
		VirtualUnit * CreateVirtual(void);

		Error	Configure(TDA7318Config __far & config);
	};

class VirtualTDA7318 : public VirtualAudioMixer {
	private:
		TDA7318			*	fader;	

		int	curInput;

		WORD	source;

		BOOL	mute;
		WORD	gain;

		WORD 	faderFrontLeft,	faderFrontRight;
		WORD 	faderRearLeft,		faderRearRight;
		BOOL	faderFrontMute,	faderRearMute;

		WORD 	dFaderFrontLeft,	dFaderFrontRight;
		WORD 	dFaderRearLeft,	dFaderRearRight;

		int 	bass, treble;
		
		InputProperties	dProps[TDA7318_NUM_INPUTS];
				
		Error Preempt(VirtualUnit *previous);

	public:
		VirtualTDA7318(TDA7318 *physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE



#endif
