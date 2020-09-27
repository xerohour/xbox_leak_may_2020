// FILE:			library\hardware\audio\generic\ac3mix.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		20.11.1996
//
// PURPOSE:		Defintions for common Dolby-AC3 capable mixers.
//
// HISTORY:

#ifndef AC3MIX_H
#define AC3MIX_H

#include "audmix.h"
#include "ac3setup.h"

// Additional flags for Direct Tag Change Hook Messages
#define DTCM_CHANGED_GENERAL_AC3				0x01000000		// General AC3 settings were changed

// Input specific flags for DTC Hook Messages
#define DTCM_CHANGED_INPUTS_GENERAL_AC3	0x10000000		// General AC3 settings for input were changed

//
// Extended AC3 / ProLogic input Capabilities
//

#define INPUT_AC3						0x01000000		// The input can carry AC3 surround sound.
#define INPUT_PROLOGIC				0x02000000		// The input can carry ProLogic surround sound.

//
// Tags to control the AC3 setup. Direct tags, only to be called from mixing applications.
//

MKTAG (AUDMIX_D_AC3_SPEAKER_CONFIG,					AUDIO_MIXER_UNIT, 0x1000, AC3SpeakerConfig)
MKTAG (AUDMIX_D_AC3_BASS_CONFIG,						AUDIO_MIXER_UNIT, 0x1001, AC3BassConfig)
MKTAG (AUDMIX_D_AC3_SPEAKER_DELAY_CENTER,			AUDIO_MIXER_UNIT, 0x1002, WORD)
MKTAG (AUDMIX_D_AC3_SPEAKER_DELAY_SURROUND,		AUDIO_MIXER_UNIT, 0x1003, WORD)

// AC3 setup tags for individual inputs.

MKTAG (AUDMIX_D_INPUT_AC3_SPEAKER_CONFIG,				AUDIO_MIXER_UNIT, 0x1010, AC3SpeakerConfig)
MKTAG (AUDMIX_D_INPUT_AC3_BASS_CONFIG,					AUDIO_MIXER_UNIT, 0x1011, AC3BassConfig)
MKTAG (AUDMIX_D_INPUT_AC3_SPEAKER_DELAY_CENTER,		AUDIO_MIXER_UNIT, 0x1012, WORD)
MKTAG (AUDMIX_D_INPUT_AC3_SPEAKER_DELAY_SURROUND,	AUDIO_MIXER_UNIT, 0x1013, WORD)

//
// Additional volume tags to control AC3 channels
//

MKTAG (AUDMIX_VOLUME_CENTER,							AUDIO_MIXER_UNIT, 0x1020, WORD)
MKTAG (AUDMIX_VOLUME_SURROUND_LEFT,					AUDIO_MIXER_UNIT, 0x1021, WORD)
MKTAG (AUDMIX_VOLUME_SURROUND_RIGHT,				AUDIO_MIXER_UNIT, 0x1022, WORD)
MKTAG (AUDMIX_VOLUME_SUBWOOFER,						AUDIO_MIXER_UNIT, 0x1023, WORD)

// Direct access versions of the additional volume tags.

MKTAG (AUDMIX_D_VOLUME_CENTER,						AUDIO_MIXER_UNIT, 0x1030, WORD)
MKTAG (AUDMIX_D_VOLUME_SURROUND_LEFT,				AUDIO_MIXER_UNIT, 0x1031, WORD)
MKTAG (AUDMIX_D_VOLUME_SURROUND_RIGHT,				AUDIO_MIXER_UNIT, 0x1032, WORD)
MKTAG (AUDMIX_D_VOLUME_SUBWOOFER,					AUDIO_MIXER_UNIT, 0x1033, WORD)

// The same as above, but for a specific input set with AUDMIX_D_INPUT_SELECT
MKTAG (AUDMIX_D_INPUT_VOLUME_CENTER,				AUDIO_MIXER_UNIT, 0x1034, WORD)
MKTAG (AUDMIX_D_INPUT_VOLUME_SURROUND_LEFT,		AUDIO_MIXER_UNIT, 0x1035, WORD)
MKTAG (AUDMIX_D_INPUT_VOLUME_SURROUND_RIGHT,		AUDIO_MIXER_UNIT, 0x1036, WORD)
MKTAG (AUDMIX_D_INPUT_VOLUME_SUBWOOFER,			AUDIO_MIXER_UNIT, 0x1037, WORD)

#endif
