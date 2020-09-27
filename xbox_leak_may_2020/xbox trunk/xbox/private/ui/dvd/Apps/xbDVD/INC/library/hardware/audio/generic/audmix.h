////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// FILE:			library\hardware\audio\generic\audmix.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Base class for audio mixers.
//
// HISTORY:

#ifndef AUDMIX_H
#define AUDMIX_H

#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\virtunit.h"
#include "library\common\hooks.h"
#include "audtypes.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Input capabilities
/////////////////////////////////////////////////////////////////////////////////////////////

#define INPUT_NO_CAPTURE			0x00000001		// This input cannot be captured (digitized).
#define INPUT_STEREO					0x00000002		// This is a stereo input.
#define INPUT_CAPTURE_LOOPBACK	0x00000004		// Capture is only possible via loopback mode.
#define INPUT_MIXABLE				0x00000008		// Input may be mixed to output (i.e. gain/att. may be set).
#define INPUT_MUTABLE				0x00000010		// Input may be muted (concerns mix with output).

/////////////////////////////////////////////////////////////////////////////////////////////
// Definition of input types.
/////////////////////////////////////////////////////////////////////////////////////////////

enum MixerInputType {IN_MIC, IN_LINE, IN_AUX, IN_MONO, IN_OTHER, IN_NOT_CONNECTED};

/////////////////////////////////////////////////////////////////////////////////////////////
// Description structure for each input.
/////////////////////////////////////////////////////////////////////////////////////////////

struct MixerInput {
	MixerInputType	type;			// Type of mixer input.
	DWORD				caps;			// Capabilities (see above).
	char				name[50];	// Name for this input.
	};


typedef MixerInput __far * MixerInputArrayPtr;

/////////////////////////////////////////////////////////////////////////////////////////////
// Definition of callback hook for Direct Tag changes
/////////////////////////////////////////////////////////////////////////////////////////////


struct DirectTagChangeMsg
	{
	DWORD		flags;		// See definitions above
	DWORD		inputs;		// If DTCM_AFFECTS_INPUT is contained in flags, this tells which
								// inputs have been affected (see specific mixer implementations).
	};

// Flag definitions for Direct Tag Change Message "flags" field
#define DTCM_CHANGED_GENERAL			0x00000001		// The general Direct Tags have been changed
																// General means: Not specifically for a certain
																// input.
#define DTCM_CHANGED_INPUTS			0x00000002		// The Direct Tag values were changed for
																// specific inputs.
// Flags specific for inputs
#define DTCM_CHANGED_INPUTS_GENERAL	0x00001000	// Changed general settings for an input


MKHOOK(DirectTagChange, DirectTagChangeMsg)

typedef DirectTagChangeHook __far * DirectTagChangeHookPtr;



struct NotificationMsg
	{
	DWORD	senderID;
	DWORD	id;
	DWORD	type;
	DWORD	msg;
	DWORD	data;
	};

MKHOOK(Notification, NotificationMsg)

typedef NotificationHook * NotificationHookPtr;

// Use this for notifications that have to be transferred to different process instances
// through the kernel mode driver. As it may happen that there is only one virtual unit
// for all process instances (like for a High Level Windows Mixer), we need to identify
// the hook, which we do by using "id".
// To remove the hook, set "hook" to NULL, but keep "id" valid.
struct IdentifiedNotification
	{
	NotificationHookPtr	hook;
	DWORD						id;
	};

typedef IdentifiedNotification __far * IdentifiedNotificationPtr;

#define NOTIFY_MIXER	MKFLAG(0)



/////////////////////////////////////////////////////////////////////////////////////////////
// Tag Definitions
/////////////////////////////////////////////////////////////////////////////////////////////

// General tags for Audio Mixer
//
MKTAG (AUDMIX_CAPTURE_SELECT,					AUDIO_MIXER_UNIT, 0x0001, WORD)	// Selects input to capture.
MKTAG (AUDMIX_CAPTURE_SELECT_LEFT,			AUDIO_MIXER_UNIT, 0x0002, WORD)	//   dto. for left channel only.
MKTAG (AUDMIX_CAPTURE_SELECT_RIGHT,			AUDIO_MIXER_UNIT, 0x0003, WORD)	//   dto. for right channel only.
MKTAG (AUDMIX_GAIN,								AUDIO_MIXER_UNIT, 0x0004, WORD)	// Sets ADC gain.
MKTAG (AUDMIX_GAIN_LEFT,						AUDIO_MIXER_UNIT, 0x0005, WORD)
MKTAG (AUDMIX_GAIN_RIGHT,						AUDIO_MIXER_UNIT, 0x0006, WORD)
MKTAG (AUDMIX_GAIN_ENABLE_LEFT,				AUDIO_MIXER_UNIT, 0x0007, BOOL)	// Enables ADC extra gain for mic.
MKTAG (AUDMIX_GAIN_ENABLE_RIGHT,				AUDIO_MIXER_UNIT, 0x0008, BOOL)
MKTAG (AUDMIX_ATTENUATE,						AUDIO_MIXER_UNIT, 0x0009, WORD)	// Sets DAC attenuation.
MKTAG (AUDMIX_ATTENUATE_LEFT,					AUDIO_MIXER_UNIT, 0x000a, WORD)
MKTAG (AUDMIX_ATTENUATE_RIGHT,				AUDIO_MIXER_UNIT, 0x000b, WORD)
MKTAG (AUDMIX_MUTE,								AUDIO_MIXER_UNIT, 0x000c, BOOL)	// Sets DAC mute.
MKTAG (AUDMIX_MUTE_LEFT,						AUDIO_MIXER_UNIT, 0x000d, BOOL)
MKTAG (AUDMIX_MUTE_RIGHT,						AUDIO_MIXER_UNIT, 0x000e, BOOL)

