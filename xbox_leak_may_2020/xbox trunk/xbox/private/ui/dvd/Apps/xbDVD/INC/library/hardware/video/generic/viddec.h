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


// FILE:      library\hardware\video\generic\viddec.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   24.03.95
//
// PURPOSE: The abstract class for video decoder programming.
//
// HISTORY:

#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "library\common\gnerrors.h"
#include "vidchip.h"


#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\hardwrio.h"

#endif

#define MAX_Brightness  10000
#define MAX_Contrast    10000
#define MAX_Saturation  10000
#define MAX_Hue         10000


#define VIDDEC_InputNameMaxSize  40

struct VideoDecoderInputEntry
	{
	TCHAR name[VIDDEC_InputNameMaxSize];
	};


// Note that the tag enumeration must start after those of VideoChipClass !

MKTAG (DEC_INPUT_NUMBER, VIDEO_DECODER_UNIT, 0x0001001, WORD)
MKTAG (DEC_INPUT_NAMES,  VIDEO_DECODER_UNIT, 0x0001002, VideoDecoderInputEntry __far *)
MKTAG (DEC_INPUT,        VIDEO_DECODER_UNIT, 0x0001003, WORD)
// Each decoder has its own number of input lines. For flexibility, you ask
// for the number of input lines by GET_DEC_INPUT_NUMBER, then allocate enough
// space to hold DEC_INPUT_NUMBER times a struct VideoDecoderInputEntry,
// then call DEC_INPUT_NAMES to get an array of these structs filled
// with DEC_INPUT_NUMBER entries.

MKTAG (DEC_BRIGHTNESS, VIDEO_DECODER_UNIT, 0x0001004, WORD)
MKTAG (DEC_CONTRAST,   VIDEO_DECODER_UNIT, 0x0001005, WORD)
MKTAG (DEC_SATURATION, VIDEO_DECODER_UNIT, 0x0001006, WORD)
MKTAG (DEC_HUE,        VIDEO_DECODER_UNIT, 0x0001007, WORD)

MKTAG (DEC_DETECT_STD, VIDEO_DECODER_UNIT, 0x0001008, VideoStandard)
MKTAG (DEC_SAMPLEMODE, VIDEO_DECODER_UNIT, 0x0001009, VideoSampleMode)
MKTAG (DEC_TELETEXT,   VIDEO_DECODER_UNIT, 0x0002001, BOOL)


// Errors from the video decoder.

#define GNR_VDEC_PARAMS		MKERR(ERROR, VDECODER, PARAMS,   0x01)
	// illegal parameters

#define GNR_VDEC_NODETECT	MKERR(ERROR, VDECODER, PARAMS,   0x02)
	// video standard not detected



#ifndef ONLY_EXTERNAL_VISIBLE


class VideoDecoder : public VideoChip
	{
	friend class VirtualVideoDecoder;

	protected:
		VirtualUnit	*videoBus;
		WORD numInputs;
		VideoDecoderInputEntry __far *inputNames;

		Error Lock (VirtualUnit *unit);
		Error Unlock (VirtualUnit *unit);

	public:
		BitInputPort *signalPort;

		VideoDecoder (VirtualUnit *videoBus, WORD numInputs = 0, VideoDecoderInputEntry __far *inputNames = NULL)
			{
			this->videoBus   = videoBus;
			this->numInputs  = numInputs;
			this->inputNames = inputNames;
			this->signalPort = NULL;
			}

		virtual Error SetInput (WORD input) = 0;
		virtual Error SetBrightness (WORD brightness) = 0;
		virtual Error SetContrast (WORD contrast) = 0;
		virtual Error SetSaturation (WORD saturation) = 0;
		virtual Error SetHue (WORD hue) = 0;
		virtual Error SetSampleMode (VideoSampleMode mode) = 0;
		virtual Error SetTeletextMode (BOOL mode) {GNRAISE_OK;}

		virtual Error DetectVideoStandard (VideoStandard __far &std) = 0;
	};


class VirtualVideoDecoder : public VirtualVideoChip
	{
	protected:
		VideoDecoder	*	decoder;

		WORD 					input;
		WORD					brightness;
		WORD					contrast;
		WORD					saturation;
		WORD					hue;
		VideoSampleMode	sampleMode;
		BOOL					teletext;

		Error Preempt(VirtualUnit *previous);

	public:
		VirtualVideoDecoder(VideoDecoder *physical);
		Error Configure(TAG __far * tags);
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
