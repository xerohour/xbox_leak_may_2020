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

// FILE:       library\hardware\mpegenc\generic\mpegenc.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		26.10.1999
//
// PURPOSE: 	MPEG encoder and supporting classes. --- Header file
//
// HISTORY:
//

#ifndef MPEGENC_H
#define MPEGENC_H

#include "library/common/vddebug.h"
#include "library/common/tags.h"
#include "library/common/hooks.h"
#include "library/common/tagunits.h"
#include "library/common/virtunit.h"
#include "library/hardware/video/generic/vidtypes.h"
#include "library/hardware/mpgcodec/generic/mpgcodec.h"


struct MPEGTransferDoneMsg {
	};

struct MPEGEncoderBufferStarvingMsg {
	};


MKHOOK(MPEGTransferDone, MPEGTransferDoneMsg)
MKHOOK(MPEGEncoderBufferStarving, MPEGEncoderBufferStarvingMsg)


typedef MPEGTransferDoneHook	__far *	MPEGTransferDoneHookPtr;
typedef MPEGEncoderBufferStarvingHook __far *  MPEGEncoderBufferStarvingHookPtr;


enum MPEGEncoderStreamType
	{
	MST_UNKNOWN					= 0x00,
	MST_AUDIO_ELEMENTARY		= 0x01,
	MST_VIDEO_ELEMENTARY		= 0x02,
	MST_PROGRAM					= 0x03,
	MST_TRANSPORT				= 0x04,
	MST_PACKET_ELEMENTARY	= 0x05,
	MST_PACKET_VIDEO_ES		= 0x06,
	MST_PACKET_AUDIO_ES		= 0x07,
	MST_MPEG1					= 0x08
	};


MKTAG(MPEG_TRANSFER_DONE_HOOK,		MPEG_ENCODER_UNIT,	0x0001, MPEGTransferDoneHookPtr)
MKTAG(MPEG_ENCODER_BUFFER_STARVING, MPEG_ENCODER_UNIT,   0x0002, MPEGEncoderBufferStarvingHookPtr)
MKTAG(MPEG_ENC_OUTPUT_TYPE,			MPEG_ENCODER_UNIT,   0x0003, MPEGEncoderStreamType)
MKTAG(MPEG_ENC_VIDEO_SOURCE,			MPEG_ENCODER_UNIT,	0x0004, VideoSource)
MKTAG(MPEG_ENC_VIDEOSTANDARD,			MPEG_ENCODER_UNIT,	0x0005, VideoStandard)
//MKTAG(MPEG_ENC_VIDEO_WIDTH,	MPEG_ENCODER_UNIT,	0x0002, WORD)
//MKTAG(MPEG_ENC_VIDEO_HEIGHT,  MPEG_ENCODER_UNIT,   0x0003, WORD)

//
// Previous definition conflicted with decoder definition.
// Should move shared defines to hardware\mpgcodec\generic\mpgcodec.h"
//
//#include "library\hardware\mpegdec\generic\mpegdec.h"

#endif // MPEGENC_H