// Tags for input properties/selection
//
MKTAG (AUDMIX_INPUT_NUMBER, 					AUDIO_MIXER_UNIT, 0x000f, WORD)	// Delivers total number of inputs.
MKTAG (AUDMIX_MIXER_DESCRIPTION,				AUDIO_MIXER_UNIT, 0x0010, MixerInputArrayPtr)
// The next tag selects one of the inputs, for which the following tags can then be applied.
MKTAG (AUDMIX_INPUT_SELECT,					AUDIO_MIXER_UNIT, 0x0000011, WORD)
//
// The following tags are used to set the properties of an input.
// Use QUERY_... to find out the capabilities for a certain input. If tags like AUDMIX_INPUT_GAIN
// deliver FALSE (i.e. input is stereo), then you MUST check AUDMIX_INPUT_GAIN_LEFT/_RIGHT.
// If it is TRUE, then this is not necessary.
//
MKTAG (AUDMIX_INPUT_GAIN,						AUDIO_MIXER_UNIT, 0x0012, WORD)	// Gain (to the ADC) is changeable (mono).
MKTAG (AUDMIX_INPUT_GAIN_LEFT,				AUDIO_MIXER_UNIT, 0x0013, WORD)	// Gain is independently changeable for
MKTAG (AUDMIX_INPUT_GAIN_RIGHT,				AUDIO_MIXER_UNIT, 0x0014, WORD)	// left/right channels.
MKTAG (AUDMIX_INPUT_MIX_GAIN,					AUDIO_MIXER_UNIT, 0x0015, WORD)	// Analog output mix gain for the input.
MKTAG (AUDMIX_INPUT_MIX_GAIN_LEFT,			AUDIO_MIXER_UNIT, 0x0016, WORD)
MKTAG (AUDMIX_INPUT_MIX_GAIN_RIGHT,			AUDIO_MIXER_UNIT, 0x0017, WORD)
MKTAG (AUDMIX_INPUT_MIX_ATTENUATE,			AUDIO_MIXER_UNIT, 0x0018, WORD)	// Analog output mix attenuation.
MKTAG (AUDMIX_INPUT_MIX_ATTENUATE_LEFT,	AUDIO_MIXER_UNIT, 0x0019, WORD)
MKTAG (AUDMIX_INPUT_MIX_ATTENUATE_RIGHT,	AUDIO_MIXER_UNIT, 0x001a, WORD)
MKTAG (AUDMIX_INPUT_GAIN_ENABLE,				AUDIO_MIXER_UNIT, 0x001b, BOOL)	// Enables extra gain for input.
MKTAG (AUDMIX_INPUT_GAIN_ENABLE_LEFT,		AUDIO_MIXER_UNIT, 0x0001c, BOOL)
MKTAG (AUDMIX_INPUT_GAIN_ENABLE_RIGHT,		AUDIO_MIXER_UNIT, 0x001d, BOOL)
MKTAG (AUDMIX_INPUT_MUTE,						AUDIO_MIXER_UNIT, 0x001e, BOOL)	// Mutes input for the analog output mix.
MKTAG (AUDMIX_INPUT_MUTE_LEFT,				AUDIO_MIXER_UNIT, 0x001f, BOOL)
MKTAG (AUDMIX_INPUT_MUTE_RIGHT,				AUDIO_MIXER_UNIT, 0x0020, BOOL)

// Special tags for more advanced mixers.
//
MKTAG (AUDMIX_LOOPBACK,							AUDIO_MIXER_UNIT, 0x0021, BOOL)	// Enables digital loopback (ADC->DAC)
MKTAG (AUDMIX_LOOPBACK_ATTENUATION, 		AUDIO_MIXER_UNIT, 0x0022, WORD)	// Sets digital attenuation.

