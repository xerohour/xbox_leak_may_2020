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

// FILE:			library\hardware\audio\generic\karaokesetup.h
// AUTHOR:		Mark Spinnenweber
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		July 28, 2000
//
// PURPOSE:		Definitions for Karaoke data types (used by Karaoke tags)
//
// HISTORY:

#ifndef KARAOKESETUP_H
#define KARAOKESETUP_H

enum KaraokeVoiceEffectType
	{
	KARAOKE_VOICE_EFFECT_NONE = 0,
	KARAOKE_VOICE_EFFECT_ECHO = 1,
	KARAOKE_VOICE_EFFECT_CHORUS = 2,
	KARAOKE_VOICE_EFFECT_REVERB = 3
	};


enum KaraokeVoiceChannelSelection
	{
	KARAOKE_MUSIC_VOICE_TO_LR = 0,
	KARAOKE_VOICE_TO_CENTER   = 1
	};


enum KaraokeWordSizeType
	{
	KARAOKE_WORD_SIZE_16bit = 0,
	KARAOKE_WORD_SIZE_18bit = 1,
	KARAOKE_WORD_SIZE_20bit = 2,
	KARAOKE_WORD_SIZE_24bit = 3
	};

enum KaraokeJustificationType
	{
	KARAOKE_START_JUSTIFIED = 0,
	KARAOKE_END_JUSITIFIED  = 1
	};

enum KaraokePCMOutputModeType
	{
	KARAOKE_PCM_OUTPUT_MODE_2Channel = 0,
	KARAOKE_PCM_OUTPUT_MODE_6Channel = 1
	};

enum KaraokePCMByteOrderType
	{
	KARAOKE_PCM_BYTE_ORDER_LSB_FIRST = 0,
	KARAOKE_PCM_BYTE_ORDER_MSB_FIRST = 1
	};

enum KaraokeVoiceSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_12kHz = 0,
	KARAOKE_SAMPLING_FREQUENCY_11_025KHz = 1,
	KARAOKE_SAMPLING_FREQUENCY_10kHz = 2
	};

enum KaraokeMusicSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_48kHz = 0,
	KARAOKE_SAMPLING_FREQUENCY_44_1KHz = 1,
	KARAOKE_SAMPLING_FREQUENCY_32kHz = 2
	};

enum KaraokeSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_256Fs = 0,
	KARAOKE_SAMPLING_FREQUENCY_384Fs = 1
	};

enum KaraokeCanInputModeType
	{
	KARAOKE_16_SLOTS_MODE = 0,
	KARAOKE_16_SLOTS_MODE_LSB_FIRST = 1,
	KARAOKE_32_SLOTS_MODE_LEFT_ALIGNED = 2,
	KARAOKE_32_SLOTS_MODE_RIGHT_ALIGNED = 3,
	KARAOKE_32_SLOTS_MODE_I2S_MODE = 4,
	KARAOKE_32_SLOTS_MODE_SIGN_EXTENDED = 5,
	KARAOKE_32_SLOTS_MODE_8_BIT_DATA = 6,
	KARAOKE_32_SLOTS_MOD16_BIT_DATA = 7
	};



#endif
