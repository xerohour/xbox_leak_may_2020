// FILE:			library\hardware\audio\specific\cs4231.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995, 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Crystal CS4231 device management class.
//
// HISTORY:

#include "ad1846.h"

#ifndef CS4231_H
#define CS4231_H

enum CSInputSource	{CS_INPUT_LINE,
							 CS_INPUT_AUX1,
							 CS_INPUT_MICROPHONE,
							 CS_INPUT_POSTMIXED_DAC,
							 CS_INPUT_AUX2 = CS_INPUT_POSTMIXED_DAC,
							 CS_INPUT_MONO
							};

class CS4231StereoCodec : public AD1846StereoCodec {
	protected:
		Error SetClockDataFormat		(BOOL					capture,
												 AudioSampleRate	sampleRate,
												 AudioDataFormat	format,
												 AudioMode			mode);
	public:
		CS4231StereoCodec(ByteIndexedInOutPort * port, AudioFIFO * fifo) : AD1846StereoCodec(port, fifo) {;}
		
		Error Reset							(void);

												 
		Error SetMode2					(BOOL mode2);
		Error SetLine					(AudioChannel channel, WORD mixGain, BOOL mute);
		Error SetMonoInOut			(WORD attenuation, BOOL outputMute, BOOL inputMute);
		Error SetAltFeatureI			(BOOL dacZero, BOOL timerEnable, BOOL outputLevelAttenuate);
		Error SetAltFeatureII		(BOOL highPassOn);
	};

#endif

