// FILE:			library\hardware\audio\generic\adstcodc.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Class for Analog Devices 1846 compatible stereo codecs. 
//
// HISTORY:

#ifndef ADSTCODC_H
#define ADSTCID_H

#include "..\common\gnerrors.h"
#include "audtypes.h"

enum ADInputSource {AD_LINE, AD_AUX1, AD_MICROPHONE, AD_POSTMIXED_DAC};

enum ADAuxInputs {AD_AUX2 = AD_POSTMIXED_DAC};


class ADStereoCodec {
	protected:
		ByteIndexedInOutPort *	port;
	public:
	   ADStereoCodec(ByteIndexedInOutPort * port);

	   virtual ResetADStereoCodec();

		virtual Error Playback(BOOL run);
		virtual Error InstallPlaybackHook();
		
		virtual Error Capture(BOOL run);
		virtual Error InstallCaptureHook();
		
		virtual Error SetSources  		(SoundChannel channel, ADInputSource src);
		virtual Error SetInputGain		(SoundChannel channel, WORD gain);
		virtual Error SetMikeGain 		(SoundChannel channel, BOOL mikeGain);
		virtual Error SetAuxAttenuate	(SoundChannel channel, ADAuxInputs input, WORD att);
		virtual Error SetAuxMute		(SoundChannel channel, ADAuxInputs input, BOOL mute);
		virtual Error SetDACAttenuate	(SoundChannel channel, WORD att);
		virtual Error SetDACMute		(SoundChannel channel, BOOL mute);
		virtual Error SetSampleRate	(WORD rate);
		virtual Error SetFormat			(AudioDataFormat format, AudioDataEndianess endianess, AudioMode mode);
		virtual Error SetAutoCalibrate(BOOL calibrate);
		
		virtual Error GetOverrange		(SoundChannel channel, WORD __far & rangeLevel);
	};


#endif

