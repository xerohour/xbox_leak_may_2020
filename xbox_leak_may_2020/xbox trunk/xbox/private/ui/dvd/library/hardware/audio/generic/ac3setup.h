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

// FILE:			library\hardware\audio\generic\ac3setup.h
// AUTHOR:		U. Sigmund
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		20.11.1996
//
// PURPOSE:		Definitions for AC3 decoder setups
//
// HISTORY:

#ifndef AC3SETUP_H
#define AC3SETUP_H

enum AC3BassConfig
	{
	AC3BC_NO_REDIRECTION,
	AC3BC_REDIRECT_CENTER_BASS_TO_LEFT_AND_RIGHT,
	AC3BC_REDIRECT_LEFT_RIGHT_CENTER_BASS_TO_SUBWOOFER,
	AC3BC_REDIRECT_CENTER_BASS_TO_SUBWOOFER
	};

enum AC3SpeakerConfig
	{
	AC3SC_20_SURROUND_COMPATIBLE,
	AC3SC_10,
	AC3SC_20_NORMAL,
	AC3SC_30,
	AC3SC_21,
	AC3SC_31,
	AC3SC_22,
	AC3SC_32,
	AC3SC_KARAOKE		// This may only be available for MPEG-2 audio, but it is
							// added to the AC3 settings to avoid introducing new tags
	};

enum AC3DualModeConfig
	{
	AC3DMDM_DEFAULT,	// default is stereo
	AC3DMDM_STEREO = AC3DMDM_DEFAULT,
	AC3DMDM_CHANNEL1,	// channel 1 on both L/R
	AC3DMDM_CHANNEL2, // channel 2 on both L/R
	AC3DMDM_MIX			// mix channel 1 and 2 to mono, output on both L/R
	};

enum AC3KaraokeConfig
	{
	AC3KARA_AWARE,
	AC3KARA_DEFAULT,
	AC3KARA_OFF = AC3KARA_DEFAULT,
	AC3KARA_MULTICHANNEL	= 3,
	AC3KARA_CAPABLE_NO_VOCAL,
	AC3KARA_CAPABLE_V1,
	AC3KARA_CAPABLE_V2,
	AC3KARA_CAPABLE_BOTH_VOCAL
	};

enum DolbyProLogicConfig
	{
	DPLCFG_OFF,
	DPLCFG_DEFAULT = DPLCFG_OFF,
	DPLCFG_3_0 = 3,	// LCR three stereo
	DPLCFG_2_1,			// LRS phantom (= center not used)
	DPLCFG_3_1,			// LCRS
	DPLCFG_2_2,			// LRSS phantom
	DPLCFG_3_2			// LCRSS
	};

enum DolbyProLogicMode
	{
	DPLMODE_AUTOMATIC,	// Decoding depends on input fed into the PL decoder
	DPLMODE_DEFAULT = DPLMODE_AUTOMATIC,
	DPLMODE_ON,				// force it always on
	DPLMODE_OFF				// always off
	};

enum AC3DynamicRange
	{
	AC3DYNRG_DEFAULT,
	AC3DYNRG_COMPRESSED = AC3DYNRG_DEFAULT,	// Dynamic range sclae factor taken from MPEG-2 AC3 stream
	AC3DYNRG_MAXIMUM,									// Always use full dynamic range
	AC3DYNRG_MINIMUM,
	AC3DYNRG_TV
	};

// AC3 "acmod" property
enum AC3AudioCodingMode
	{
	AC3ACMOD_DUALMONO,
	AC3ACMOD_1_0,
	AC3ACMOD_2_0,
	AC3ACMOD_3_0,
	AC3ACMOD_2_1,
	AC3ACMOD_3_1,
	AC3ACMOD_2_2,
	AC3ACMOD_3_2,
	AC3ACMOD_UNKNOWN
	};

enum AC3DialogNorm
    {
	AC3DNORM_DEFAULT,
	AC3DNORM_ON = AC3DNORM_DEFAULT,
	AC3DNORM_OFF
	};

enum AC3OperationalMode
	{
	AC3OPMODE_DEFAULT,
	AC3OPMODE_LINE = AC3OPMODE_DEFAULT,
	AC3OPMODE_RF,
	AC3OPMODE_CUSTOM_0,
	AC3OPMODE_CUSTOM_1
	};





#endif
