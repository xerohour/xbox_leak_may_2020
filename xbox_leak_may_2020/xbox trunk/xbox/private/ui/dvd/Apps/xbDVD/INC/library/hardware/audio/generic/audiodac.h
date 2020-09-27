// FILE:			library\hardware\audio\generic\audiodac.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		04.12.96
//
// PURPOSE:		
//
// HISTORY:
#ifndef AUDIODAC_H
#define AUDIODAC_H

#include "..\..\..\common\prelude.h"
#include "..\..\..\common\gnerrors.h"
#include "..\..\..\common\virtunit.h"
#include "..\..\..\common\tagunits.h"

//
// Audio (PCM)DAC configuration parameter, esp. input and
// output format.
//
#define AUDIODAC_INPUT_MODE_I2S		MKFLAG(0)
#define AUDIODAC_INPUT_MODE_NORMAL	0L

#define AUDIODAC_INPUT_CLOCK_HLLR   MKFLAG(1)
#define AUDIODAC_INPUT_CLOCK_HLRL	0L

#define AUDIODAC_INPUT_LENGTH_18		MKFLAG(2)
#define AUDIODAC_INPUT_LENGTH_16		0L

#define AUDIODAC_INPUT_RESOLUTION	3, 2
#define AUDIODAC_INPUT_RES_16			MKBF(AUDIODAC_INPUT_RESOLUTION, 0)
#define AUDIODAC_INPUT_RES_20			MKBF(AUDIODAC_INPUT_RESOLUTION, 1)
#define AUDIODAC_INPUT_RES_24			MKBF(AUDIODAC_INPUT_RESOLUTION, 2)

#define AUDIODAC_OUTPUT_MONO			0
#define AUDIODAC_OUTPUT_REVERSE		1
#define AUDIODAC_OUTPUT_STEREO		2
#define AUDIODAC_OUTPUT_MUTE			3

//
// Audio (PCM)DAC specific tags
//
MKTAG(AUDIODAC_LEFT_VOLUME,		AUDIO_DAC_UNIT,	0x0001,	WORD)
MKTAG(AUDIODAC_RIGHT_VOLUME,		AUDIO_DAC_UNIT,	0x0002,	WORD)
MKTAG(AUDIODAC_MUTE,					AUDIO_DAC_UNIT,	0x0003,	BOOL)
MKTAG(AUDIODAC_INPUT_FORMAT,		AUDIO_DAC_UNIT,	0x0004,	DWORD)
MKTAG(AUDIODAC_OUTPUT_FORMAT,		AUDIO_DAC_UNIT,	0x0005,	DWORD)

//
// Abstract audio (PCM)DAC, straight physical/virtual unit
// implementation.
//
class AudioDAC : public PhysicalUnit
	{
	friend class VirtualAudioDAC;
	protected:
		virtual Error SetLeftVolume(WORD volume) = 0;
		virtual Error SetRightVolume(WORD volume) = 0;
		virtual Error SetMute(BOOL mute) = 0;
		virtual Error SetInputFormat(DWORD format) = 0;
		virtual Error SetOutputFormat(DWORD format) = 0;
		
	public:
		VirtualUnit * CreateVirtual(void);
		virtual VirtualAudioDAC * CreateVirtualAudioDAC(void) = 0;
	};
	
class VirtualAudioDAC : public VirtualUnit
	{
	private:
		AudioDAC	*	device;
	protected:
		WORD leftVolume;
		WORD rightVolume;
		BOOL mute;		
		DWORD inputFormat;
		DWORD outputFormat;
	public:
		VirtualAudioDAC(AudioDAC * device);
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);
	};

inline VirtualUnit * AudioDAC::CreateVirtual(void) {return CreateVirtualAudioDAC();}

MKTAG(AUDIODAC_AC3_CENTER_VOLUME,			AUDIO_DAC_UNIT,	0x1001,	WORD)
MKTAG(AUDIODAC_AC3_LEFT_SURROUND_VOLUME,  AUDIO_DAC_UNIT,	0x1002,	WORD)
MKTAG(AUDIODAC_AC3_RIGHT_SURROUND_VOLUME,	AUDIO_DAC_UNIT,	0x1003,	WORD)
MKTAG(AUDIODAC_AC3_SUBWOOFER_VOLUME,		AUDIO_DAC_UNIT,	0x1004,	WORD)

class AC3AudioDAC : public AudioDAC
	{
	friend class VirtualAC3AudioDAC;
	protected:
		virtual Error SetCenterVolume(WORD volume) = 0;
		virtual Error SetLeftSurroundVolume(WORD volume) = 0;
		virtual Error SetRightSurroundVolume(WORD volume) = 0;
		virtual Error SetSubwooferVolume(WORD volume) = 0;
	public:
		VirtualAudioDAC * CreateVirtualAudioDAC(void);
		virtual VirtualAC3AudioDAC * CreateVirtualAC3AudioDAC(void) = 0;
	};

class VirtualAC3AudioDAC : public VirtualAudioDAC
	{
	private:
		AC3AudioDAC	*	device;
	protected:
		WORD	centerVolume;
		WORD	leftSurroundVolume;
		WORD	rightSurroundVolume;
		WORD	subwooferVolume;
	public:
		VirtualAC3AudioDAC(AC3AudioDAC * device);
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);		
	};
	
inline VirtualAudioDAC * AC3AudioDAC::CreateVirtualAudioDAC(void) {return CreateVirtualAC3AudioDAC();}

class EmulatedAC3AudioDAC : public AC3AudioDAC
	{
	friend class VirtualEmulatedAC3AudioDAC;
	protected:
		VirtualAudioDAC		*	front;
		VirtualAudioDAC		*	surround;
		VirtualAudioDAC		*	effect;
	
		Error SetLeftVolume(WORD volume);
		Error SetRightVolume(WORD volume);
		Error SetMute(BOOL mute);
		Error SetInputFormat(DWORD format);
		Error SetOutputFormat(DWORD format);
		Error SetCenterVolume(WORD volume);
		Error SetLeftSurroundVolume(WORD volume);
		Error SetRightSurroundVolume(WORD volume);
		Error SetSubwooferVolume(WORD volume);
	
	public:
		EmulatedAC3AudioDAC(VirtualAudioDAC * front, VirtualAudioDAC * surround, VirtualAudioDAC * effect);	

		Error Configure(TAG __far * tags);

		VirtualAC3AudioDAC * CreateVirtualAC3AudioDAC(void);
	};
	
class VirtualEmulatedAC3AudioDAC : public VirtualAC3AudioDAC
	{
	private:
		EmulatedAC3AudioDAC	*	device;
	protected:
		BOOL	idle;
	public:
		VirtualEmulatedAC3AudioDAC(EmulatedAC3AudioDAC * device, BOOL idle = FALSE) : VirtualAC3AudioDAC(device) 
			{this->device = device; this->idle = idle;}
		Error Preempt(VirtualUnit * previous);
	};

#endif
