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


// FILE:      library\common\tagunits.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   24.03.95
//
// PURPOSE: The tag units.
//
// HISTORY:

#ifndef TAGUNITS_H
#define TAGUNITS_H

#include "Tags.h"

MKTAGUNIT(PIP_UNIT,            0)
MKTAGUNIT(VIDEO_ENCODER_UNIT,  1)
MKTAGUNIT(VIDEO_DECODER_UNIT,  2)
MKTAGUNIT(MPEG_DECODER_UNIT,   3)
MKTAGUNIT(TV_TUNER_UNIT,       4)
MKTAGUNIT(AUDIO_MIXER_UNIT,    5)
MKTAGUNIT(AUDIO_DECODER_UNIT,  6)
MKTAGUNIT(AUDIO_ENCODER_UNIT,  7)
MKTAGUNIT(JPEG_CODEC_UNIT,     8)
MKTAGUNIT(OSD_UNIT,				 9)
MKTAGUNIT(PANEL_UNIT,         10)
MKTAGUNIT(COMMUNICATION_UNIT, 11)
// DO NOT USE 12 or 13 due to contention with tag type
MKTAGUNIT(MPEG_ENCODER_UNIT,  14)

#define VIDEO_CHIP_UNIT			(VIDEO_ENCODER_UNIT | VIDEO_DECODER_UNIT)
#define AUDIO_STREAMDEV_UNIT	(AUDIO_ENCODER_UNIT | AUDIO_DECODER_UNIT)

#define NUM_UNITS		12

#ifndef ONLY_EXTERNAL_VISIBLE

// Only internally used tag units
MKITAGUNIT(AUDIO_DAC_UNIT,			1)
MKITAGUNIT(STEREO_DECODER_UNIT,	2)
MKITAGUNIT(VIDEO_DEVICE_UNIT,		3)
MKITAGUNIT(PLL_UNIT,				   4)


#endif // external visible

#endif
