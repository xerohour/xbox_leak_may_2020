////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// FILE:      library\hardware\video\generic\viddec.cpp
// AUTHOR:    Ulrich Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   16.08.95
//
// PURPOSE: The abstract class for video decoder programming.
//
// HISTORY:

#include "viddec.h"

Error VideoDecoder::Lock(VirtualUnit * unit)
	{
	Error err = GNR_OK;

	if (videoBus && IS_ERROR(err = videoBus->Lock()))
		{
		}
	else if (IS_ERROR(err = VideoChip::Lock(unit)))
		{
		if (videoBus)
			videoBus->Unlock();
		}

	return err;
	}

Error VideoDecoder::Unlock(VirtualUnit * unit)
	{
	if (videoBus)
		GNREASSERT(videoBus->Unlock());
	GNREASSERT(VideoChip::Unlock(unit));

	GNRAISE_OK;
	}

Error VirtualVideoDecoder::Configure(TAG __far * tags)
	{
	GNREASSERT(VirtualVideoChip::Configure(tags));

	PARSE_TAGS_START(tags)
		GETSETVC(DEC_INPUT,		input,		GNREASSERT(decoder->SetInput(input)));
		GETSETVC(DEC_BRIGHTNESS,brightness, GNREASSERT(decoder->SetBrightness(brightness)));
		GETSETVC(DEC_CONTRAST,	contrast,	GNREASSERT(decoder->SetContrast(contrast)));
		GETSETVC(DEC_SATURATION,saturation,	GNREASSERT(decoder->SetSaturation(saturation)));
		GETSETVC(DEC_HUE,			hue,			GNREASSERT(decoder->SetHue(hue)));
		GETSETVC(DEC_SAMPLEMODE,sampleMode,	GNREASSERT(decoder->SetSampleMode(sampleMode)));
		GETSETVC(DEC_TELETEXT,	teletext,	GNREASSERT(decoder->SetTeletextMode(teletext)));

		GETONLY(DEC_INPUT_NUMBER, decoder->numInputs);
#if NT_KERNEL || VXD_VERSION
		GETINQUIRE(DEC_INPUT_NAMES,
				memcpy (data, decoder->inputNames, sizeof(VideoDecoderInputEntry)*decoder->numInputs);
				);
#else
		GETINQUIRE(DEC_INPUT_NAMES,
				hmemcpy (data, decoder->inputNames, sizeof(VideoDecoderInputEntry)*decoder->numInputs);
				);
#endif

		GETINQUIRE(DEC_DETECT_STD, if (GetState() == current) return decoder->DetectVideoStandard(data); else GNRAISE(GNR_OBJECT_NOT_CURRENT););
	PARSE_TAGS_END;

	GNRAISE_OK;
	}

Error VirtualVideoDecoder::Preempt(VirtualUnit *previous)
	{
	GNREASSERT(VirtualVideoChip::Preempt(previous));

	PREEMPT_START(VirtualVideoDecoder)
		PREEMPT_UPDATE(input,		GNREASSERT(decoder->SetInput(input)));
		PREEMPT_UPDATE(brightness, GNREASSERT(decoder->SetBrightness(brightness)));
		PREEMPT_UPDATE(contrast,	GNREASSERT(decoder->SetContrast(contrast)));
		PREEMPT_UPDATE(saturation,	GNREASSERT(decoder->SetSaturation(saturation)));
		PREEMPT_UPDATE(hue,			GNREASSERT(decoder->SetHue(hue)));
		PREEMPT_UPDATE(sampleMode, GNREASSERT(decoder->SetSampleMode(sampleMode)));
		PREEMPT_UPDATE(teletext,	GNREASSERT(decoder->SetTeletextMode(teletext)));
	PREEMPT_END;

	GNRAISE_OK;
	}

VirtualVideoDecoder::VirtualVideoDecoder(VideoDecoder	*	physical)
		 : VirtualVideoChip(physical)
	{
	decoder = physical;
	input = 0;
	brightness = 5216;
	contrast = 5748;
	saturation = 5000;
	hue = 5000;
	sampleMode = VSAMOD_CCIR;
	teletext = FALSE;
	}