MKTAG (AUDMIX_MONOOUT_MUTE, 					AUDIO_MIXER_UNIT, 0x0023, BOOL)	// Mutes mono output.
MKTAG (AUDMIX_MONOIN_MUTE, 					AUDIO_MIXER_UNIT, 0x0024, BOOL)	// Mutes mono input.
MKTAG (AUDMIX_MONO_ATTENUATION, 				AUDIO_MIXER_UNIT, 0x0025, BOOL)	// Mono output attenuation.

// General tags for Audio Fader
//
MKTAG (AUDMIX_LOUDNESS,							AUDIO_MIXER_UNIT, 0x2001, BOOL)
MKTAG (AUDMIX_ZERO_CROSSING,					AUDIO_MIXER_UNIT, 0x2002, BOOL)

// Affect both front and rear fader in the right proportion as necessary for Buster
MKTAG (AUDMIX_FADER_LEFT,						AUDIO_MIXER_UNIT, 0x2003, WORD)
MKTAG (AUDMIX_FADER_RIGHT,						AUDIO_MIXER_UNIT, 0x2004, WORD)

MKTAG (AUDMIX_BASS,								AUDIO_MIXER_UNIT, 0x2007, int)
MKTAG (AUDMIX_TREBLE,							AUDIO_MIXER_UNIT, 0x2008, int)

//
// Special tags for Microsoft Sound System Audio Mixer Device
// Their execution will directly affect the physical unit.
//
//          -- DO NOT USE EXCEPT FOR THIS PURPOSE!! --
//
MKTAG (AUDMIX_D_GAIN,								AUDIO_MIXER_UNIT, 0x0026, WORD)
MKTAG (AUDMIX_D_GAIN_LEFT,							AUDIO_MIXER_UNIT, 0x0027, WORD)
MKTAG (AUDMIX_D_GAIN_RIGHT,						AUDIO_MIXER_UNIT, 0x0028, WORD)
MKTAG (AUDMIX_D_ATTENUATE,							AUDIO_MIXER_UNIT, 0x0029, WORD)
MKTAG (AUDMIX_D_ATTENUATE_LEFT,					AUDIO_MIXER_UNIT, 0x002a, WORD)
MKTAG (AUDMIX_D_ATTENUATE_RIGHT,					AUDIO_MIXER_UNIT, 0x002b, WORD)
MKTAG (AUDMIX_D_MUTE,								AUDIO_MIXER_UNIT, 0x002c, BOOL)
MKTAG (AUDMIX_D_MUTE_LEFT,							AUDIO_MIXER_UNIT, 0x002d, BOOL)
MKTAG (AUDMIX_D_MUTE_RIGHT,						AUDIO_MIXER_UNIT, 0x002e, BOOL)
MKTAG	(AUDMIX_D_INPUT_SELECT,						AUDIO_MIXER_UNIT, 0x002f, int)	// Selects the affected input (see above)
MKTAG (AUDMIX_D_INPUT_MUTE,						AUDIO_MIXER_UNIT, 0x0030, BOOL)
MKTAG (AUDMIX_D_INPUT_MUTE_LEFT,					AUDIO_MIXER_UNIT, 0x0031, BOOL)
MKTAG (AUDMIX_D_INPUT_MUTE_RIGHT,				AUDIO_MIXER_UNIT, 0x0032, BOOL)
MKTAG (AUDMIX_D_LOUDNESS,							AUDIO_MIXER_UNIT, 0x200b, BOOL)
MKTAG (AUDMIX_D_FADER_LEFT,						AUDIO_MIXER_UNIT, 0x200c, WORD)
MKTAG (AUDMIX_D_FADER_RIGHT,						AUDIO_MIXER_UNIT, 0x200d, WORD)
MKTAG (AUDMIX_D_BASS,								AUDIO_MIXER_UNIT, 0x200e, WORD)
MKTAG (AUDMIX_D_TREBLE,								AUDIO_MIXER_UNIT, 0x200f, WORD)
MKTAG (AUDMIX_D_INPUT,								AUDIO_MIXER_UNIT, 0x2010, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_LEFT,				AUDIO_MIXER_UNIT, 0x2011, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_RIGHT,				AUDIO_MIXER_UNIT, 0x2012, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_FRONT_LEFT,		AUDIO_MIXER_UNIT, 0x2013, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_FRONT_RIGHT,		AUDIO_MIXER_UNIT, 0x2014, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_REAR_LEFT,			AUDIO_MIXER_UNIT, 0x2015, WORD)
MKTAG (AUDMIX_D_INPUT_FADER_REAR_RIGHT,		AUDIO_MIXER_UNIT, 0x2016, WORD)

// Additional tags for faders and/or mixers.
//

