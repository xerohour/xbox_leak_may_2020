// FILE:			library\hardware\audio\specific\ad1845.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		10.03.97
//
// PURPOSE:		AD 1845 device management class.
//
// HISTORY:

#include "cs4231.h"

#ifndef AD1845_H
#define AD1845_H

// Definitions for Power Down modes of AD1845
#define ADPWRM_NO_PD		0x80000000		// No Power Down, supercedes all other flag bits

#define ADPWRM_TOTAL		0x10000000		// Total 				power down
#define ADPWRM_MIXER		0x00000001		// Mixer + DAC				"
#define ADPWRM_DAC		0x00000002		// DAC only					"
#define ADPWRM_ADC		0x00000004		// ADC 						"


class AD1845StereoCodec : public CS4231StereoCodec {
	protected:
		BOOL	outputLevelAttenuate;
		BOOL	timerEnable;
		BOOL	dacZero;

		WORD	leftMICGain, rightMICGain;
		BOOL	leftMICMixEnable, rightMICMixEnable;

		BOOL	frEnable;
		
		WORD	sourceLeft, sourceRight;
		WORD	sourceGainLeft, sourceGainRight;

		DWORD	xfs;

		Error SetClockDataFormat	(BOOL					capture,
											 AudioSampleRate	sampleRate,
											 AudioDataFormat	format,
											 AudioMode			mode);
	public:
		AD1845StereoCodec(ByteIndexedInOutPort * port, AudioFIFO * fifo, BOOL frEnable = FALSE);
		
		Error Reset						();

	   BOOL  IsThere					(void);

		Error HaltTransfers	(void);
		Error ResumeTransfers(void);
												 
		Error SetAltFeatureI			(BOOL dacZero, BOOL timerEnable, BOOL outputLevelAttenuate);
		Error SetAltFeatureII		(BOOL highPassOn) {GNRAISE_OK;}

		// AD1845 specific
		Error SetMIC					(AudioChannel channel, WORD mixGain, BOOL mixEnable);
		Error SetSampleFreq			(WORD	freq);
		Error SetPowerMode			(DWORD powerMode);

		Error SetInput					(AudioChannel channel, ADInputSource source, WORD gain, BOOL gainEnable);
		Error SetAux					(AudioChannel channel, ADAuxInputs input, WORD attenuation, BOOL mute);
		Error SetLine					(AudioChannel channel, WORD mixGain, BOOL mute);

		virtual Error TestLoopback	(BOOL __far & loopback);

		virtual Error SetXFS			(DWORD xfs);
	};

#endif

