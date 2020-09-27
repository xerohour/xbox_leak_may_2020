// FILE:			library\hardware\audio\specific\tda9850.h
// AUTHOR:		Stefan Herr
// COPYRIGHT:	(c) 1996 Viona Development.  All Rights Reserved.
// CREATED:		22.08.95
//
// PURPOSE:		Class for Philips TDA9850 TV stereo decoders
//
// HISTORY:

#ifndef TDA9850_H
#define TDA9850_H

#include "library\lowlevel\hardwrio.h"
#include "library\hardware\audio\generic\stereodc.h"

#define I2C_TDA9850	0xb4

enum ADJBitSetting {MODE_STEREO_DECODER, MODE_AUTO_ADJUST};
enum STSBitSetting {STON_LT_30MV, STON_LT_35MV};

MKTAG(STEREODEC_LEVEL,							STEREO_DECODER_UNIT,	0x101, WORD)
MKTAG(STEREODEC_STEREO_NOISE_THRESHOLD,	STEREO_DECODER_UNIT,	0x102, WORD)
MKTAG(STEREODEC_SAP_NOISE_THRESHOLD,		STEREO_DECODER_UNIT,	0x103, WORD)
MKTAG(STEREODEC_TIMING_CURRENT,				STEREO_DECODER_UNIT,	0x104, WORD)
MKTAG(STEREODEC_ADJ,								STEREO_DECODER_UNIT,	0x105, ADJBitSetting)
MKTAG(STEREODEC_STS,								STEREO_DECODER_UNIT,	0x106, STSBitSetting)
MKTAG(STEREODEC_MUTE,							STEREO_DECODER_UNIT,	0x107, BOOL)
MKTAG(STEREODEC_ALIGNMENT_1,					STEREO_DECODER_UNIT, 0x108, WORD)
MKTAG(STEREODEC_ALIGNMENT_2,					STEREO_DECODER_UNIT, 0x109, WORD)

class TDA9850 : public StereoDecoder	{
	friend class VirtualTDA9850;
	protected:
		ByteIndexedInOutPort * port;

		DWORD	changed;
	
		BOOL	sap;
		BOOL	stereo;
		
		ADJBitSetting	adj;
		STSBitSetting	sts;

		WORD	level, stereoNoiseThreshold, sapNoiseThreshold, timingCurrent;
		BOOL	mute;
		WORD	alignment1, alignment2;

		Error GetSignalMode(SignalMode __far & mode);
		Error SetSignalMode(SignalMode mode);

		virtual Error SetLevel(WORD level);
		virtual Error SetStereoNoiseThreshold(WORD stereoNoiseThreshold);
		virtual Error SetSAPNoiseThreshold(WORD sapNoiseThreshold);
		virtual Error SetTimingCurrent(WORD timingCurrent);
		virtual Error SetADJ(ADJBitSetting adj);
		virtual Error SetSTS(STSBitSetting sts);
		virtual Error SetMute(BOOL mute);
		virtual Error SetAlignment1(WORD alignment1);
		virtual Error SetAlignment2(WORD alignment2);

		Error ProcessChanges();
	public:
		TDA9850(ByteIndexedInOutPort * port);
		
		VirtualUnit * CreateVirtual(void); 
	};
	
class VirtualTDA9850 : public VirtualStereoDecoder {
	private:
		TDA9850	*	decoder;
	protected:
		ADJBitSetting	adj;
		STSBitSetting	sts;

		WORD	level, stereoNoiseThreshold, sapNoiseThreshold, timingCurrent;
		BOOL	mute;
		WORD	alignment1, alignment2;
		
	public:
		VirtualTDA9850(TDA9850 * decoder);
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);
	};
#endif
