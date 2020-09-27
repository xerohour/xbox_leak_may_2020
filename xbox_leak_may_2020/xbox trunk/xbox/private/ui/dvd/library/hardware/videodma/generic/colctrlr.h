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

// FILE:      library\hardware\videodma\generic\colctrlr.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1998 VIONA Development GmbH. All Rights Reserved.
// CREATED:   05.06.1998
//
// PURPOSE:   Abstract Color Controller class
//
// HISTORY:

#ifndef COLCTRLR_H
#define COLCTRLR_H

#include "library/common/prelude.h"

#define PCCF_BRIGHTNESS					0x00000001l
#define PCCF_CONTRAST					0x00000002l
#define PCCF_HUE							0x00000004l
#define PCCF_SATURATION					0x00000008l
#define PCCF_SHARPNESS					0x00000010l
#define PCCF_GAMMA						0x00000020l
#define PCCF_RED_BRIGHTNESS			0x00001000l
#define PCCF_BLUE_BRIGHTNESS			0x00002000l
#define PCCF_GREEN_BRIGHTNESS			0x00004000l

#ifndef ONLY_EXTERNAL_VISIBLE

struct ColorControlValues
	{
	WORD ccBrightness;
	WORD ccContrast;
	WORD ccSaturation;
	WORD ccHue;
	WORD ccGamma;
	WORD ccSharpness;
	BOOL ccColorEnable;
	WORD ccRedBrightness;
	WORD ccGreenBrightness;
	WORD ccBlueBrightness;
	};

class ColorController
	{
	public:
		virtual WORD  GetColorControl() = 0;
		virtual Error SetColorControlValues(ColorControlValues __far & clrCtrl) = 0;
	};

#endif

#endif
