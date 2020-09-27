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

#ifndef XBOXPROFILE_H
#define XBOXPROFILE_H

#include "library\common\profiles.h"
#include "..\AC3Decoder\AC3Decoder.h"
#include "..\Video\XBoxDisplay.h"

#define XBOX_CONFIG														\
	{																	\
	{"$", SET_SECTION, 0},												\
																		\
		{"DRIVE", SET_SECTION, 0},										\
		{"", SET_END, 0},												\
																		\
		{"Display", SET_SECTION, 0},									\
			{"EnforceFlipSync", SET_VALUE, FALSE},						\
			{"MaxPrimaryDecodeWidth", SET_VALUE, 1600},					\
			{"AlwaysNeedsColorkey", SET_VALUE, FALSE},					\
			{"InvertFlipOddEven", SET_VALUE, FALSE},					\
			{"BobWeaveSwitchDelay", SET_VALUE, FALSE},					\
			{"IgnoreDownscalingCaps", SET_VALUE, FALSE},				\
			{"HasDownscalingBob", SET_VALUE, TRUE},						\
			{"FrameFreezeForFieldContent", SET_VALUE, false},			\
			{"SyncWithScreenRefresh", SET_VALUE, false},				\
			{"DisableNoSysLock", SET_VALUE, false},						\
			{"SurfaceFlipMode", SET_VALUE, 0xFFFFFFFF},					\
		{"", SET_END, 0},												\
																		\
		{"VideoDecoder", SET_SECTION, 0},								\
			{"PerformanceClass", SET_VALUE, 0xffffffff},				\
			{"HardwareOptimization", SET_VALUE, 0},						\
			{"BobWeave", SET_VALUE, 4},									\
			{"PullDownReconstruction", SET_VALUE, TRUE},				\
			{"AlwaysHurryUp", SET_VALUE, FALSE},						\
			{"FrameDropIncrement", SET_VALUE, 0},						\
		{"", SET_END, 0},												\
																		\
		{"Scheduler", SET_SECTION, 0},									\
			{"STCDivider", SET_VALUE, 0},								\
			{"MinSleepTime", SET_VALUE, 0},								\
			{"BufferSleepTime", SET_VALUE, 4},							\
			{"MaxSleepSkew", SET_VALUE, 2},								\
			{"DynamicSleepAdjust", SET_VALUE, TRUE},					\
			{"TimeSource", SET_VALUE, FALSE},							\
		{"", SET_END, 0},												\
																		\
		{"AudioDecoder", SET_SECTION, 0},								\
			{"dualModeConfig", SET_VALUE, AC3DMDM_DEFAULT},				\
			{"karaokeConfig", SET_VALUE, AC3KARA_AWARE},				\
			{"UseLFE", SET_VALUE, FALSE},								\
		{"", SET_END, 0},												\
																		\
		{"Regionalization", SET_SECTION, 0},							\
			{"Changes", SET_VALUE, 0},									\
			{"regionMagic", SET_VALUE, RegionCodes[0]},					\
			{"RegionOverride", SET_VALUE, 0},							\
		{"", SET_END, 0},												\
																		\
		{"VIDEODECODER", SET_SECTION, 0},								\
		{"", SET_END, 0},												\
																		\
	{"", SET_END, 0},													\
	}																	\


enum SetupEntryType
	{
	SET_VALUE,
	SET_SECTION,
	SET_END
	};


// One entry of the setup array
struct SetupEntry
	{
	char    * name;
	SetupEntryType type;
	DWORD    value;
	};



SetupEntry config[] = XBOX_CONFIG;



static DWORD CalculateID(BOOL section, KernelString name)
	{
	DWORD temp = 0;

	if (name.Length() > 0)
		{
		for (int i = 0; i < name.Length(); i++)
			{
			temp ^= name[i];
			temp = (temp << 3) | (temp >> 29); // The first 10 characters count
			}

		return (section ? (temp |= 1) : (temp &= ~1));
		}
	else
		return 0;
	}


void ParseSetup(SetupEntry * & sentry, ProfileEntry * pentry, int & num)
	{
	int snum;

	num = 0;
	pentry->id = CalculateID(TRUE, sentry->name);
	num++;
	sentry++;
	while (sentry->type != SET_END)
		{
		if (sentry->type == SET_VALUE)
			{
			pentry[num].id = CalculateID(FALSE, sentry->name);
			pentry[num].value = sentry->value;
			num++;
			sentry++;
			}
		else
			{
			ParseSetup(sentry, pentry + num, snum);
			num += snum;
			}
		}
	pentry->value = num;

	sentry++;
	}


class XboxProfile :public StaticProfile
	{
	private:
		ProfileEntry profileEntry[60];	//currently have 50

	public:
		XboxProfile::XboxProfile() : StaticProfile((const ProfileEntry*)&profileEntry)
			{
			int num = 0;
			SetupEntry * se = config;

			ParseSetup(se, &profileEntry[0], num);
			}

	};





#endif XBOXPROFILE_H