// New additional fader properties
MKTAG (AUDMIX_FADER_FRONT_LEFT,					AUDIO_MIXER_UNIT, 0x2030, WORD)
MKTAG (AUDMIX_FADER_FRONT_RIGHT,					AUDIO_MIXER_UNIT, 0x2031, WORD)
MKTAG (AUDMIX_FADER_REAR_LEFT,					AUDIO_MIXER_UNIT, 0x2032, WORD)
MKTAG (AUDMIX_FADER_REAR_RIGHT,					AUDIO_MIXER_UNIT, 0x2033, WORD)

MKTAG (AUDMIX_VOLUME,								AUDIO_MIXER_UNIT, 0x2050, WORD)
MKTAG (AUDMIX_VOLUME_LEFT,							AUDIO_MIXER_UNIT, 0x2003, WORD)	// Note: same as FADER_LEFT
MKTAG (AUDMIX_VOLUME_RIGHT,						AUDIO_MIXER_UNIT, 0x2004, WORD)

MKTAG (AUDMIX_FADER_FRONT,							AUDIO_MIXER_UNIT, 0x2053, WORD)
MKTAG (AUDMIX_FADER_REAR,							AUDIO_MIXER_UNIT, 0x2054, WORD)
MKTAG (AUDMIX_FADER_FRONT_MUTE,					AUDIO_MIXER_UNIT, 0x2055, WORD)
MKTAG (AUDMIX_FADER_REAR_MUTE,					AUDIO_MIXER_UNIT, 0x2056, WORD)

// Additional Direct Tags for faders and/or mixers
MKTAG (AUDMIX_D_INPUT_VOLUME,						AUDIO_MIXER_UNIT, 0x2057, WORD)
MKTAG (AUDMIX_D_INPUT_VOLUME_LEFT,				AUDIO_MIXER_UNIT, 0x2058, WORD)
MKTAG (AUDMIX_D_INPUT_VOLUME_RIGHT,				AUDIO_MIXER_UNIT, 0x2059, WORD)
MKTAG (AUDMIX_D_VOLUME,								AUDIO_MIXER_UNIT, 0x205a, WORD)
MKTAG (AUDMIX_D_VOLUME_LEFT,						AUDIO_MIXER_UNIT, 0x205b, WORD)
MKTAG (AUDMIX_D_VOLUME_RIGHT,						AUDIO_MIXER_UNIT, 0x205c, WORD)

// Tag for Installation of a Direct-Tag-Change-Hook
// Set to NULL to remove the hook.
MKTAG (AUDMIX_D_DTAG_CHANGE_HOOK,				AUDIO_MIXER_UNIT, 0x205d, DirectTagChangeHookPtr)

// Identified change hook
MKTAG (AUDMIX_D_ID_NOTIFICATION,					AUDIO_MIXER_UNIT, 0x205d, IdentifiedNotificationPtr)


#ifndef ONLY_EXTERNAL_VISIBLE

#if NT_KERNEL

class IDNotification : public Node
	{
	public:
		DWORD							id;
		NotificationHookHandle	hookHandle;

		IDNotification(DWORD id, NotificationHookHandle hookHandle) {this->id = id; this->hookHandle = hookHandle;}
	};

typedef IDNotification * IDNotificationPtr;

class IDNotificationList : public List
	{
	public:
		~IDNotificationList();
		IDNotificationPtr FindByID(DWORD id);
	};

#endif

class AudioMixer : public PhysicalUnit {
	friend class VirtualAudioMixer;
	protected:
		WORD							numInputs;
		MixerInputArrayPtr		inputs;
		int							currentInput;

		virtual Error SetCurrentInput(int currentInput);
		virtual Error ProcessChanges() = 0;

#if NT_KERNEL
		IDNotificationList	*notificationList;

		virtual Error SetIdentifiedNotification(IdentifiedNotificationPtr notification);
		virtual Error SendNotifications(DWORD senderID, DWORD type, DWORD msg, DWORD data);
#endif
	public:
		AudioMixer();
		~AudioMixer();

		virtual Error RetrieveINIValues(void) {GNRAISE_OK;}

		virtual VirtualAudioMixer * CreateVirtualAudioMixer(void) {return NULL;}
		VirtualUnit * CreateVirtual(void);
	};

class VirtualAudioMixer : public VirtualUnit {
	private:
		AudioMixer			*	mixer;
	protected:
		WORD	curInput;

		Error Preempt(VirtualUnit *previous);
	public:
		VirtualAudioMixer(AudioMixer *physical) : VirtualUnit(physical) {this->mixer = physical;}
		Error Configure(TAG __far * tags);

#if NT_KERNEL
		virtual Error SendNotifications(DWORD senderID, DWORD type, DWORD msg, DWORD data);
#endif
	};

inline VirtualUnit * AudioMixer::CreateVirtual(void) {return CreateVirtualAudioMixer();}

#endif // ONLY_EXTERNAL_VISIBLE

#endif
