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

// FILE:       Driver/Dvdpldrv/Common/MPEGAudioTypes.h
// AUTHOR:     Sam Frantz
// COPYRIGHT:  (c) 2000 Ravisent Technologies, Inc.
// CREATED:    6-NOV-2000
//
// PURPOSE:    Common MPEG audio data types.
//
// HISTORY:

#ifndef MPEGAUDIOTYPES_H
#define MPEGAUDIOTYPES_H

enum MPEGAudioVersion
	{
	MPEG_AUDIO_VERSION_2_5 = 0,
	MPEG_AUDIO_VERSION_RESERVED = 1,
	MPEG_AUDIO_VERSION_2 = 2,
	MPEG_AUDIO_VERSION_1	= 3
	};

// MPEGAudioLayer is defined in "library\hardware\mpegdec\generic\mpegdec.h" as:
//
// enum MPEGAudioLayer
//        {
//        MPAUDLAYER_1,
//        MPAUDLAYER_2,
//        MPAUDLAYER_3                            // = MP3
//        };

enum MPEGAudioLayerDescription
	{
	MPEG_AUDIO_LAYER_1 = 1,
	MPEG_AUDIO_LAYER_2 = 2,
	MPEG_AUDIO_LAYER_3 = 3, // = MP3
	MPEG_AUDIO_LAYER_RESERVED = 4
	};

enum MPEGAudioChannelMode
	{
	MPEG_AUDIO_MODE_STEREO = 0,
	MPEG_AUDIO_MODE_JOINT_STEREO = 1,
	MPEG_AUDIO_MODE_DUAL_CHANNEL = 2,
	MPEG_AUDIO_MODE_SINGLE_CHANNEL = 3
	};

enum MPEGAudioJointStereoModeExtLayerIAndII
	{
	MPEG_AUDIO_INTENSITY_STEREO_BANDS_4_TO_31 = 0,
	MPEG_AUDIO_INTENSITY_STEREO_BANDS_8_TO_31 = 1,
	MPEG_AUDIO_INTENSITY_STEREO_BANDS_12_TO_31 = 2,
	MPEG_AUDIO_INTENSITY_STEREO_BANDS_16_TO_31 = 3
	};

enum MPEGAudioEmphasisMode
	{
	MPEG_AUDIO_EMPHASIS_NONE = 0,
	MPEG_AUDIO_EMPHASIS_50_15_MS = 1,
	MPEG_AUDIO_EMPHASIS_RESERVED = 2,
	MPEG_AUDIO_EMPHASIS_CCIT_J17 = 3
	};

#define MPEG_ID3V1_STRING_LEN		32	// 30 + 1 terminator + 1 padding byte

typedef struct
	{
	char		title[MPEG_ID3V1_STRING_LEN];
	char		artist[MPEG_ID3V1_STRING_LEN];
	char		album[MPEG_ID3V1_STRING_LEN];
	char		comment[MPEG_ID3V1_STRING_LEN];
	WORD		year;			// or NULL
	BYTE		trackNum;	// or NULL
	BYTE		genreCode;	// this is a joke
	} MPEGAudioID3V1Tag;

typedef struct
	{
	MPEGAudioVersion				mpegVersion;
	MPEGAudioLayerDescription	mpegLayer;
	DWORD								bitrate;
	DWORD								sampleRate;
	DWORD								frameLength;
	BOOL								privateBit;
	MPEGAudioChannelMode								channelMode;
	MPEGAudioJointStereoModeExtLayerIAndII		modeExtension;
	BOOL								copyrighted;
	BOOL								originalMedia;
	MPEGAudioEmphasisMode		emphasisMode;
	BOOL								bitrateSwitchingInUse;
	BOOL								bitReservoirInUse;
	BOOL								intensityStereo;
	BOOL								msStereo;
	} MPEGAudioFrameInfo;

#endif

