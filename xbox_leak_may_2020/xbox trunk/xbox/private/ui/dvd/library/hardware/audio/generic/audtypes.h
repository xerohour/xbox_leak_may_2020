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

// FILE:			library\hardware\audio\generic\audtypes.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		30.11.95
//
// PURPOSE:		Common audio data types.
//
// HISTORY:

#ifndef AUDTYPES_H
#define AUDTYPES_H


//
// Supported sample rates for codecs
//
enum AudioSampleRate		{
									RATE_11_025,				// 11.025 kHz
									RATE_22_05,					// 22.050 kHz
									RATE_44_10,					// 44.100 kHz
									RATE_8,						//  8.000 kHz
									RATE_32_0,					// 32.000 kHz
									RATE_48_0					// 48.000 kHz
								};

//
// Supported data formats for codecs
//
enum AudioDataFormat		{
									UNSIGNED_8 		= 0,		// 8 bit Unsigned
									ULAW_8,						// 8 bit u-Law encoded
									SIGNED_16_LE,				// 16 bit Signed Little Endian
									ALAW_8,						// 8 bit a-Law encoded
									ADPCM_4 			= 5,		// 4 bit ADPCM
									SIGNED_16_BE				// 16 bit Signed Big Endian
								};
//
// Distinction between stereo and mono for all audio devices
//
enum AudioMode 			{
									AUDIO_MONO,
									AUDIO_STEREO
								};

//
// Distinction between channels for all audio devices
//
enum AudioChannel 		{
									CHANNEL_LEFT,
									CHANNEL_RIGHT,
									CHANNEL_BOTH,					// Both left & right channel affected
									CHANNEL_CENTER,
									CHANNEL_SURROUND_LEFT,
									CHANNEL_SURROUND_RIGHT,
									CHANNEL_SUBWOOFER,
									CHANNEL_ALL_SURROUND			// All channels affected
								};

//
// Defines if front or rear fader
//
enum FaderType {FADER_FRONT, FADER_REAR};

//
// Possible values for Overrange Detection
//
enum AudioOverrange		{
									LESS_MINUS_1_DB,
									MINUS_1_TO_ZERO_DB,
									ZERO_TO_1_DB,
									GREATER_1_DB
								};

#endif
