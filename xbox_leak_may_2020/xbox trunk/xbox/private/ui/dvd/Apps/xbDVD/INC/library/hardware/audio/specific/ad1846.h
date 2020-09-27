// FILE:			library\hardware\audio\specific\ad1846.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Class for Analog Devices 1846 compatible stereo codecs. 
//
// HISTORY:

#ifndef AD1846_H
#define AD1846_H

#include "library\common\gnerrors.h"
#include "library\lowlevel\hardwrio.h"
#include "library\hardware\clocks\generic\clocks.h"
#include "..\generic\audtypes.h"
#include "..\generic\aucodecs.h"
#include "..\generic\audfifos.h"

#ifndef ONLY_EXTERNAL_VISIBLE

//
// Special data types for AD1846 compatible codecs.
//
enum ADInputSource	{AD_INPUT_LINE,
							 AD_INPUT_AUX1,
							 AD_INPUT_MICROPHONE,
							 AD_INPUT_POSTMIXED_DAC,
							 AD_INPUT_AUX2 = AD_INPUT_POSTMIXED_DAC};
							 
enum ADAuxInputs 		{AD_AUX1, AD_AUX2};

enum ADClockSource	{CS_XTAL1, CS_XTAL2};

enum ADDivideFactor	{DF_3072, DF_1536, DF_896, DF_768, DF_448, DF_384, DF_512, DF_2560};


//
// Error definitions
//
#define GNR_AUDIO_MODE_CHANGE_FAILED	MKERR(ERROR, AUDIO, GENERAL, 0x0501)
#define GNR_AUDIO_ILLEGAL_DATA_FORMAT	MKERR(ERROR, AUDIO, GENERAL, 0x0502)
#define GNR_AUDIO_ILLEGAL_SAMPLE_RATE	MKERR(ERROR, AUDIO, GENERAL, 0x0503)

//
// Misc. definitions.
//
#define MODECHANGE_TIMEOUT 1000	// Should be sufficient for the lowest sample rate



class AD1846StereoCodec : public AudioCodec {
	friend class AD1846XCTLPort;
	protected:
		PLLClockGenerator		*	clock;
		ByteIndexedInOutPort *	indPort;
		ByteIndexedInOutPort *	port;
		AudioFIFO				*	fifo;

		ADClockSource				clockSource;					// Current clock source as set by SetClockDataFormat()
		BOOL							clockSrcChangeable;			// Changes of clock source allowed?

		virtual Error ModeChangeEnable(BOOL enable);
		virtual Error WaitModeChangeFinish();
		virtual Error WaitChipReprogram();

		virtual Error SetClockDataFormat		(BOOL					capture,
														 AudioSampleRate	sampleRate,
														 AudioDataFormat	format,
														 AudioMode			mode);
	public:
		BitIndexedOutputPort	*	xctlPort;

		AD1846StereoCodec(ByteIndexedInOutPort *	port, AudioFIFO * fifo);
		
		~AD1846StereoCodec();

		Error InitPlayback	(void);
		Error EndPlayback		(void);
		
		Error InitCapture		(void);
		Error EndCapture		(void);

		Error HaltTransfers	(void);
		Error ResumeTransfers(void);

		virtual Error FIFOTest					(void);				// Just for test purposes!!!!!		

	   virtual Error Reset						(void);
	   virtual BOOL  IsThere					(void) {return TRUE;}
	   
		virtual void SetClockGenerator		(PLLClockGenerator	*	clock) {this->clock = clock;}

		virtual Error InstallPlaybackHook	(void); 
		virtual Error InstallCaptureHook		(void);

		virtual Error SetTransferRequest		(BOOL enable);
		virtual Error SetInterrupt				(BOOL enable);
		virtual Error SetPlaybackDMA			(BOOL enable);
		virtual Error SetCaptureDMA			(BOOL enable);
		virtual Error SetXCTL					(BYTE xctl);
				
		virtual Error SetInput					(AudioChannel channel, ADInputSource source, WORD gain, BOOL gainEnable);
		virtual Error SetAux						(AudioChannel channel, ADAuxInputs input, WORD attenuation, BOOL mute);
		virtual Error SetDAC						(AudioChannel channel, WORD attenuation, BOOL mute);

		virtual Error SetCaptureDataFormat	(AudioSampleRate	sampleRate,
														 AudioDataFormat	format,
														 AudioMode			mode)
															{return SetClockDataFormat(TRUE, sampleRate, format, mode);}

		virtual Error SetPlaybackDataFormat	(AudioSampleRate	sampleRate,
														 AudioDataFormat	format,
														 AudioMode			mode)
															{return SetClockDataFormat(FALSE, sampleRate, format, mode);}
																			 
		virtual Error SetAutoCalibrate		(BOOL calibrate);
		virtual Error SetLoopback				(BOOL enable, WORD attenuation);
		
		virtual Error GetOverrange				(AudioChannel channel, AudioOverrange __far & rangeLevel);

		virtual Error GetStatus					(BYTE __far & status);

		// CS4231 specific, but defined here already for consistent class hierarchy
		virtual Error SetMode2					(BOOL mode2) {GNRAISE_OK;}
		virtual Error SetLine					(AudioChannel channel, WORD mixGain, BOOL mute) {GNRAISE_OK;}
		virtual Error SetMonoInOut				(WORD attenuation, BOOL outputMute, BOOL inputMute) {GNRAISE_OK;}
		virtual Error SetAltFeatureI			(BOOL dacZero, BOOL timerEnable, BOOL outputLevelAttenuate) {GNRAISE_OK;}
		virtual Error SetAltFeatureII			(BOOL highPassOn) {GNRAISE_OK;}

		// AD1845 specific
		virtual Error SetMIC						(AudioChannel channel, WORD mixGain, BOOL mixEnable) {GNRAISE_OK;}
		virtual Error SetSampleFreq			(WORD	freq) {GNRAISE_OK;}
		virtual Error SetPowerMode				(DWORD powerMode) {GNRAISE_OK;}
	};


#endif // ONLY_EXTERNAL_VISIBLE

#endif

