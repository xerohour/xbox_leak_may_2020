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

#include "MacrovisionEncoder.h"
#include "tvout.h"
#include "av.h"


static const GUID BroeselGuid = { 0x2c62061, 0x1097, 0x11d1, { 0x92, 0xf, 0x0, 0xa0, 0x24, 0xdf, 0x15, 0x6e } };


extern "C"
	{
	extern volatile DWORD *D3D__GpuReg;
	}


WindowsMacrovisionEncoder::WindowsMacrovisionEncoder(void)
	{

	ULONG Result;

	prevMacrovisionLevel = 0;
	AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_MACROVISION_MODE, 0, &Result);

	}

WindowsMacrovisionEncoder::~WindowsMacrovisionEncoder(void)
	{

	ULONG Result;

	D3DDevice_BlockUntilVerticalBlank();
	AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_MACROVISION_MODE, 0, &Result);

	D3DDevice_BlockUntilVerticalBlank();
	AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_MACROVISION_COMMIT, 0, &Result);


	}


bool WindowsMacrovisionEncoder::Initialize(void)
	{
	return TRUE;
	}

bool WindowsMacrovisionEncoder::SetMacrovisionLevel(int level)
	{

	ULONG Result;
    char stringBuffer[200];

	if (level != prevMacrovisionLevel)
		{
	    wsprintf(stringBuffer, "Set Macrovision level %d\n", level);
		OutputDebugString(stringBuffer);

		prevMacrovisionLevel = level;

		D3DDevice_BlockUntilVerticalBlank();
		AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_MACROVISION_MODE, level, &Result);

		D3DDevice_BlockUntilVerticalBlank();
		AvSendTVEncoderOption((void *)D3D__GpuReg, AV_OPTION_MACROVISION_COMMIT, level, &Result);

		}

	return TRUE;
	}


