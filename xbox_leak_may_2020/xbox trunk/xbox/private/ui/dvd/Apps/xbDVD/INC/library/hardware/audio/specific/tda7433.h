// FILE:			library\hardware\audio\specific\tda7433.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		08.02.97
//
// PURPOSE:		Class for TDA7433 Basic Function Audio Processor
//
// HISTORY:

#ifndef TDA7433_H
#define TDA7433_H

#include "library\hardware\audio\generic\audmix.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"
#include "library\common\krnlstr.h"
#include "library\common\profiles.h"

#endif

// Inputs of TDA 733

enum TDA7433Inputs {INPUT_IN1, INPUT_IN2, INPUT_MONO};

#ifndef ONLY_EXTERNAL_VISIBLE

//
// I2C address of TDA 7433
//
#define I2C_ADDRESS_TDA7433 	0x8a

//
// Number of inputs of TDA7433
//
#define TDA7433_NUM_INPUTS	3

//
// Properties for each input
//
struct TDA7433InputProperties
	{ 
	WORD	volumeLeft, volumeRight;
	BOOL	mute;
	};


struct TDA7433Config
	{
	BOOL	symBassCut;
	BOOL	extendedBassRange;
	int	gainOffset[TDA7433_NUM_INPUTS];
	};


class TDA7433 : public AudioMixer {
	friend class VirtualTDA7433;

	private:
		DWORD				changed;

   	TDA7433Config	config;

		BYTE	source;

		WORD	gain;
		WORD	volumeLeft,			volumeRight;
		BOOL	faderFrontMute,	faderRearMute;
		
		WORD	dBass, dTreble;

		WORD	dVolumeLeft, dVolumeRight;
		BOOL	dFaderFrontMute,	dFaderRearMute;
		
		TDA7433InputProperties			dProps[TDA7433_NUM_INPUTS];
                        	
		int	sourceInput;
		int	curInput;

		Profile	* profile;
	protected:
		ByteIndexedOutputPort * port;

		virtual Error SetSource(WORD source);

		virtual Error SetVolume(AudioChannel channel, WORD volumeValue);
		virtual Error SetFaderMute(FaderType type, BOOL mute);

		virtual Error DirectSetMute(BOOL dMute);
		virtual Error DirectSetBass(WORD dBass);
		virtual Error DirectSetTreble(WORD dTreble);
		virtual Error DirectSetVolume(AudioChannel channel, WORD dVolumeValue);
		virtual Error DirectSetInput(WORD input);
		virtual Error DirectSetInputVolume(AudioChannel channel, WORD dVolumeValue);
		virtual Error DirectSetInputMute(BOOL mute);

		virtual Error ProgramVolume();

		virtual Error ProcessChanges();
		
	public:
		TDA7433(ByteIndexedOutputPort * port, Profile * profile); 
		
		Error ReInitialize(void);

		Error RetrieveINIValues();
		VirtualAudioMixer * CreateVirtualAudioMixer(void);

		Error	ConfigureMixer(TDA7433Config __far & config);
	};

class VirtualTDA7433 : public VirtualAudioMixer {
	private:
		TDA7433			*	fader;	

		int	curInput;

		WORD	source;

		BOOL	mute;
		WORD	gain;

		WORD	volumeLeft, volumeRight;

		BOOL	faderFrontMute,	faderRearMute;

		WORD	dVolumeLeft, dVolumeRight;

		int 	bass, treble;
		
		TDA7433InputProperties	dProps[TDA7433_NUM_INPUTS];
				
		Error Preempt(VirtualUnit *previous);

	public:
		VirtualTDA7433(TDA7433 *physical);
		Error Configure(TAG __far * tags);
	};

#endif // ONLY_EXTERNAL_VISIBLE



#endif
